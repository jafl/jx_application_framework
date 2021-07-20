/******************************************************************************
 JPrefsManager.cpp

	Buffers the data in a JPrefsFile and provides a base class for
	application-specific preferences management.  The functions to access
	the data are protected because they should be hidden behind a clean
	interface in the derived class.

	UpgradeData() is called after the file has been read.  Since UpgradeData()
	must work on an empty file, this ensures that the program has a valid set
	of preferences even if the file could not be read.

	Some programs enforce that only a single copy is running for each user,
	so an open prefs file means that the program crashed while editing the
	preferences.  If this is the case, pass true for eraseFileIfOpen to
	the constructor.

	Derived classes must implement the following function:

		UpgradeData
			Upgrade the preferences data from the specified version.

	BASE CLASS = JContainer

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include "JPrefsManager.h"
#include "JPrefsFile.h"
#include "jFileUtil.h"
#include "jSysUtil.h"
#include "jGlobals.h"
#include "jAssert.h"

// JBroadcaster messages

const JUtf8Byte* JPrefsManager::kDataChanged = "DataChanged::JPrefsManager";
const JUtf8Byte* JPrefsManager::kDataRemoved = "DataRemoved::JPrefsManager";

// JError data

const JUtf8Byte* JPrefsManager::kWrongVersion = "WrongVersion::JPrefsManager";

/******************************************************************************
 Constructor (protected)

	Derived class must call UpgradeData().

 ******************************************************************************/

JPrefsManager::JPrefsManager
	(
	const JString&		fileName,
	const JFileVersion	currentVersion,
	const bool		eraseFileIfOpen
	)
	:
	JContainer(),
	itsCurrentFileVersion(currentVersion),
	itsEraseFileIfOpenFlag(eraseFileIfOpen)
{
	itsFileName = jnew JString(fileName);
	assert( itsFileName != nullptr );

	itsData = jnew JArray<PrefItem>;
	assert( itsData != nullptr );
	itsData->SetCompareFunction(ComparePrefIDs);

	InstallCollection(itsData);
}

/******************************************************************************
 Destructor

	Derived classes must call SaveToDisk().

 ******************************************************************************/

JPrefsManager::~JPrefsManager()
{
	jdelete itsFileName;

	for (const PrefItem& item : *itsData)
		{
		jdelete item.data;
		}
	jdelete itsData;
}

/******************************************************************************
 GetData (protected)

 ******************************************************************************/

bool
JPrefsManager::GetData
	(
	const JPrefID&	id,
	std::string*	data
	)
	const
{
	PrefItem item(id.GetID(), nullptr);
	JIndex index;
	if (itsData->SearchSorted(item, JListT::kAnyMatch, &index))
		{
		item = itsData->GetElement(index);
		data->assign(item.data->GetBytes(), item.data->GetByteCount());
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 SetData (protected)

	This creates the item if it doesn't already exist.

 ******************************************************************************/

void
JPrefsManager::SetData
	(
	const JPrefID&	id,
	const JString&	data
	)
{
	SetData(id, data.GetBytes());
}

void
JPrefsManager::SetData
	(
	const JPrefID&		id,
	const JUtf8Byte*	data
	)
{
	PrefItem item(id.GetID(), nullptr);
	bool found;
	const JIndex index =
		itsData->SearchSorted1(item, JListT::kAnyMatch, &found);
	if (found)
		{
		item         = itsData->GetElement(index);
		*(item.data) = data;
		}
	else
		{
		item.data = jnew JString(data);
		assert( item.data != nullptr );
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
	PrefItem item(id.GetID(), nullptr);

	JIndex index;
	if (itsData->SearchSorted(item, JListT::kAnyMatch, &index))
		{
		item = itsData->GetElement(index);
		jdelete item.data;

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

	JPrefsFile* file = nullptr;
	JError err = Open(&file, true);
	if (err.OK())
		{
		jdelete file;
		file = nullptr;
		}
	else
		{
		return err;
		}

	// save owner

	bool preserveOwner = JUserIsAdmin();
	JString fullName;
	uid_t ownerID;
	gid_t groupID;
	if (preserveOwner)
		{
		JError err = JPrefsFile::GetFullName(*itsFileName, &fullName);
		if (err.OK())
			{
			err = JGetOwnerID(fullName, &ownerID);
			}
		if (err.OK())
			{
			err = JGetOwnerGroup(fullName, &groupID);
			}
		if (!err.OK())
			{
			return err;
			}

		preserveOwner = !JUserIsAdmin(ownerID);
		}

	// toss everything

	err = DeletePrefsFile(*itsFileName);
	if (!err.OK())
		{
		return err;
		}

	// write current data

	err = Open(&file, true);		// version is zero since file was deleted
	if (!err.OK())
		{
		return err;
		}

	file->SetVersion(itsCurrentFileVersion);

	for (const PrefItem& item : *itsData)
		{
		file->SetData(item.id, *item.data);
		}

	jdelete file;

	// restore owner -- too late to do anything if it fails

	if (preserveOwner)
		{
		JSetOwner(fullName, ownerID, groupID);
		}

	return JNoError();
}

/******************************************************************************
 UpgradeData (protected)

	Returns true if the file had to be created.

 ******************************************************************************/

bool
JPrefsManager::UpgradeData
	(
	const bool reportError
	)
{
	bool isNew = false;

	JPrefsFile* file = nullptr;
	const JError err = Open(&file, true);
	if (err.OK())
		{
		isNew              = file->IsEmpty();
		itsPrevFileVersion = file->GetVersion();
		LoadData(file);
		UpgradeData(isNew, itsPrevFileVersion);
		jdelete file;
		}
	else
		{
		itsPrevFileVersion = 0;
		UpgradeData(true, itsPrevFileVersion);

		if (reportError && err == kWrongVersion)
			{
			JGetUserNotification()->ReportError(JGetString("NewerVersion::JPrefsManager"));
			}
		else if (reportError)
			{
			const JUtf8Byte* map[] =
				{
				"msg", err.GetMessage().GetBytes()
				};
			const JString msg = JGetString("OtherError::JPrefsManager", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
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
		const bool ok = file->IndexToID(i, &id);
		assert( ok );

		std::string data;
		file->GetElement(JFAIndex(i), &data);
		auto* s = jnew JString(data.c_str(), data.length());
		assert( s != nullptr );

		PrefItem item(id.GetID(), s);
		bool isDuplicate;
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
	const bool	allowPrevVers
	)
	const
{
	*file = nullptr;
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
			jdelete *file;
			*file = nullptr;
			return WrongVersion();
			}
		}

	else if (err == JPrefsFile::kFileAlreadyOpen && itsEraseFileIfOpenFlag &&
			 DeletePrefsFile(*itsFileName) == kJNoError)
		{
		return Open(file, allowPrevVers);		// now it will work
		}

	jdelete *file;
	*file = nullptr;
	return err;
}

/******************************************************************************
 DeletePrefsFile (private)

	We can't call it "DeleteFile" because Windows #defines this.

 ******************************************************************************/

JError
JPrefsManager::DeletePrefsFile
	(
	const JString& fileName
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

JListT::CompareResult
JPrefsManager::ComparePrefIDs
	(
	const PrefItem& p1,
	const PrefItem& p2
	)
{
	if (p1.id < p2.id)
		{
		return JListT::kFirstLessSecond;
		}
	else if (p1.id == p2.id)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}
