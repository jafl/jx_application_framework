/******************************************************************************
 JDirInfo.cpp

	Class that contains information about everything in a given directory.

	Derived classes can override IsVisible() to perform extra filtering
	of the list of files.  Note that since this class has a constructor
	function, derived classes must also enforce their own constructor
	functions.

	If the contents will merely change due to filtering or updating,
	instead of changing directories, we broadcast ContentsWillBeUpdated
	before changing anything.  This gives others a chance to save state
	that can be restored after ContentsChanged.  Note that PathChanged
	cancels the effect of ContentsWillBeUpdated.

	The content filter is applied in BuildInfo() because it is so
	expensive and is not likely to change very often since it should not
	be under user control.

	BASE CLASS = JContainer

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JDirInfo.h>
#include <JRegex.h>
#include <JLatentPG.h>
#include <JStdError.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <jDirUtil.h>
#include <jTime.h>
#include <jGlobals.h>
#include <ace/OS_NS_sys_stat.h>
#include <jAssert.h>

// Broadcaster messages types

const JCharacter* JDirInfo::kContentsWillBeUpdated = "ContentsWillBeUpdated::JDirInfo";
const JCharacter* JDirInfo::kContentsChanged       = "ContentsChanged::JDirInfo";
const JCharacter* JDirInfo::kPathChanged           = "PathChanged::JDirInfo";
const JCharacter* JDirInfo::kPermissionsChanged    = "PermissionsChanged::JDirInfo";
const JCharacter* JDirInfo::kSettingsChanged       = "SettingsChanged::JDirInfo";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about BuildInfo() succeeding within the class itself.

	Note that this prevents one from creating derived classes unless one
	creates a similar constructor function that checks OKToCreate().

 ******************************************************************************/

JBoolean
JDirInfo::Create
	(
	const JCharacter*	dirName,
	JDirInfo**			obj
	)
{
	if (OKToCreate(dirName))
		{
		*obj = new JDirInfo(dirName);
		assert( *obj != NULL );
		return kJTrue;
		}
	else
		{
		*obj = NULL;
		return kJFalse;
		}
}

JBoolean
JDirInfo::Create
	(
	const JDirInfo&		source,
	const JCharacter*	dirName,
	JDirInfo**			obj
	)
{
	if (OKToCreate(dirName))
		{
		*obj = new JDirInfo(source, dirName);
		assert( *obj != NULL );
		return kJTrue;
		}
	else
		{
		*obj = NULL;
		return kJFalse;
		}
}

JBoolean
JDirInfo::OKToCreate
	(
	const JCharacter* dirName
	)
{
	return JConvertToBoolean( JDirectoryExists(dirName) &&
							  JDirectoryReadable(dirName) &&
							  JCanEnterDirectory(dirName) );
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JDirInfo::JDirInfo
	(
	const JCharacter* dirName
	)
	:
	JContainer()
{
	AllocateCWD(dirName);

	itsIsValidFlag         = kJTrue;	// see assert() at end of this function
	itsSwitchIfInvalidFlag = kJFalse;
	itsIsWritableFlag      = kJFalse;
	itsModTime             = 0;
	itsStatusTime          = 0;

	itsShowFilesFlag  = kJTrue;			// remember to update ResetCSFFilters()
	itsShowDirsFlag   = kJTrue;
	itsShowHiddenFlag = kJFalse;
	itsShowOthersFlag = kJFalse;

	itsNameRegex           = NULL;
	itsOwnsNameRegexFlag   = kJFalse;
	itsInvertNameRegexFlag = kJFalse;
	itsFilterDirsFlag      = kJFalse;
	itsPermFilter          = NULL;
	itsContentRegex        = NULL;
	itsPG                  = NULL;

	itsDirEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
	assert( itsDirEntries != NULL);
	itsDirEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsDirEntries->SetSortOrder(JOrderedSetT::kSortAscending);

	itsVisEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsVisEntries != NULL);

	itsAlphaEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != NULL);
	itsAlphaEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsAlphaEntries->SetSortOrder(JOrderedSetT::kSortAscending);

	InstallOrderedSet(itsVisEntries);

	const JError err = BuildInfo();
	assert_ok( err );
}

// private

void
JDirInfo::AllocateCWD
	(
	const JCharacter* dirName
	)
{
	itsCWD = new JString;
	assert( itsCWD != NULL );
	const JBoolean ok = JConvertToAbsolutePath(dirName, NULL, itsCWD);
	assert( ok );
	JAppendDirSeparator(itsCWD);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDirInfo::JDirInfo
	(
	const JDirInfo& source
	)
	:
	JContainer(source)
{
	itsCWD = new JString(*(source.itsCWD));
	assert( itsCWD != NULL );

	itsIsValidFlag         = source.itsIsValidFlag;
	itsSwitchIfInvalidFlag = source.itsSwitchIfInvalidFlag;
	itsIsWritableFlag      = source.itsIsWritableFlag;
	itsModTime             = source.itsModTime;
	itsStatusTime          = source.itsStatusTime;

	JDirInfoX(source);
	PrivateCopySettings(source);
	CopyDirEntries(source);
	InstallOrderedSet(itsVisEntries);
}

JDirInfo::JDirInfo
	(
	const JDirInfo&		source,
	const JCharacter*	dirName
	)
	:
	JContainer(source)
{
	AllocateCWD(dirName);

	itsIsValidFlag         = kJTrue;			// see assert() at end of this function
	itsSwitchIfInvalidFlag = kJFalse;
	itsIsWritableFlag      = kJFalse;
	itsModTime             = 0;
	itsStatusTime          = 0;

	JDirInfoX(source);
	PrivateCopySettings(source);
	InstallOrderedSet(itsVisEntries);

	const JError err = BuildInfo();
	assert_ok( err );
}

// private

void
JDirInfo::JDirInfoX
	(
	const JDirInfo& source
	)
{
	itsNameRegex           = NULL;
	itsOwnsNameRegexFlag   = kJFalse;
	itsInvertNameRegexFlag = kJFalse;
	itsFilterDirsFlag      = kJFalse;
	itsPermFilter          = NULL;
	itsContentRegex        = NULL;
	itsPG                  = NULL;

	itsDirEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
	assert( itsDirEntries != NULL);

	itsVisEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsVisEntries != NULL);

	itsAlphaEntries = new JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != NULL);
	itsAlphaEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsAlphaEntries->SetSortOrder(JOrderedSetT::kSortAscending);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDirInfo::~JDirInfo()
{
	delete itsDirEntries;
	delete itsVisEntries;
	delete itsAlphaEntries;

	delete itsCWD;

	if (itsOwnsNameRegexFlag)
		{
		delete itsNameRegex;
		}

	delete itsContentRegex;
	delete itsPG;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JDirInfo&
JDirInfo::operator=
	(
	const JDirInfo& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	JContainer::operator=(source);

	*itsCWD = *(source.itsCWD);

	itsIsValidFlag         = source.itsIsValidFlag;
	itsSwitchIfInvalidFlag = source.itsSwitchIfInvalidFlag;
	itsIsWritableFlag      = source.itsIsWritableFlag;
	itsModTime             = source.itsModTime;
	itsStatusTime          = source.itsStatusTime;

	PrivateCopySettings(source);
	CopyDirEntries(source);
	Broadcast(SettingsChanged());

	return *this;
}

/*****************************************************************************
 CopySettings

 ******************************************************************************/

void
JDirInfo::CopySettings
	(
	const JDirInfo& source
	)
{
	const JBoolean rebuild = JNegate(
		itsContentRegex != NULL && source.itsContentRegex != NULL &&
		itsContentRegex->GetPattern() == (source.itsContentRegex)->GetPattern());

	PrivateCopySettings(source);

	if (rebuild)
		{
		ForceUpdate();
		}
	else
		{
		itsDirEntries->Sort();
		ApplyFilters(kJTrue);
		}

	Broadcast(SettingsChanged());
}

/*****************************************************************************
 PrivateCopySettings (private)

 ******************************************************************************/

void
JDirInfo::PrivateCopySettings
	(
	const JDirInfo& source
	)
{
	itsShowFilesFlag  = source.itsShowFilesFlag;
	itsShowDirsFlag   = source.itsShowDirsFlag;
	itsShowHiddenFlag = source.itsShowHiddenFlag;
	itsShowOthersFlag = source.itsShowOthersFlag;

	itsPermFilter = source.itsPermFilter;

	// copy name filter

	if (itsOwnsNameRegexFlag)
		{
		delete itsNameRegex;
		}
	itsNameRegex = NULL;

	if (source.itsNameRegex != NULL && !source.itsOwnsNameRegexFlag)
		{
		itsNameRegex         = source.itsNameRegex;
		itsOwnsNameRegexFlag = kJFalse;
		}
	else if (source.itsNameRegex != NULL)
		{
		itsNameRegex = new JRegex(*(source.itsNameRegex));
		assert( itsNameRegex != NULL );
		itsOwnsNameRegexFlag = kJTrue;
		}

	itsInvertNameRegexFlag = source.itsInvertNameRegexFlag;
	itsFilterDirsFlag      = source.itsFilterDirsFlag;

	// copy content filter

	delete itsContentRegex;
	itsContentRegex = NULL;

	if (source.itsContentRegex != NULL)
		{
		itsContentRegex = new JRegex(*(source.itsContentRegex));
		assert( itsContentRegex != NULL );
		}

	// copy sort method

	const JElementComparison<JDirEntry*>* compareObj = NULL;
	const JBoolean hasCompare = (source.itsDirEntries)->GetCompareObject(&compareObj);
	assert( hasCompare );

	itsDirEntries->SetCompareObject(*compareObj);
	itsDirEntries->SetSortOrder((source.itsDirEntries)->GetSortOrder());
}

/*****************************************************************************
 CopyDirEntries (private)

 ******************************************************************************/

void
JDirInfo::CopyDirEntries
	(
	const JDirInfo& source
	)
{
	assert( itsDirEntries != NULL );
	itsDirEntries->CleanOut();

	const JSize entryCount = (source.itsDirEntries)->GetElementCount();
	for (JIndex i=1; i<=entryCount; i++)
		{
		JDirEntry* origEntry = (source.itsDirEntries)->NthElement(i);
		JDirEntry* entry     = new JDirEntry(*origEntry);
		assert( entry != NULL );
		itsDirEntries->Append(entry);
		}

	// update other file lists

	assert( itsVisEntries != NULL && itsAlphaEntries != NULL );

	ApplyFilters(kJFalse);
}

/******************************************************************************
 ShowFiles

 ******************************************************************************/

void
JDirInfo::ShowFiles
	(
	const JBoolean showFiles
	)
{
	if (showFiles != itsShowFilesFlag)
		{
		itsShowFilesFlag = showFiles;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ShowDirs

 ******************************************************************************/

void
JDirInfo::ShowDirs
	(
	const JBoolean showDirs
	)
{
	if (showDirs != itsShowDirsFlag)
		{
		itsShowDirsFlag = showDirs;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ShowHidden

 ******************************************************************************/

void
JDirInfo::ShowHidden
	(
	const JBoolean showHidden
	)
{
	if (showHidden != itsShowHiddenFlag)
		{
		itsShowHiddenFlag = showHidden;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ShowOthers

 ******************************************************************************/

void
JDirInfo::ShowOthers
	(
	const JBoolean showOthers
	)
{
	if (showOthers != itsShowOthersFlag)
		{
		itsShowOthersFlag = showOthers;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ShouldApplyWildcardFilterToDirs

 ******************************************************************************/

void
JDirInfo::ShouldApplyWildcardFilterToDirs
	(
	const JBoolean apply
	)
{
	if (apply != itsFilterDirsFlag)
		{
		itsFilterDirsFlag = apply;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ChangeSort

 ******************************************************************************/

void
JDirInfo::ChangeSort
	(
	JCompareDirEntries*				f,
	const JOrderedSetT::SortOrder	order
	)
{
	Broadcast(ContentsWillBeUpdated());

	itsDirEntries->SetCompareFunction(f);
	itsDirEntries->SetSortOrder(order);
	itsDirEntries->Sort();

	itsVisEntries->SetCompareFunction(f);
	itsVisEntries->SetSortOrder(order);
	itsVisEntries->Sort();

	Broadcast(ContentsChanged());
	Broadcast(SettingsChanged());
}

/******************************************************************************
 ChangeProgressDisplay

	We take ownership of the object and will delete it when appropriate.

 ******************************************************************************/

void
JDirInfo::ChangeProgressDisplay
	(
	JProgressDisplay* pg
	)
{
	assert( pg != NULL );

	delete itsPG;
	itsPG = pg;
}

/******************************************************************************
 UseDefaultProgressDisplay

 ******************************************************************************/

void
JDirInfo::UseDefaultProgressDisplay()
{
	delete itsPG;
	itsPG = NULL;
}

/******************************************************************************
 FindEntry

	Returns kJTrue if an entry with the given name exists.

 ******************************************************************************/

JBoolean
JDirInfo::FindEntry
	(
	const JCharacter*	name,
	JIndex*				index
	)
	const
{
	JDirEntry target(name, 0);
	return itsAlphaEntries->SearchSorted(&target, JOrderedSetT::kFirstMatch, index);
}

/******************************************************************************
 ClosestMatch

	Returns the index of the closest match for the given name prefix.

 ******************************************************************************/

JBoolean
JDirInfo::ClosestMatch
	(
	const JCharacter*	prefixStr,
	JIndex*				index
	)
	const
{
	JDirEntry target(prefixStr, 0);
	JBoolean found;
	*index = itsAlphaEntries->SearchSorted1(&target, JOrderedSetT::kFirstMatch, &found);
	if (*index > itsAlphaEntries->GetElementCount())		// insert beyond end of list
		{
		*index = itsAlphaEntries->GetElementCount();
		}
	return JConvertToBoolean( *index > 0 );
}

/*****************************************************************************
 GoUp

 ******************************************************************************/

JError
JDirInfo::GoUp()
{
	JString theCWD = *itsCWD;

	// strip trailing slashes

	JStripTrailingDirSeparator(&theCWD);
	if (JIsRootDirectory(theCWD))
		{
		return JNoError();
		}

	// change directory

	JString newCWD, name;
	if (JSplitPathAndName(theCWD, &newCWD, &name))
		{
		return GoTo(newCWD);
		}
	else
		{
		return JBadPath(theCWD);
		}
}

/*****************************************************************************
 GoDown

 ******************************************************************************/

JError
JDirInfo::GoDown
	(
	const JCharacter* dirName
	)
{
	const JString theCWD = *itsCWD + dirName;
	return GoTo(theCWD);
}

/*****************************************************************************
 GoToClosest

	If the directory exists, go to it.  Otherwise, go as far down the
	directory tree as possible towards the specified directory.

	As an example, /usr/include/junk doesn't normally exist, so it will
	go to /usr/include instead.

 ******************************************************************************/

void
JDirInfo::GoToClosest
	(
	const JCharacter* origDirName
	)
{
	const JString dirName = JGetClosestDirectory(origDirName);
	const JError err      = GoTo(dirName);
	assert_ok( err );
}

/*****************************************************************************
 GoTo

 ******************************************************************************/

JError
JDirInfo::GoTo
	(
	const JCharacter* origDirName
	)
{
	JString dirName;
	if (JStringEmpty(origDirName) ||
		!JConvertToAbsolutePath(origDirName, NULL, &dirName))
		{
		return JBadPath(origDirName);
		}

	if (JSameDirEntry(dirName, *itsCWD))
		{
		Update();
		return JNoError();
		}
	JAppendDirSeparator(&dirName);

	const JString origCWD = *itsCWD;

	*itsCWD = dirName;
	const JError err = BuildInfo();
	if (err.OK())
		{
		Broadcast(PathChanged());
		}
	else
		{
		*itsCWD = origCWD;
		}

	return err;
}

/*****************************************************************************
 BuildInfo (private)

	If you add error conditions, remember to update OKToCreate().

 ******************************************************************************/

JError
JDirInfo::BuildInfo()
{
	if (!JDirectoryReadable(*itsCWD))
		{
		return JAccessDenied(*itsCWD);
		}

	const JString origDir = JGetCurrentDirectory();

	JError err = JChangeDirectory(*itsCWD);
	if (!err.OK())
		{
		return err;
		}

	// clear old information

	itsDirEntries->CleanOut();

	// update instance variables

	JStripTrailingDirSeparator(itsCWD);		// keep Windows happy

	ACE_stat stbuf;
	ACE_OS::stat(*itsCWD, &stbuf);
	itsIsValidFlag    = kJTrue;
	itsIsWritableFlag = JDirectoryWritable(*itsCWD);
	itsModTime        = stbuf.st_mtime;
	itsStatusTime     = stbuf.st_ctime;

	JAppendDirSeparator(itsCWD);

	// process files in the directory

	JLatentPG pg(itsContentRegex == NULL ? 100 : 10);
	if (itsPG != NULL)
		{
		pg.SetPG(itsPG, kJFalse);
		}
	pg.VariableLengthProcessBeginning("Scanning directory...", kJTrue, kJFalse);

	BuildInfo1(pg);

	pg.ProcessFinished();

	err = JChangeDirectory(origDir);
	assert_ok( err );

	ApplyFilters(kJFalse);
	return JNoError();
}

/*****************************************************************************
 Update

	Returns kJTrue if anything needed to be updated.

 ******************************************************************************/

JBoolean
JDirInfo::Update
	(
	const JBoolean force
	)
{
	ACE_stat info;
	if (force ||
		ACE_OS::lstat(*itsCWD, &info) != 0 ||
		ACE_OS::stat(*itsCWD, &info) != 0  ||
		itsModTime != (time_t) info.st_mtime)
		{
		ForceUpdate();
		return kJTrue;
		}
	else if (itsStatusTime != (time_t) info.st_ctime &&
			 JDirectoryReadable(*itsCWD))
		{
		itsStatusTime     = info.st_ctime;
		itsIsWritableFlag = JDirectoryWritable(*itsCWD);
		Broadcast(PermissionsChanged());
		return kJTrue;
		}
	else if (itsStatusTime != (time_t) info.st_ctime)
		{
		ForceUpdate();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/*****************************************************************************
 ForceUpdate

	Returns kJTrue if the update was successful.  Otherwise, returns
	kJFalse to indicate that path is no longer valid.

 ******************************************************************************/

JBoolean
JDirInfo::ForceUpdate()
{
	if (JDirectoryExists(*itsCWD))
		{
		Broadcast(ContentsWillBeUpdated());

		const JError err = BuildInfo();
		if (err.OK())
			{
			return kJTrue;
			}
		}

	if (itsSwitchIfInvalidFlag)
		{
		JString path;
		if (!JGetHomeDirectory(&path) || !OKToCreate(path))
			{
			path = JGetRootDirectory();
			}
		GoTo(path);
		}
	else
		{
		itsIsValidFlag    = kJFalse;
		itsIsWritableFlag = kJFalse;
		itsDirEntries->CleanOut();
		itsVisEntries->CleanOut();
		itsAlphaEntries->CleanOut();
		}

	return kJFalse;
}

/******************************************************************************
 ApplyFilters (private)

	update should be kJTrue if the contents are merely being updated.

 ******************************************************************************/

void
JDirInfo::ApplyFilters
	(
	const JBoolean update
	)
{
	if (update)
		{
		Broadcast(ContentsWillBeUpdated());
		}

	itsVisEntries->CleanOut();
	itsAlphaEntries->CleanOut();

	const JSize count = itsDirEntries->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JDirEntry* entry = itsDirEntries->NthElement(i);
		if (IsVisible(*entry))
			{
			itsVisEntries->Append(entry);
			itsAlphaEntries->InsertSorted(entry, kJTrue);
			}
		}

	Broadcast(ContentsChanged());
}

/******************************************************************************
 IsVisible (virtual protected)

 ******************************************************************************/

JBoolean
JDirInfo::IsVisible
	(
	const JDirEntry& entry
	)
	const
{
	const JDirEntry::Type type = entry.GetType();
	const JString& name        = entry.GetName();

	if (name.GetFirstCharacter() == '.' && name != ".." && !itsShowHiddenFlag)
		{
		return kJFalse;
		}

	if (type == JDirEntry::kDir || type == JDirEntry::kDirLink)
		{
		return JI2B(itsShowDirsFlag &&
					(!itsFilterDirsFlag || MatchesNameFilter(entry)) &&
					MatchesDirEntryFilter(entry));
		}
	else if (type == JDirEntry::kFile || type == JDirEntry::kFileLink ||
			 type == JDirEntry::kBrokenLink)
		{
		return JI2B(itsShowFilesFlag &&
					MatchesNameFilter(entry) &&
					MatchesDirEntryFilter(entry));
		}
	else if (type == JDirEntry::kUnknown || type == JDirEntry::kUnknownLink)
		{
		return JI2B(itsShowOthersFlag &&
					MatchesNameFilter(entry) &&
					MatchesDirEntryFilter(entry));
		}
	else if (type == JDirEntry::kDoesNotExist)
		{
		return kJFalse;
		}
	else
		{
		assert( 0 );	// this should never happen
		return kJFalse;
		}
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
JDirInfo::SetWildcardFilter
	(
	const JCharacter*	filterStr,
	const JBoolean		negate,
	const JBoolean		caseSensitive
	)
{
	JString regexStr;
	if (!BuildRegexFromWildcardFilter(filterStr, &regexStr))
		{
		ClearWildcardFilter();
		return;
		}

	JRegex* r = new JRegex(regexStr);
	assert( r != NULL );
	r->SetCaseSensitive(caseSensitive);

	SetWildcardFilter(r, kJTrue, negate);
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
JDirInfo::SetWildcardFilter
	(
	JRegex*			filter,
	const JBoolean	dirInfoOwnsRegex,
	const JBoolean	negate
	)
{
	if (filter == NULL)
		{
		ClearWildcardFilter();
		return;
		}

	if (itsOwnsNameRegexFlag)
		{
		delete itsNameRegex;
		}

	itsNameRegex = filter;
	assert( itsNameRegex != NULL );

	itsOwnsNameRegexFlag   = dirInfoOwnsRegex;
	itsInvertNameRegexFlag = negate;

	ApplyFilters(kJTrue);
	Broadcast(SettingsChanged());
}

/******************************************************************************
 ClearWildcardFilter

 ******************************************************************************/

void
JDirInfo::ClearWildcardFilter()
{
	if (itsNameRegex != NULL)
		{
		if (itsOwnsNameRegexFlag)
			{
			delete itsNameRegex;
			}
		itsNameRegex         = NULL;
		itsOwnsNameRegexFlag = kJFalse;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 BuildRegexFromWildcardFilter (static)

	Converts a wildcard filter ("*.cc *.h") to a regex ("^.*\.cc$|^.*\.h$").
	Returns kJFalse if the given wildcard filter string is empty.

 ******************************************************************************/

JBoolean
JDirInfo::BuildRegexFromWildcardFilter
	(
	const JCharacter*	origFilterStr,
	JString*			regexStr
	)
{
	regexStr->Clear();

	JString filterStr = origFilterStr;
	filterStr.TrimWhitespace();

	if (filterStr.IsEmpty())
		{
		return kJFalse;
		}

	JIndex index;
	while (filterStr.LocateSubstring(" ", &index))
		{
		assert( index > 1 );
		const JString str = filterStr.GetSubstring(1, index-1);

		AppendRegex(str, regexStr);

		filterStr.RemoveSubstring(1, index);
		filterStr.TrimWhitespace();
		}

	assert( !filterStr.IsEmpty() );
	AppendRegex(filterStr, regexStr);
	return kJTrue;
}

/******************************************************************************
 AppendRegex (static private)

 ******************************************************************************/

void
JDirInfo::AppendRegex
	(
	const JCharacter*	origStr,
	JString*			regexStr
	)
{
JIndex i;

	JString str = origStr;

	// Convert wildcard multiples (*) to regex multiples (.*)
	// and wildcard singles (?) to regex singles (.)

	for (i = str.GetLength(); i>=1; i--)
		{
		const JCharacter c = str.GetCharacter(i);
		if (c == '*')
			{
			str.InsertSubstring(".", i);
			}
		else if (c == '?')
			{
			str.SetCharacter(i, '.');
			}
		else if (JRegex::NeedsBackslashToBeLiteral(c))
			{
			str.InsertSubstring("\\", i);
			}
		}

	// Add instructions that it must match the entire file name.

	str.PrependCharacter('^');
	str.AppendCharacter('$');

	// append to regexStr

	if (!regexStr->IsEmpty())
		{
		regexStr->AppendCharacter('|');
		}
	*regexStr += str;
}

/******************************************************************************
 MatchesNameFilter (protected)

 ******************************************************************************/

JBoolean
JDirInfo::MatchesNameFilter
	(
	const JDirEntry& entry
	)
	const
{
	if (itsNameRegex != NULL)
		{
		const JString& name = entry.GetName();
		JBoolean match      = itsNameRegex->Match(name);
		if (itsInvertNameRegexFlag)
			{
			match = !match;
			}
		return match;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 SetDirEntryFilter

 ******************************************************************************/

void
JDirInfo::SetDirEntryFilter
	(
	JCheckPermissions* f
	)
{
	if (f != itsPermFilter)
		{
		itsPermFilter = f;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 ClearDirEntryFilter

 ******************************************************************************/

void
JDirInfo::ClearDirEntryFilter()
{
	if (itsPermFilter != NULL)
		{
		itsPermFilter = NULL;
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 MatchesDirEntryFilter (protected)

 ******************************************************************************/

JBoolean
JDirInfo::MatchesDirEntryFilter
	(
	const JDirEntry& entry
	)
	const
{
	if (itsPermFilter != NULL)
		{
		return itsPermFilter(entry);
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 SetContentFilter

 ******************************************************************************/

JError
JDirInfo::SetContentFilter
	(
	const JCharacter* regexStr
	)
{
	if (itsContentRegex != NULL && regexStr == itsContentRegex->GetPattern())
		{
		return JNoError();
		}

	JBoolean hadFilter = kJTrue;
	JString prevPattern;
	if (itsContentRegex == NULL)
		{
		hadFilter       = kJFalse;
		itsContentRegex = new JRegex;
		assert( itsContentRegex != NULL );
		itsContentRegex->SetSingleLine();
		}
	else
		{
		prevPattern = itsContentRegex->GetPattern();
		}

	JError err = itsContentRegex->SetPattern(regexStr);
	if (err.OK())
		{
		ForceUpdate();
		Broadcast(SettingsChanged());
		}
	else if (hadFilter)
		{
		err = itsContentRegex->SetPattern(prevPattern);
		assert_ok( err );
		}
	else
		{
		delete itsContentRegex;
		itsContentRegex = NULL;
		}
	return err;
}

/******************************************************************************
 ClearContentFilter

 ******************************************************************************/

void
JDirInfo::ClearContentFilter()
{
	if (itsContentRegex != NULL)
		{
		delete itsContentRegex;
		itsContentRegex = NULL;
		ForceUpdate();
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 MatchesContentFilter (protected)

 ******************************************************************************/

JBoolean
JDirInfo::MatchesContentFilter
	(
	const JDirEntry& entry
	)
	const
{
	if (itsContentRegex == NULL || entry.IsDirectory())
		{
		return kJTrue;
		}
	else
		{
		return entry.MatchesContentFilter(*itsContentRegex);
		}
}

/******************************************************************************
 ResetCSFFilters

	This does not clear the name filter because it is primarily useful
	for JChooseSaveFile, which doesn't want it changed.

 ******************************************************************************/

void
JDirInfo::ResetCSFFilters()
{
	JBoolean apply = kJFalse, rebuild = kJFalse;

	if (!itsShowFilesFlag)
		{
		itsShowFilesFlag = kJTrue;
		apply            = kJTrue;
		}
	if (!itsShowDirsFlag)
		{
		itsShowDirsFlag = kJTrue;
		apply           = kJTrue;
		}
//	if (itsShowHiddenFlag)
//		{
//		itsShowHiddenFlag = kJFalse;
//		apply             = kJTrue;
//		}
	if (itsShowOthersFlag)
		{
		itsShowOthersFlag = kJFalse;
		apply             = kJTrue;
		}

	if (itsPermFilter != NULL)
		{
		itsPermFilter = NULL;
		apply         = kJTrue;
		}
	if (itsContentRegex != NULL)
		{
		delete itsContentRegex;
		itsContentRegex = NULL;
		rebuild         = kJTrue;
		}

	if (rebuild)
		{
		ForceUpdate();
		Broadcast(SettingsChanged());
		}
	else if (apply)
		{
		ApplyFilters(kJTrue);
		Broadcast(SettingsChanged());
		}
}

/******************************************************************************
 Empty (static)

	Returns kJTrue if the directory doesn't exist or exists and is empty.

 ******************************************************************************/

JBoolean
JDirInfo::Empty
	(
	const JCharacter* dirName
	)
{
	JDirInfo* info;
	if (!Create(dirName, &info))
		{
		return kJTrue;
		}
	const JBoolean empty = info->IsEmpty();
	delete info;
	return empty;
}

#define JTemplateType JDirEntry
#include <JPtrArray.tmpls>
#undef JTemplateType
