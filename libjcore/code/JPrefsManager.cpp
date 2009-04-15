/******************************************************************************
 JPrefsManager.cpp

	Buffers the data in a JPrefsFile and provides a base class for
	application-specific preferences management.  The functions to access
	the data are protected because they should be hidden behind a clean
	interface in the derived class.

	UpgradeData() is called after the file has been read.  Since UpgradeData()
	must work on an empty file, this insures that the program has a valid set
	of preferences even if the file could not be read.

	Some programs enforce that only a single copy is running for each user,
	so an open prefs file means that the program crashed while editing the
	preferences.  If this is the case, pass kJTrue for eraseFileIfOpen to
	the constructor.

	Derived classes must implement the following function:

		UpgradeData
			Upgrade the preferences data from the specified version.

	BASE CLASS = JContainer

	Copyright © 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPrefsManager.h>
#include <JPrefsFile.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

// JBroadcaster messages

const JCharacter* JPrefsManager::kDataChanged = "DataChanged::JPrefsManager";
const JCharacter* JPrefsManager::kDataRemoved = "DataRemoved::JPrefsManager";

// JError data

const JCharacter* JPrefsManager::kWrongVersion = "WrongVersion::JPrefsManager";

/******************************************************************************
 Constructor (protected)

	Derived class must call UpgradeData().

 ******************************************************************************/

JPrefsManager::JPrefsManager
	(
	const JCharacter*	fileName,
	const JFileVersion	currentVersion,
	const JBoolean		eraseFileIfOpen
	)
	:
	JContainer(),
	itsCurrentFileVersion(currentVersion),
	itsEraseFileIfOpenFlag(eraseFileIfOpen)
{
	itsFileName = new JString(fileName);
	assert( itsFileName != NULL );

	itsData = new JArray<PrefItem>;
	assert( itsData != NULL );
	itsData->SetCompareFunction(ComparePrefIDs);

	InstallOrderedSet(itsData);
}

/******************************************************************************
 Destructor

	Derived classes must call SaveToDisk().

 ******************************************************************************/

JPrefsManager::~JPrefsManager()
{
	delete itsFileName;

	const JSize count = itsData->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		PrefItem item = itsData->GetElement(i);
		delete item.data;
		}
	delete itsData;
}

/******************************************************************************
 GetData (protected)

 ******************************************************************************/

JBoolean
JPrefsManager::GetData
	(
	const JPrefID&	id,
	std::string*	data
	)
	const
{
	PrefItem item(id.GetID(), NULL);
	JIndex index;
	if (itsData->SearchSorted(item, JOrderedSetT::kAnyMatch, &index))
		{
		item = itsData->GetElement(index);
		data->assign(*(item.data), (item.data)->GetLength());
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetData (protected)

	This creates the item if it doesn't already exist.

 ******************************************************************************/

void
JPrefsManager::SetData
	(
	const JPrefID&		id,
	const JCharacter*	data
	)
{
	PrefItem item(id.GetID(), NULL);
	JBoolean found;
	const JIndex index =
		itsData->SearchSorted1(item, JOrderedSetT::kAnyMatch, &found);
	if (found)
		{
		item         = itsData->GetElement(index);
		*(item.data) = data;
		}
	else
		{
		item.data = new JString(data);
		assert( item.data != NULL );
		itsData->InsertElementAtIndex(index, item);
		}

	Broadcast(DataChanged(id));
}

/******************************************************************************
 RemoveData (protected)

 ******************************************************************************/

void
JPrefsManager::RemoveData
	(
	const JPrefID& id
	)
{
	PrefItem item(id.GetID(), NULL);

	JIndex index;
	if (itsData->SearchSorted(item, JOrderedSetT::kAnyMatch, &index))
		{
		item = itsData->GetElement(index);
		delete item.data;

		itsData->RemoveElement(index);

		Broadcast(DataRemoved(id));
		}
}

/******************************************************************************
 SaveToDisk

 ******************************************************************************/

JError
JPrefsManager::SaveToDisk()
	const
{
	// check that current file is readable

	JPrefsFile* file = NULL;
	JError err = Open(&file, kJTrue);
	if (err.OK())
		{
		delete file;
		file = NULL;
		}
	else
		{
		return err;
		}

	// toss everything

	err = DeletePrefsFile(*itsFileName);
	if (!err.OK())
		{
		return err;
		}

	// write current data

	err = Open(&file, kJTrue);		// version is zero since file was deleted
	if (!err.OK())
		{
		return err;
		}

	file->SetVersion(itsCurrentFileVersion);

	const JSize count = itsData->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const PrefItem item = itsData->GetElement(i);
		file->SetData(item.id, *(item.data));
		}

	delete file;
	return JNoError();
}

/******************************************************************************
 UpgradeData (protected)

	Returns kJTrue if the file had to be created.

 ******************************************************************************/

JBoolean
JPrefsManager::UpgradeData
	(
	const JBoolean reportError
	)
{
	JBoolean isNew = kJFalse;

	JPrefsFile* file = NULL;
	const JError err = Open(&file, kJTrue);
	if (err.OK())
		{
		isNew              = file->IsEmpty();
		itsPrevFileVersion = file->GetVersion();
		LoadData(file);
		UpgradeData(isNew, itsPrevFileVersion);
		delete file;
		}
	else
		{
		itsPrevFileVersion = 0;
		UpgradeData(kJTrue, itsPrevFileVersion);

		if (reportError && err == kWrongVersion)
			{
			(JGetUserNotification())->ReportError(
				"The preferences file is unreadable because it has been modified "
				"by a newer version of this program.");
			}
		else if (reportError)
			{
			JString msg = "The preferences cannot be used because:\n\n";
			msg += err.GetMessage();
			(JGetUserNotification())->ReportError(msg);
			}
		}

	return isNew;
}

/******************************************************************************
 LoadData (private)

 ******************************************************************************/

void
JPrefsManager::LoadData
	(
	JPrefsFile* file
	)
{
	const JSize count = file->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JFAID id;
		const JBoolean ok = file->IndexToID(i, &id);
		assert( ok );

		std::string data;
		file->GetElement(JFAIndex(i), &data);
		JString* s = new JString(data);
		assert( s != NULL );

		PrefItem item(id.GetID(), s);
		JBoolean isDuplicate;
		const JIndex j = itsData->GetInsertionSortIndex(item, &isDuplicate);
		assert( !isDuplicate );
		itsData->InsertElementAtIndex(j, item);
		}
}

/******************************************************************************
 Open (private)

 ******************************************************************************/

JError
JPrefsManager::Open
	(
	JPrefsFile**	file,
	const JBoolean	allowPrevVers
	)
	const
{
	*file = NULL;
	const JError err = JPrefsFile::Create(*itsFileName, file);
	if (err.OK())
		{
		const JFileVersion vers = (**file).GetVersion();
		if (vers == itsCurrentFileVersion ||
			(allowPrevVers && vers < itsCurrentFileVersion))
			{
			return JNoError();
			}
		else
			{
			delete *file;
			*file = NULL;
			return WrongVersion();
			}
		}

	else if (err == JPrefsFile::kFileAlreadyOpen && itsEraseFileIfOpenFlag &&
			 DeletePrefsFile(*itsFileName) == kJNoError)
		{
		return Open(file, allowPrevVers);		// now it will work
		}

	delete *file;
	*file = NULL;
	return err;
}

/******************************************************************************
 DeletePrefsFile (virtual protected)

	We can't call it "DeleteFile" because Windows #defines this.

 ******************************************************************************/

JError
JPrefsManager::DeletePrefsFile
	(
	const JCharacter* fileName
	)
	const
{
	JString fullName;
	JError err = JPrefsFile::GetFullName(fileName, &fullName);
	if (err.OK())
		{
		err = JRemoveFile(fullName);
		}
	return err;
}

/******************************************************************************
 ComparePrefIDs (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JPrefsManager::ComparePrefIDs
	(
	const PrefItem& p1,
	const PrefItem& p2
	)
{
	if (p1.id < p2.id)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (p1.id == p2.id)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

#define JTemplateType JPrefsManager::PrefItem
#include <JArray.tmpls>
#undef JTemplateType
