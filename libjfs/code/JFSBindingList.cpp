/******************************************************************************
 JFSBindingList.cpp

	BASE CLASS = JContainer

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "JFSBindingList.h"
#include "JFSBinding.h"
#include <JPrefsFile.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kGlobalBindingsFile           = "/usr/lib/jx/jfs/file_bindings";
static const JCharacter* kUserExtensionBindingRoot     = "jx/jfs/file_bindings";
static const JCharacter* kOrigUserExtensionBindingFile = "~/.systemG.filebindings";
static const JCharacter* kSignalFileName               = "~/.jx/jfs/file_bindings.signal";

const JFileVersion kCurrentBindingVersion = 2;

	// version 2:
	//	Converted "$f" to $q and $f to $u
	//	Removed alternate binding
	//	Added command type and "one at a time" flag
	//	Instead of "<default>", now stores blank pattern
	//	Stores misc info after version numbers
	// version 1 0:
	//	Prepended version and write_version

static const JCharacter* kDefaultCmd          = "jcc";
const JFSBinding::CommandType kDefaultCmdType = JFSBinding::kRunPlain;
const JBoolean kDefaultSingleFile             = kJFalse;

static const JCharacter* kDefaultShellCmd  = "/bin/sh -c $q";
static const JCharacter* kDefaultWindowCmd = "xterm -title $q -n $q -e $u";

const JSize kContentLength = 1024;

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JFSBindingList*
JFSBindingList::Create
	(
	const JCharacter** needUserCheck
	)
{
	JString signalFileName, path, name;
	if (JExpandHomeDirShortcut(kSignalFileName, &signalFileName))
		{
		JSplitPathAndName(signalFileName, &path, &name);
		if ((JCreateDirectory(path, 0700)).OK() && JDirectoryWritable(path))
			{
			JFSBindingList* list = jnew JFSBindingList(signalFileName, needUserCheck);
			assert( list != NULL );
			return list;
			}
		}

	JFSBindingList* list = jnew JFSBindingList("", needUserCheck);
	assert( list != NULL );
	return list;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JFSBindingList::JFSBindingList
	(
	const JCharacter*	signalFileName,
	const JCharacter**	needUserCheck
	)
	:
	JContainer(),
	itsUseDefaultFlag(kJFalse),
	itsUserDefault(NULL),
	itsSystemDefault(NULL),
	itsShellCmd(kDefaultShellCmd),
	itsWindowCmd(kDefaultWindowCmd),
	itsAutoShellFlag(kJFalse),
	itsSignalFileName(signalFileName)
{
	itsBindingList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
	assert( itsBindingList != NULL );
	itsBindingList->SetCompareFunction(JFSBinding::ComparePatterns);

	itsOverriddenList = jnew JPtrArray<JFSBinding>(JPtrArrayT::kDeleteAll);
	assert( itsOverriddenList != NULL );
	itsOverriddenList->SetCompareFunction(JFSBinding::ComparePatterns);

	InstallOrderedSet(itsBindingList);

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
	const JCharacter*				pattern,
	const JCharacter*				cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile,
	const JBoolean					isSystem
	)
{
	JFSBinding* b = jnew JFSBinding(pattern, cmd, type, singleFile, isSystem);
	assert( b != NULL );
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
		itsBindingList->SearchSorted1(b, JOrderedSetT::kLastMatch, &found);
	if (found)
		{
		JFSBinding* origB = itsBindingList->NthElement(index);
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
	JFSBinding* b = itsBindingList->NthElement(index);
	if (b->IsSystemBinding())
		{
		return kJTrue;
		}

	JIndex fIndex;
	const JBoolean found =
		itsOverriddenList->SearchSorted(b, JOrderedSetT::kLastMatch, &fIndex);

	itsBindingList->DeleteElement(index);
	b = NULL;

	if (found)
		{
		b = itsOverriddenList->NthElement(fIndex);
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
	const JIndex		index,
	const JCharacter*	pattern,
	JIndex*				newIndex
	)
{
	*newIndex = index;

	JFSBinding* b = itsBindingList->NthElement(index);
	if (b->GetPattern() == pattern)
		{
		return kJTrue;
		}

	JFSBinding temp(pattern, "", JFSBinding::kRunPlain, kJFalse, kJFalse);
	JIndex fIndex;
	if (itsBindingList->SearchSorted(&temp, JOrderedSetT::kAnyMatch, &fIndex))
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
		itsBindingList->SearchSorted(b, JOrderedSetT::kAnyMatch, newIndex);
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
	const JIndex		index,
	const JCharacter*	cmd
	)
{
	JFSBinding* b = itsBindingList->NthElement(index);

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
	JFSBinding* b = itsBindingList->NthElement(index);
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
	JFSBinding* b = itsBindingList->NthElement(index);
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
	const JCharacter*				pattern,
	const JCharacter*				cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	JFSBinding* b = jnew JFSBinding(pattern, cmd, type, singleFile, kJFalse);
	assert( b != NULL );

	JIndex index;
	if (itsBindingList->SearchSorted(b, JOrderedSetT::kLastMatch, &index) &&
		!(itsBindingList->NthElement(index))->IsSystemBinding())
		{
		jdelete b;
		b = itsBindingList->NthElement(index);
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
	if (itsUserDefault != NULL)
		{
		return itsUserDefault;
		}
	else
		{
		assert( itsSystemDefault != NULL );
		return itsSystemDefault;
		}
}

/******************************************************************************
 SetDefaultCommand

 ******************************************************************************/

void
JFSBindingList::SetDefaultCommand
	(
	const JCharacter*				cmd,
	const JFSBinding::CommandType	type,
	const JBoolean					singleFile
	)
{
	if (itsUserDefault == NULL)
		{
		itsUserDefault = jnew JFSBinding("", cmd, type, singleFile, kJFalse);
		assert( itsUserDefault != NULL );
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
	const JCharacter*	origFileName,
	const JFSBinding**	binding
	)
{
	// ignore # and ~ on end

	JString fileName = origFileName;
	CleanFileName(&fileName);

	// read content

	JString content;
	ifstream input(origFileName);
	content.Read(input, kContentLength);
	input.close();

	// scan bindings for match -- check content types first

	const JSize count = GetElementCount();
	for (JIndex j=0; j<=1; j++)
		{
		const JBoolean isContent = JNegate(j);
		for (JIndex i=1; i<=count; i++)
			{
			*binding = GetBinding(i);
			if ((**binding).IsContentBinding() == isContent &&
				(**binding).Match(fileName, content))
				{
				return kJTrue;
				}
			}
		}

	if (itsUseDefaultFlag && itsUserDefault != NULL)
		{
		*binding = itsUserDefault;
		return kJTrue;
		}

	if (itsUseDefaultFlag && itsSystemDefault != NULL)
		{
		*binding = itsSystemDefault;
		return kJTrue;
		}

	*binding = NULL;
	return kJFalse;
}

/******************************************************************************
 CleanFileName (static)

	Strip # and ~ off the end.

 *****************************************************************************/

void
JFSBindingList::CleanFileName
	(
	JString* s
	)
{
	while (s->EndsWith("~") || s->EndsWith("#"))
		{
		s->RemoveSubstring(s->GetLength(), s->GetLength());
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

const JCharacter*
JFSBindingList::Revert()
{
	const JCharacter* userMsg = "";

	// toss everything

	itsBindingList->CleanOut();
	itsOverriddenList->CleanOut();

	jdelete itsUserDefault;
	itsUserDefault = NULL;

	jdelete itsSystemDefault;
	itsSystemDefault = NULL;

	// read system bindings

	ifstream sysInput(kGlobalBindingsFile);
	if (sysInput.good())
		{
		Load(sysInput, kJTrue);
		}
	sysInput.close();

	// read user bindings

	JPrefsFile* file = NULL;
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
				ifstream userInput(origUserFile);
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
		const JCharacter* data = JGetString("DefaultBindingList-OSX::JFSBindingList");
#else
		const JCharacter* data = JGetString("DefaultBindingList::JFSBindingList");
#endif
		const std::string s(data, strlen(data));
		std::istringstream input(s);
		Load(input, kJFalse);
		}

	if (!itsSignalFileName.IsEmpty())
		{
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}

	// ensure that GetDefaultCommand() will work

	if (itsUserDefault == NULL && itsSystemDefault == NULL)
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
	istream&		input,
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
		assert( b != NULL );

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
	JPrefsFile* file = NULL;
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

		if (itsUserDefault != NULL)
			{
			data << *itsUserDefault << '\n';
			}

		const JSize count = GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JFSBinding* b = GetBinding(i);
			if (!b->IsSystemBinding())
				{
				data << *b << '\n';
				}
			}

		file->SetData(kCurrentBindingVersion, data);
		jdelete file;

		ofstream touch(itsSignalFileName);
		touch.close();
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}

	return err;
}
