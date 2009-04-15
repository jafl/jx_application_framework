/******************************************************************************
 JPrefsFile.cpp

	Class to manage a preferences file.  The location of the file is system
	dependent.  Thus, creating a JPrefsFile object only requires specifying
	a stem for the file name.

		OS		Location
		--		--------
		UNIX	~/.*.pref

	To guarantee that your preference file name is unique, you should
	register the stem with jx-registry@newplanetsoftware.com
	(This is a free service.  We simply check whether anybody else has
	 already registered the signature.)

	Each item in the JPrefsFile has an id.  This id is specified when the
	item is created, and -must- be unique.  The id is the only guaranteed way
	to get at the data after it is created.

	For convenience, SetData() automatically creates a new item if the
	given id doesn't already exist.

	We ignore the issue of the file signature because preferences files
	are usually hidden from the user and should therefore be named to avoid
	conflicts between programs.  The names serve as a sufficient signature.

	There is still the problem of more than one program trying to open
	the same file, however.  Derived classes are therefore still required
	to write constructor functions.

	BASE CLASS = JFileArray

	Copyright © 1995-2001 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPrefsFile.h>
#include <JFileArrayIndex.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jAssert.h>

// JError types

const JCharacter* JPrefsFile::kNoHomeDirectory = "NoHomeDirectory::JPrefsFile";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about errors within the class itself.

 ******************************************************************************/

JError
JPrefsFile::Create
	(
	const JCharacter*	fileNameStem,
	JPrefsFile**		obj,
	const CreateAction	action
	)
{
	JString fullName;
	const JError err = OKToCreate(fileNameStem, &fullName, action);
	if (err.OK())
		{
		*obj = new JPrefsFile(fullName, action);
		assert( *obj != NULL );
		}
	else
		{
		*obj = NULL;
		}
	return err;
}

JError
JPrefsFile::OKToCreate
	(
	const JCharacter*	fileNameStem,
	JString*			fullName,
	const CreateAction	action
	)
{
	JError err = GetFullName(fileNameStem, fullName);
	if (err.OK())
		{
		err = OKToCreateBase(*fullName, "", action);
		}
	return err;
}

/******************************************************************************
 GetFullName (static)

 ******************************************************************************/

JError
JPrefsFile::GetFullName
	(
	const JCharacter*	fileNameStem,
	JString*			fullName
	)
{
	if (JGetPrefsDirectory(fullName))
		{
		*fullName += ".";
		*fullName += fileNameStem;
		*fullName += ".pref";
		return JNoError();
		}
	else
		{
		fullName->Clear();
		return NoHomeDirectory();
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JPrefsFile::JPrefsFile
	(
	const JCharacter*	fileName,
	const CreateAction	action
	)
	:
	JFileArray(fileName, "", action)
{
#ifdef _J_UNIX
	JSetPermissions(fileName, S_IRUSR | S_IWUSR);
#endif
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPrefsFile::~JPrefsFile()
{
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
JPrefsFile::SetData
	(
	const JPrefID&		id,
	std::ostringstream&	data
	)
{
	if (IDValid(id))
		{
		SetElement(id, data);
		}
	else
		{
		AppendElement(data);
		(GetFileArrayIndex())->SetElementID(GetElementCount(), id);
		}
}

void
JPrefsFile::SetData
	(
	const JPrefID&		id,
	const std::string&	data
	)
{
	if (IDValid(id))
		{
		SetElement(id, data.c_str());
		}
	else
		{
		AppendElement(data.c_str());
		(GetFileArrayIndex())->SetElementID(GetElementCount(), id);
		}
}

void
JPrefsFile::SetData
	(
	const JPrefID&		id,
	const JCharacter*	data
	)
{
	if (IDValid(id))
		{
		SetElement(id, data);
		}
	else
		{
		AppendElement(data);
		(GetFileArrayIndex())->SetElementID(GetElementCount(), id);
		}
}
