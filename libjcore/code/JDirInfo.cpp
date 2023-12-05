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

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#include "JDirInfo.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JRegex.h"
#include "JLatentPG.h"
#include "JStdError.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include "jDirUtil.h"
#include "jVCSUtil.h"
#include "jTime.h"
#include "jGlobals.h"
#include <ace/OS_NS_sys_stat.h>
#include "jAssert.h"

// Broadcaster messages types

const JUtf8Byte* JDirInfo::kContentsWillBeUpdated = "ContentsWillBeUpdated::JDirInfo";
const JUtf8Byte* JDirInfo::kContentsChanged       = "ContentsChanged::JDirInfo";
const JUtf8Byte* JDirInfo::kPathChanged           = "PathChanged::JDirInfo";
const JUtf8Byte* JDirInfo::kPermissionsChanged    = "PermissionsChanged::JDirInfo";
const JUtf8Byte* JDirInfo::kSettingsChanged       = "SettingsChanged::JDirInfo";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about BuildInfo() succeeding within the class itself.

	Note that this prevents one from creating derived classes unless one
	creates a similar constructor function that checks OKToCreate().

 ******************************************************************************/

bool
JDirInfo::Create
	(
	const JString&	dirName,
	JDirInfo**		obj
	)
{
	if (OKToCreate(dirName))
	{
		*obj = jnew JDirInfo(dirName);
		assert( *obj != nullptr );
		return true;
	}
	else
	{
		*obj = nullptr;
		return false;
	}
}

bool
JDirInfo::Create
	(
	const JDirInfo&	source,
	const JString&	dirName,
	JDirInfo**		obj
	)
{
	if (OKToCreate(dirName))
	{
		*obj = jnew JDirInfo(source, dirName);
		assert( *obj != nullptr );
		return true;
	}
	else
	{
		*obj = nullptr;
		return false;
	}
}

bool
JDirInfo::OKToCreate
	(
	const JString& dirName
	)
{
	return JDirectoryExists(dirName) &&
		   JDirectoryReadable(dirName) &&
		   JCanEnterDirectory(dirName);
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JDirInfo::JDirInfo
	(
	const JString& dirName
	)
	:
	JContainer()
{
	BuildCWD(dirName);

	itsIsValidFlag         = true;	// see assert() at end of this function
	itsSwitchIfInvalidFlag = false;
	itsIsWritableFlag      = false;
	itsModTime             = 0;
	itsStatusTime          = 0;

	itsShowFilesFlag   = true;		// remember to update ResetCSFFilters()
	itsShowDirsFlag    = true;
	itsShowHiddenFlag  = false;
	itsShowVCSDirsFlag = true;
	itsShowOthersFlag  = false;

	itsNameRegex           = nullptr;
	itsOwnsNameRegexFlag   = false;
	itsInvertNameRegexFlag = false;
	itsFilterDirsFlag      = false;
	itsPermFilter          = nullptr;
	itsContentRegex        = nullptr;
	itsPG                  = nullptr;

	itsDirEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
	assert( itsDirEntries != nullptr);
	itsDirEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsDirEntries->SetSortOrder(JListT::kSortAscending);

	itsVisEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsVisEntries != nullptr);

	itsAlphaEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != nullptr);
	itsAlphaEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsAlphaEntries->SetSortOrder(JListT::kSortAscending);

	InstallCollection(itsVisEntries);

	const JError err = BuildInfo();
	assert_ok( err );
}

// private

void
JDirInfo::BuildCWD
	(
	const JString& dirName
	)
{
	const bool ok = JConvertToAbsolutePath(dirName, JString::empty, &itsCWD);
	assert( ok );
	JAppendDirSeparator(&itsCWD);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDirInfo::JDirInfo
	(
	const JDirInfo& source
	)
	:
	JContainer(source),
	itsCWD(source.itsCWD)
{
	itsIsValidFlag         = source.itsIsValidFlag;
	itsSwitchIfInvalidFlag = source.itsSwitchIfInvalidFlag;
	itsIsWritableFlag      = source.itsIsWritableFlag;
	itsModTime             = source.itsModTime;
	itsStatusTime          = source.itsStatusTime;

	JDirInfoX(source);
	PrivateCopySettings(source);
	CopyDirEntries(source);
	InstallCollection(itsVisEntries);
}

JDirInfo::JDirInfo
	(
	const JDirInfo&	source,
	const JString&	dirName
	)
	:
	JContainer(source)
{
	BuildCWD(dirName);

	itsIsValidFlag         = true;			// see assert() at end of this function
	itsSwitchIfInvalidFlag = false;
	itsIsWritableFlag      = false;
	itsModTime             = 0;
	itsStatusTime          = 0;

	JDirInfoX(source);
	PrivateCopySettings(source);
	InstallCollection(itsVisEntries);

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
	itsNameRegex           = nullptr;
	itsOwnsNameRegexFlag   = false;
	itsInvertNameRegexFlag = false;
	itsFilterDirsFlag      = false;
	itsPermFilter          = nullptr;
	itsContentRegex        = nullptr;
	itsPG                  = nullptr;

	itsDirEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
	assert( itsDirEntries != nullptr);

	itsVisEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsVisEntries != nullptr);

	itsAlphaEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kForgetAll);
	assert( itsAlphaEntries != nullptr);
	itsAlphaEntries->SetCompareFunction(JDirEntry::CompareNames);
	itsAlphaEntries->SetSortOrder(JListT::kSortAscending);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JDirInfo::~JDirInfo()
{
	jdelete itsDirEntries;
	jdelete itsVisEntries;
	jdelete itsAlphaEntries;

	if (itsOwnsNameRegexFlag)
	{
		jdelete itsNameRegex;
	}

	jdelete itsPermFilter;
	jdelete itsContentRegex;
	jdelete itsPG;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

JDirInfo&
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

	itsCWD = source.itsCWD;

	itsIsValidFlag         = source.itsIsValidFlag;
	itsSwitchIfInvalidFlag = source.itsSwitchIfInvalidFlag;
	itsIsWritableFlag      = source.itsIsWritableFlag;
	itsModTime             = source.itsModTime;
	itsStatusTime          = source.itsStatusTime;

	itsPermFilter = nullptr;

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
	const bool rebuild =
		itsContentRegex == nullptr || source.itsContentRegex == nullptr ||
		itsContentRegex->GetPattern() != source.itsContentRegex->GetPattern();

	PrivateCopySettings(source);

	if (rebuild)
	{
		ForceUpdate();
	}
	else
	{
		itsDirEntries->Sort();
		ApplyFilters(true);
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
	itsShowFilesFlag   = source.itsShowFilesFlag;
	itsShowDirsFlag    = source.itsShowDirsFlag;
	itsShowHiddenFlag  = source.itsShowHiddenFlag;
	itsShowVCSDirsFlag = source.itsShowVCSDirsFlag;
	itsShowOthersFlag  = source.itsShowOthersFlag;

	jdelete itsPermFilter;
	itsPermFilter = nullptr;
	if (source.itsPermFilter != nullptr)
	{
		itsPermFilter = jnew std::function(*source.itsPermFilter);
		assert( itsPermFilter != nullptr );
	}

	// copy name filter

	if (itsOwnsNameRegexFlag)
	{
		jdelete itsNameRegex;
	}
	itsNameRegex = nullptr;

	if (source.itsNameRegex != nullptr && !source.itsOwnsNameRegexFlag)
	{
		itsNameRegex         = source.itsNameRegex;
		itsOwnsNameRegexFlag = false;
	}
	else if (source.itsNameRegex != nullptr)
	{
		itsNameRegex = jnew JRegex(*source.itsNameRegex);
		itsOwnsNameRegexFlag = true;
	}

	itsInvertNameRegexFlag = source.itsInvertNameRegexFlag;
	itsFilterDirsFlag      = source.itsFilterDirsFlag;

	// copy content filter

	jdelete itsContentRegex;
	itsContentRegex = nullptr;

	if (source.itsContentRegex != nullptr)
	{
		itsContentRegex = jnew JRegex(*source.itsContentRegex);
	}

	// copy sort method

	itsDirEntries->CopyCompareFunction(*source.itsDirEntries);
	itsDirEntries->SetSortOrder(source.itsDirEntries->GetSortOrder());
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
	assert( itsDirEntries != nullptr );
	itsDirEntries->CleanOut();

	for (const auto* origEntry : *source.itsDirEntries)
	{
		auto* entry = jnew JDirEntry(*origEntry);
		itsDirEntries->Append(entry);
	}

	// update other file lists

	assert( itsVisEntries != nullptr && itsAlphaEntries != nullptr );

	ApplyFilters(false);
}

/******************************************************************************
 ShowFiles

 ******************************************************************************/

void
JDirInfo::ShowFiles
	(
	const bool show
	)
{
	if (show != itsShowFilesFlag)
	{
		itsShowFilesFlag = show;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ShowDirs

 ******************************************************************************/

void
JDirInfo::ShowDirs
	(
	const bool show
	)
{
	if (show != itsShowDirsFlag)
	{
		itsShowDirsFlag = show;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ShowHidden

 ******************************************************************************/

void
JDirInfo::ShowHidden
	(
	const bool show
	)
{
	if (show != itsShowHiddenFlag)
	{
		itsShowHiddenFlag = show;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ShowVCSDirs

 ******************************************************************************/

void
JDirInfo::ShowVCSDirs
	(
	const bool show
	)
{
	if (show != itsShowVCSDirsFlag)
	{
		itsShowVCSDirsFlag = show;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ShowOthers

 ******************************************************************************/

void
JDirInfo::ShowOthers
	(
	const bool show
	)
{
	if (show != itsShowOthersFlag)
	{
		itsShowOthersFlag = show;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ShouldApplyWildcardFilterToDirs

 ******************************************************************************/

void
JDirInfo::ShouldApplyWildcardFilterToDirs
	(
	const bool apply
	)
{
	if (apply != itsFilterDirsFlag)
	{
		itsFilterDirsFlag = apply;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 ChangeSort

 ******************************************************************************/

void
JDirInfo::ChangeSort
	(
	std::function<std::weak_ordering(JDirEntry * const &, JDirEntry * const &)> f,

	const JListT::SortOrder order
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
	assert( pg != nullptr );

	jdelete itsPG;
	itsPG = pg;
}

/******************************************************************************
 UseDefaultProgressDisplay

 ******************************************************************************/

void
JDirInfo::UseDefaultProgressDisplay()
{
	jdelete itsPG;
	itsPG = nullptr;
}

/******************************************************************************
 FindEntry

	Returns true if an entry with the given name exists.

 ******************************************************************************/

bool
JDirInfo::FindEntry
	(
	const JString&	name,
	JIndex*			index
	)
	const
{
	JDirEntry target(name, 0);
	return itsAlphaEntries->SearchSorted(&target, JListT::kFirstMatch, index);
}

/******************************************************************************
 ClosestMatch

	Returns the index of the closest match for the given name prefix.

 ******************************************************************************/

bool
JDirInfo::ClosestMatch
	(
	const JString&	prefixStr,
	JIndex*			index
	)
	const
{
	JDirEntry target(prefixStr, 0);
	bool found;
	*index = itsAlphaEntries->SearchSortedOTI(&target, JListT::kFirstMatch, &found);
	if (*index > itsAlphaEntries->GetItemCount())		// insert beyond end of list
	{
		*index = itsAlphaEntries->GetItemCount();
	}
	return *index > 0;
}

/*****************************************************************************
 GoUp

 ******************************************************************************/

JError
JDirInfo::GoUp()
{
	JString theCWD = itsCWD;

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
	const JString& dirName
	)
{
	const JString theCWD = JCombinePathAndName(itsCWD, dirName);
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
	const JString& origDirName
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
	const JString& origDirName
	)
{
	JString dirName;
	if (origDirName.IsEmpty() ||
		!JConvertToAbsolutePath(origDirName, JString::empty, &dirName))
	{
		return JBadPath(origDirName);
	}

	if (JSameDirEntry(dirName, itsCWD))
	{
		Update();
		return JNoError();
	}
	JAppendDirSeparator(&dirName);

	const JString origCWD = itsCWD;

	itsCWD = dirName;
	const JError err = BuildInfo();
	if (err.OK())
	{
		Broadcast(PathChanged());
	}
	else
	{
		itsCWD = origCWD;
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
	if (!JDirectoryReadable(itsCWD))
	{
		return JAccessDenied(itsCWD);
	}

	const JString origDir = JGetCurrentDirectory();

	JError err = JChangeDirectory(itsCWD);
	if (!err.OK())
	{
		return err;
	}

	// clear old information

	itsDirEntries->CleanOut();

	// update instance variables

	JStripTrailingDirSeparator(&itsCWD);		// keep Windows happy

	ACE_stat stbuf;
	ACE_OS::stat(itsCWD.GetBytes(), &stbuf);
	itsIsValidFlag    = true;
	itsIsWritableFlag = JDirectoryWritable(itsCWD);
	itsModTime        = stbuf.st_mtime;
	itsStatusTime     = stbuf.st_ctime;

	JAppendDirSeparator(&itsCWD);

	// process files in the directory

	JLatentPG pg(itsContentRegex == nullptr ? 100 : 10);
	if (itsPG != nullptr)
	{
		pg.SetPG(itsPG, false);
	}
	pg.VariableLengthProcessBeginning(JGetString("Scanning::JDirInfo"), true, true);

	CalledByBuildInfo(pg);

	pg.ProcessFinished();

	err = JChangeDirectory(origDir);
	assert_ok( err );

	ApplyFilters(false);
	return JNoError();
}

/*****************************************************************************
 Update

	Returns true if anything needed to be updated.

 ******************************************************************************/

bool
JDirInfo::Update
	(
	const bool force
	)
{
	ACE_stat info;
	if (force ||
		ACE_OS::lstat(itsCWD.GetBytes(), &info) != 0 ||
		ACE_OS::stat(itsCWD.GetBytes(), &info) != 0  ||
		itsModTime != (time_t) info.st_mtime)
	{
		ForceUpdate();
		return true;
	}
	else if (itsStatusTime != (time_t) info.st_ctime &&
			 JDirectoryReadable(itsCWD))
	{
		itsStatusTime     = info.st_ctime;
		itsIsWritableFlag = JDirectoryWritable(itsCWD);
		Broadcast(PermissionsChanged());
		return true;
	}
	else if (itsStatusTime != (time_t) info.st_ctime)
	{
		ForceUpdate();
		return true;
	}
	else
	{
		return false;
	}
}

/*****************************************************************************
 ForceUpdate

	Returns true if the update was successful.  Otherwise, returns
	false to indicate that path is no longer valid.

 ******************************************************************************/

bool
JDirInfo::ForceUpdate()
{
	if (JDirectoryExists(itsCWD))
	{
		Broadcast(ContentsWillBeUpdated());

		const JError err = BuildInfo();
		if (err.OK())
		{
			return true;
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
		itsIsValidFlag    = false;
		itsIsWritableFlag = false;
		itsDirEntries->CleanOut();
		itsVisEntries->CleanOut();
		itsAlphaEntries->CleanOut();
	}

	return false;
}

/******************************************************************************
 ApplyFilters (private)

	update should be true if the contents are merely being updated.

 ******************************************************************************/

void
JDirInfo::ApplyFilters
	(
	const bool update
	)
{
	if (update)
	{
		Broadcast(ContentsWillBeUpdated());
	}

	itsVisEntries->CleanOut();
	itsAlphaEntries->CleanOut();

	for (auto* entry : *itsDirEntries)
	{
		if (IsVisible(*entry))
		{
			itsVisEntries->Append(entry);
			itsAlphaEntries->InsertSorted(entry, true);
		}
	}

	Broadcast(ContentsChanged());
}

/******************************************************************************
 IsVisible (virtual protected)

 ******************************************************************************/

bool
JDirInfo::IsVisible
	(
	const JDirEntry& entry
	)
	const
{
	const JDirEntry::Type type = entry.GetType();
	const JString& name        = entry.GetName();

	if (!itsShowHiddenFlag && name.GetFirstCharacter() == '.' && name != "..")
	{
		return false;
	}

	if (!itsShowVCSDirsFlag && JIsVCSDirectory(name))
	{
		return false;
	}

	if (type == JDirEntry::kDir || type == JDirEntry::kDirLink)
	{
		return itsShowDirsFlag &&
				(!itsFilterDirsFlag || MatchesNameFilter(entry)) &&
				MatchesDirEntryFilter(entry);
	}
	else if (type == JDirEntry::kFile || type == JDirEntry::kFileLink ||
			 type == JDirEntry::kBrokenLink)
	{
		return itsShowFilesFlag &&
				MatchesNameFilter(entry) &&
				MatchesDirEntryFilter(entry);
	}
	else if (type == JDirEntry::kUnknown || type == JDirEntry::kUnknownLink)
	{
		return itsShowOthersFlag &&
				MatchesNameFilter(entry) &&
				MatchesDirEntryFilter(entry);
	}
	else if (type == JDirEntry::kDoesNotExist)
	{
		return false;
	}
	else
	{
		assert( 0 );	// this should never happen
		return false;
	}
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
JDirInfo::SetWildcardFilter
	(
	const JString&	filterStr,
	const bool		negate,
	const bool		caseSensitive
	)
{
	JString regexStr;
	if (!BuildRegexFromWildcardFilter(filterStr, &regexStr))
	{
		ClearWildcardFilter();
		return;
	}

	auto* r = jnew JRegex(regexStr);
	r->SetCaseSensitive(caseSensitive);

	SetWildcardFilter(r, true, negate);
}

/******************************************************************************
 SetWildcardFilter

 ******************************************************************************/

void
JDirInfo::SetWildcardFilter
	(
	JRegex*		filter,
	const bool	dirInfoOwnsRegex,
	const bool	negate
	)
{
	if (filter == nullptr)
	{
		ClearWildcardFilter();
		return;
	}

	if (itsOwnsNameRegexFlag)
	{
		jdelete itsNameRegex;
	}

	itsNameRegex = filter;
	assert( itsNameRegex != nullptr );

	itsOwnsNameRegexFlag   = dirInfoOwnsRegex;
	itsInvertNameRegexFlag = negate;

	ApplyFilters(true);
	Broadcast(SettingsChanged());
}

/******************************************************************************
 ClearWildcardFilter

 ******************************************************************************/

void
JDirInfo::ClearWildcardFilter()
{
	if (itsNameRegex != nullptr)
	{
		if (itsOwnsNameRegexFlag)
		{
			jdelete itsNameRegex;
		}
		itsNameRegex         = nullptr;
		itsOwnsNameRegexFlag = false;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 BuildRegexFromWildcardFilter (static)

	Converts a wildcard filter ("*.cc *.h") to a regex ("^.*\.cc$|^.*\.h$").
	Returns false if the given wildcard filter string is empty.

 ******************************************************************************/

static const JRegex theFilterSplitPattern = "\\s+";

bool
JDirInfo::BuildRegexFromWildcardFilter
	(
	const JString&	origFilterStr,
	JString*		regexStr
	)
{
	regexStr->Clear();

	JString filterStr = origFilterStr;
	filterStr.TrimWhitespace();
	if (filterStr.IsEmpty())
	{
		return false;
	}

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	filterStr.Split(theFilterSplitPattern, &list);

	for (const auto* s : list)
	{
		AppendRegex(*s, regexStr);
	}

	return true;
}

/******************************************************************************
 AppendRegex (static private)

 ******************************************************************************/

void
JDirInfo::AppendRegex
	(
	const JString&	origStr,
	JString*		regexStr
	)
{
	JString str = origStr;

	// Convert wildcard multiples (*) to regex multiples (.*)
	// and wildcard singles (?) to regex singles (.)

	JStringIterator iter(&str, JStringIterator::kStartAtEnd);
	JUtf8Character c;
	while (iter.Prev(&c))
	{
		if (c == '*')
		{
			iter.Insert(".");
		}
		else if (c == '?')
		{
			iter.SetNext(JUtf8Character('.'), JStringIterator::kStay);
		}
		else if (JRegex::NeedsBackslashToBeLiteral(c))
		{
			iter.Insert("\\");
		}
	}
	iter.Invalidate();

	// Add instructions that it must match the entire file name.

	str.Prepend("^");
	str.Append("$");

	// append to regexStr

	if (!regexStr->IsEmpty())
	{
		*regexStr += "|";
	}
	*regexStr += str;
}

/******************************************************************************
 MatchesNameFilter (protected)

 ******************************************************************************/

bool
JDirInfo::MatchesNameFilter
	(
	const JDirEntry& entry
	)
	const
{
	if (itsNameRegex != nullptr)
	{
		const JString& name = entry.GetName();
		bool match      = itsNameRegex->Match(name);
		if (itsInvertNameRegexFlag)
		{
			match = !match;
		}
		return match;
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 SetDirEntryFilter

 ******************************************************************************/

void
JDirInfo::SetDirEntryFilter
	(
	const std::function<bool(const JDirEntry&)> f
	)
{
	jdelete itsPermFilter;
	itsPermFilter = jnew std::function(f);
	assert( itsPermFilter != nullptr );

	ApplyFilters(true);
	Broadcast(SettingsChanged());
}

/******************************************************************************
 ClearDirEntryFilter

 ******************************************************************************/

void
JDirInfo::ClearDirEntryFilter()
{
	if (itsPermFilter != nullptr)
	{
		jdelete itsPermFilter;
		itsPermFilter = nullptr;
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 MatchesDirEntryFilter (protected)

 ******************************************************************************/

bool
JDirInfo::MatchesDirEntryFilter
	(
	const JDirEntry& entry
	)
	const
{
	return itsPermFilter != nullptr ? (*itsPermFilter)(entry) : true;
}

/******************************************************************************
 SetContentFilter

 ******************************************************************************/

JError
JDirInfo::SetContentFilter
	(
	const JString& regexStr
	)
{
	if (itsContentRegex != nullptr && regexStr == itsContentRegex->GetPattern())
	{
		return JNoError();
	}

	bool hadFilter = true;
	JString prevPattern;
	if (itsContentRegex == nullptr)
	{
		hadFilter       = false;
		itsContentRegex = jnew JRegex;
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
		jdelete itsContentRegex;
		itsContentRegex = nullptr;
	}
	return err;
}

/******************************************************************************
 ClearContentFilter

 ******************************************************************************/

void
JDirInfo::ClearContentFilter()
{
	if (itsContentRegex != nullptr)
	{
		jdelete itsContentRegex;
		itsContentRegex = nullptr;
		ForceUpdate();
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 MatchesContentFilter (protected)

 ******************************************************************************/

bool
JDirInfo::MatchesContentFilter
	(
	const JDirEntry& entry
	)
	const
{
	if (itsContentRegex == nullptr || entry.IsDirectory())
	{
		return true;
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
	bool apply = false, rebuild = false;

	if (!itsShowFilesFlag)
	{
		itsShowFilesFlag = true;
		apply            = true;
	}
	if (!itsShowDirsFlag)
	{
		itsShowDirsFlag = true;
		apply           = true;
	}
//	if (itsShowHiddenFlag)
//		{
//		itsShowHiddenFlag = false;
//		apply             = true;
//		}
	if (!itsShowVCSDirsFlag)
	{
		itsShowVCSDirsFlag = true;
		apply              = true;
	}
	if (itsShowOthersFlag)
	{
		itsShowOthersFlag = false;
		apply             = true;
	}

	if (itsPermFilter != nullptr)
	{
		jdelete itsPermFilter;
		itsPermFilter = nullptr;
		apply         = true;
	}
	if (itsContentRegex != nullptr)
	{
		jdelete itsContentRegex;
		itsContentRegex = nullptr;
		rebuild         = true;
	}

	if (rebuild)
	{
		ForceUpdate();
		Broadcast(SettingsChanged());
	}
	else if (apply)
	{
		ApplyFilters(true);
		Broadcast(SettingsChanged());
	}
}

/******************************************************************************
 Empty (static)

	Returns true if the directory doesn't exist or exists and is empty.

	Not called IsEmpty(), because that would conflict with our base class,
	JCollection.

 ******************************************************************************/

bool
JDirInfo::Empty
	(
	const JString& dirName
	)
{
	JDirInfo* info;
	if (!Create(dirName, &info))
	{
		return true;
	}
	const bool empty = info->IsEmpty();
	jdelete info;
	return empty;
}
