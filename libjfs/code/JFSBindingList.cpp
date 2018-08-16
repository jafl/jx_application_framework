/******************************************************************************
 JFSBindingList.cpp

	BASE CLASS = JContainer

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "JFSBindingList.h"
#include "JFSBinding.h"
#include <JPrefsFile.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <sstream>
#include <jAssert.h>

#ifdef _J_OSX
static const JUtf8Byte* kGlobalBindingsFile = "/usr/local/lib/jx/jfs/file_bindings";
#else
static const JUtf8Byte* kGlobalBindingsFile = "/usr/lib/jx/jfs/file_bindings";
#endif

static const JString kUserExtensionBindingRoot("jx/jfs/file_bindings", kJFalse);
static const JString kOrigUserExtensionBindingFile("~/.systemG.filebindings", kJFalse);
static const JString kSignalFileName("~/.jx/jfs/file_bindings.signal", kJFalse);

const JFileVersion kCurrentBindingVersion = 2;

	// version 2:
	//	Converted "$f" to $q and $f to $u
	//	Removed alternate binding
	//	Added command type and "one at a time" flag
	//	Instead of "<default>", now stores blank pattern
	//	Stores misc info after version numbers
	// version 1 0:
	//	Prepended version and write_version

static const JString kDefaultCmd("jcc", kJFalse);
const JFSBinding::CommandType kDefaultCmdType = JFSBinding::kRunPlain;
const JBoolean kDefaultSingleFile             = kJFalse;

static const JUtf8Byte* kDefaultShellCmd  = "/bin/sh -c $q";
static const JUtf8Byte* kDefaultWindowCmd = "xterm -title $q -n $q -e $u";

const JSize kContentLength = 1024;

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JFSBindingList*
JFSBindingList::Create
	(
	JString* needUserCheck
	)
{
	JString signalFileName, path, name;
	if (JExpandHomeDirShortcut(kSignalFileName, &signalFileName))
		{
		JSplitPathAndName(signalFileName, &path, &name);
		if ((JCreateDirectory(path, 0700)).OK() && JDirectoryWritable(path))
			{
			JFSBindingList* list = jnew JFSBindingList(signalFileName, needUserCheck);
			assert( list != nullptr );
			return list;
			}
		}

	JFSBindingList* list = jnew JFSBindingList(JString::empty, needUserCheck);
	assert( list != nullptr );
	return list;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JFSBindingList::JFSBindingList
	(
	const JString&	signalFileName,
	JString*		needUserCheck
	)
	:
	JContainer(),
	itsUseDefaultFlag(kJFalse),
	itsUserDefault(nullptr),
	itsSystemDefault(nullptr),
	itsShellCmd(kDefaultShellCmd),
	itsWindowCmd(kDefaultWindowCmd),
	itsAutoShellFlag(kJFalse),
	itsSignalFileName(signalFileName)
{
	itsBindingList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
	assert( itsBindingList != nullptr );
	itsBindingList->SetCompareFunction(JFSBinding::ComparePatterns);

	itsOverriddenList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
	assert( itsOverriddenList != nullptr );
	itsOverriddenList->SetCompareFunction(JFSBinding::ComparePatterns);

	InstallList(itsBindingList);

	*needUserCheck = Revert();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFSBindingList::~JFSBindingList()
{
	jdelete itsBindingList;
	jdelete itsOverriddenList;
	jdelete itsUserDefault;
	jdelete itsSystemDefault;
}

/******************************************************************************
 AddBinding

	*** If you try to add a system binding, it may return index zero.
		(You should never add a system binding!)

 ******************************************************************************/

JIndex
JFSBindingList::AddBinding
	(
	const JString&					pattern,
	const JString&					cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile,
	const JBoolean					isSystem
	)
{
	JFSBinding* b = jnew JFSBinding(pattern, cmd, type, singleFile, isSystem);
	assert( b != nullptr );
	return AddBinding(b);
}

// private

JIndex
JFSBindingList::AddBinding
	(
	JFSBinding* b
	)
{
	JBoolean found;
	const JIndex index =
		itsBindingList->SearchSorted1(b, JListT::kLastMatch, &found);
	if (found)
		{
		JFSBinding* origB = itsBindingList->GetElement(index);
		if (b->IsSystemBinding() && !origB->IsSystemBinding())
			{
			if (!itsOverriddenList->InsertSorted(b, kJFalse))
				{
				jdelete b;
				return 0;
				}
			}
		else if (!b->IsSystemBinding() && origB->IsSystemBinding() &&
				 itsOverriddenList->InsertSorted(origB, kJFalse))
			{
			itsBindingList->RemoveElement(index);
			}
		else
			{
			itsBindingList->DeleteElement(index);
			}
		}

	JIndex i;
	const JBoolean inserted = itsBindingList->InsertSorted(b, kJFalse, &i);
	assert( inserted );
	return i;
}

/******************************************************************************
 DeleteBinding

	Returns kJFalse if a system binding replaces the deleted binding.

 ******************************************************************************/

JBoolean
JFSBindingList::DeleteBinding
	(
	const JIndex index
	)
{
	JFSBinding* b = itsBindingList->GetElement(index);
	if (b->IsSystemBinding())
		{
		return kJTrue;
		}

	JIndex fIndex;
	const JBoolean found =
		itsOverriddenList->SearchSorted(b, JListT::kLastMatch, &fIndex);

	itsBindingList->DeleteElement(index);
	b = nullptr;

	if (found)
		{
		b = itsOverriddenList->GetElement(fIndex);
		itsOverriddenList->RemoveElement(fIndex);

		const JBoolean inserted = itsBindingList->InsertSorted(b, kJFalse);
		assert( inserted );

		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 SetPattern

	Returns kJFalse if the pattern is already used by a different binding.
	*newIndex is always set to the new location of the element that was at
	index.

 ******************************************************************************/

JBoolean
JFSBindingList::SetPattern
	(
	const JIndex	index,
	const JString&	pattern,
	JIndex*			newIndex
	)
{
	*newIndex = index;

	JFSBinding* b = itsBindingList->GetElement(index);
	if (b->GetPattern() == pattern)
		{
		return kJTrue;
		}

	JFSBinding temp(pattern, JString::empty, JFSBinding::kRunPlain, kJFalse, kJFalse);
	JIndex fIndex;
	if (itsBindingList->SearchSorted(&temp, JListT::kAnyMatch, &fIndex))
		{
		return kJFalse;
		}

	if (b->IsSystemBinding())
		{
		JFSBinding::CommandType type;
		JBoolean singleFile;
		const JString cmd = b->GetCommand(&type, &singleFile);	// in case b is deleted
		AddBinding(pattern, cmd, type, singleFile);				// move b to itsOverriddenList
		b = &temp;
		}
	else
		{
		b->SetPattern(pattern);
		itsBindingList->Sort();
		}

	const JBoolean found =
		itsBindingList->SearchSorted(b, JListT::kAnyMatch, newIndex);
	assert( found );

	return kJTrue;
}

/******************************************************************************
 SetCommand

	Returns kJFalse if nothing changed.

 ******************************************************************************/

JBoolean
JFSBindingList::SetCommand
	(
	const JIndex	index,
	const JString&	cmd
	)
{
	JFSBinding* b = itsBindingList->GetElement(index);

	JFSBinding::CommandType type;
	JBoolean singleFile;
	if (b->GetCommand(&type, &singleFile) == cmd)
		{
		return kJFalse;
		}

	if (b->IsSystemBinding())
		{
		const JString pattern = b->GetPattern();		// in case b is deleted
		AddBinding(pattern, cmd, type, singleFile);		// move b to itsOverriddenList
		}
	else
		{
		b->SetCommand(cmd, type, singleFile);
		}

	return kJTrue;
}

/******************************************************************************
 SetCommandType

	Returns kJFalse if nothing changed.

 ******************************************************************************/

JBoolean
JFSBindingList::SetCommandType
	(
	const JIndex					index,
	const JFSBinding::CommandType	type
	)
{
	JFSBinding* b = itsBindingList->GetElement(index);
	if (b->GetCommandType() == type)
		{
		return kJFalse;
		}

	if (b->IsSystemBinding())
		{
		JFSBinding::CommandType origType;
		JBoolean singleFile;
		const JString pattern = b->GetPattern();						// in case b is deleted
		const JString cmd     = b->GetCommand(&origType, &singleFile);	// in case b is deleted
		AddBinding(pattern, cmd, type, singleFile);						// move b to itsOverriddenList
		}
	else
		{
		b->SetCommandType(type);
		}

	return kJTrue;
}

/******************************************************************************
 SetSingleFile

	Returns kJFalse if nothing changed.

 ******************************************************************************/

JBoolean
JFSBindingList::SetSingleFile
	(
	const JIndex	index,
	const JBoolean	singleFile
	)
{
	JFSBinding* b = itsBindingList->GetElement(index);
	if (b->IsSingleFileCommand() == singleFile)
		{
		return kJFalse;
		}

	if (b->IsSystemBinding())
		{
		JFSBinding::CommandType type;
		JBoolean origSingle;
		const JString pattern = b->GetPattern();					// in case b is deleted
		const JString cmd     = b->GetCommand(&type, &origSingle);	// in case b is deleted
		AddBinding(pattern, cmd, type, singleFile);					// move b to itsOverriddenList
		}
	else
		{
		b->SetSingleFileCommand(singleFile);
		}

	return kJTrue;
}

/******************************************************************************
 SetCommand

	Creates a new element if pattern isn't already registered.

 ******************************************************************************/

void
JFSBindingList::SetCommand
	(
	const JString&					pattern,
	const JString&					cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	JFSBinding* b = jnew JFSBinding(pattern, cmd, type, singleFile, kJFalse);
	assert( b != nullptr );

	JIndex index;
	if (itsBindingList->SearchSorted(b, JListT::kLastMatch, &index) &&
		!(itsBindingList->GetElement(index))->IsSystemBinding())
		{
		jdelete b;
		b = itsBindingList->GetElement(index);
		b->SetCommand(cmd, type, singleFile);
		}
	else
		{
		AddBinding(b);
		}
}

/******************************************************************************
 GetDefaultCommand

 ******************************************************************************/

const JFSBinding*
JFSBindingList::GetDefaultCommand()
	const
{
	if (itsUserDefault != nullptr)
		{
		return itsUserDefault;
		}
	else
		{
		assert( itsSystemDefault != nullptr );
		return itsSystemDefault;
		}
}

/******************************************************************************
 SetDefaultCommand

 ******************************************************************************/

void
JFSBindingList::SetDefaultCommand
	(
	const JString&					cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	if (itsUserDefault == nullptr)
		{
		itsUserDefault = jnew JFSBinding(JString::empty, cmd, type, singleFile, kJFalse);
		assert( itsUserDefault != nullptr );
		}
	else
		{
		itsUserDefault->SetCommand(cmd, type, singleFile);
		}
}

/******************************************************************************
 GetBinding

 *****************************************************************************/

JBoolean
JFSBindingList::GetBinding
	(
	const JString&		origFileName,
	const JFSBinding**	binding
	)
{
	// ignore # and ~ on end

	JString fileName = origFileName;
	CleanFileName(&fileName);

	// read content

	JString content;
	std::ifstream input(origFileName.GetBytes());
	content.Read(input, kContentLength);
	input.close();

	// scan bindings for match -- check content types first

	for (JIndex j : { 0,1 })
		{
		const JBoolean isContent = JNegate(j);
		for (const JFSBinding* b : *itsBindingList)
			{
			if (b->IsContentBinding() == isContent &&
				b->Match(fileName, content))
				{
				*binding = b;
				return kJTrue;
				}
			}
		}

	if (itsUseDefaultFlag && itsUserDefault != nullptr)
		{
		*binding = itsUserDefault;
		return kJTrue;
		}

	if (itsUseDefaultFlag && itsSystemDefault != nullptr)
		{
		*binding = itsSystemDefault;
		return kJTrue;
		}

	*binding = nullptr;
	return kJFalse;
}

/******************************************************************************
 CleanFileName (static)

	Strip # and ~ off the end.

 *****************************************************************************/

static const JRegex fileNamePattern = "[~#]+$";

void
JFSBindingList::CleanFileName
	(
	JString* s
	)
{
	JStringIterator iter(s);
	while (iter.Next(fileNamePattern))
		{
		iter.RemoveLastMatch();
		}
}

/******************************************************************************
 StoreDefault (private)

 ******************************************************************************/

inline void
JFSBindingList::StoreDefault
	(
	JFSBinding*		b,
	JFSBinding**	slot
	)
{
	jdelete *slot;
	*slot = b;
}

/******************************************************************************
 NeedsRevert

 ******************************************************************************/

JBoolean
JFSBindingList::NeedsRevert()
	const
{
	time_t t;
	return JI2B(!itsSignalFileName.IsEmpty() &&
				(JGetModificationTime(itsSignalFileName, &t)).OK() &&
				itsSignalModTime != t);
}

/******************************************************************************
 RevertIfModified

 ******************************************************************************/

void
JFSBindingList::RevertIfModified()
{
	if (NeedsRevert())
		{
		Revert();
		}
}

/******************************************************************************
 Revert

	Returns message for notifying user of upgrade from previous version.

 ******************************************************************************/

JString
JFSBindingList::Revert()
{
	JString userMsg;

	// toss everything

	itsBindingList->CleanOut();
	itsOverriddenList->CleanOut();

	jdelete itsUserDefault;
	itsUserDefault = nullptr;

	jdelete itsSystemDefault;
	itsSystemDefault = nullptr;

	// read system bindings

	std::ifstream sysInput(kGlobalBindingsFile);
	if (sysInput.good())
		{
		Load(sysInput, kJTrue);
		}
	sysInput.close();

	// read user bindings

	JPrefsFile* file = nullptr;
	if (!itsSignalFileName.IsEmpty() &&
		(JPrefsFile::Create(kUserExtensionBindingRoot, &file,
							JFileArray::kDeleteIfWaitTimeout)).OK())
		{
		if (file->IsEmpty())
			{
			JString origUserFile;
			if (JExpandHomeDirShortcut(kOrigUserExtensionBindingFile, &origUserFile) &&
				JFileReadable(origUserFile))
				{
				std::ifstream userInput(origUserFile.GetBytes());
				if (userInput.good())
					{
					Load(userInput, kJFalse);
					userMsg = JGetString("UpgradeFromVersion1::JFSBindingList");
					}
				}
			}
		else
			{
			for (JFileVersion vers = kCurrentBindingVersion; kJTrue; vers--)
				{
				if (file->IDValid(vers))
					{
					std::string data;
					file->GetData(vers, &data);
					std::istringstream input(data);
					Load(input, kJFalse);
					break;
					}

				if (vers == 0)
					{
					break;	// check *before* decrement since unsigned
					}
				}
			}

		jdelete file;
		}

	if (IsEmpty())		// nothing loaded
		{
#ifdef _J_OSX
		const JString& data = JGetString("DefaultBindingList-OSX::JFSBindingList");
#else
		const JString& data = JGetString("DefaultBindingList::JFSBindingList");
#endif
		const std::string s(data.GetBytes(), data.GetByteCount());
		std::istringstream input(s);
		Load(input, kJFalse);
		}

	if (!itsSignalFileName.IsEmpty())
		{
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}

	// ensure that GetDefaultCommand() will work

	if (itsUserDefault == nullptr && itsSystemDefault == nullptr)
		{
		SetDefaultCommand(kDefaultCmd, kDefaultCmdType, kDefaultSingleFile);
		}

	return userMsg;
}

/******************************************************************************
 Load (private)

 ******************************************************************************/

void
JFSBindingList::Load
	(
	std::istream&		input,
	const JBoolean	isSystem
	)
{
	JFileVersion vers;
	input >> vers;
	if (input.fail())
		{
		vers = 0;
		input.clear();
		}
	else if (vers == 1)
		{
		JFileVersion writeVers;
		input >> writeVers;
		}

	if (vers > kCurrentBindingVersion)
		{
		return;
		}

	if (vers >= 2)
		{
		JBoolean useDefault, autoShell;
		input >> useDefault >> autoShell;
		if (!isSystem)
			{
			itsUseDefaultFlag = useDefault;
			itsAutoShellFlag  = autoShell;
			}

		input >> itsShellCmd >> itsWindowCmd;
		}

	while (1)
		{
		JBoolean isDefault, del;
		JFSBinding* b = jnew JFSBinding(input, vers, isSystem, &isDefault, &del);
		assert( b != nullptr );

		if (input.eof() || input.fail())
			{
			jdelete b;
			break;
			}

		if (del)
			{
			jdelete b;
			}
		else if (isDefault)
			{
			StoreDefault(b, isSystem ? &itsSystemDefault : &itsUserDefault);
			}
		else
			{
			AddBinding(b);
			}
		}
}

/******************************************************************************
 Save

 ******************************************************************************/

JError
JFSBindingList::Save()
{
	JPrefsFile* file = nullptr;
	const JError err = JPrefsFile::Create(kUserExtensionBindingRoot, &file,
										  JFileArray::kDeleteIfWaitTimeout);
	if (err.OK())
		{
		std::ostringstream data;
		data << kCurrentBindingVersion;
		data << ' ' << itsUseDefaultFlag;
		data << ' ' << itsAutoShellFlag;
		data << ' ' << itsShellCmd;
		data << ' ' << itsWindowCmd;
		data << '\n';

		if (itsUserDefault != nullptr)
			{
			data << *itsUserDefault << '\n';
			}

		for (const JFSBinding* b : *itsBindingList)
			{
			if (!b->IsSystemBinding())
				{
				data << *b << '\n';
				}
			}

		file->SetData(kCurrentBindingVersion, data);
		jdelete file;

		std::ofstream touch(itsSignalFileName.GetBytes());
		touch.close();
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}

	return err;
}
