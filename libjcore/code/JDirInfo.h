/******************************************************************************
 JDirInfo.h

	Interface for the JDirInfo class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDirInfo
#define _H_JDirInfo

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JDirEntry.h>	// for convenience
#include <JPtrArray.h>
#include <JError.h>
#include <jTime.h>

class JRegex;
class JString;
class JProgressDisplay;

typedef JOrderedSetT::CompareResult
	(JCompareDirEntries)(JDirEntry * const &, JDirEntry * const &);

typedef JBoolean (JCheckPermissions)(const JDirEntry&);

class JDirInfo : public JContainer
{
public:

	static JBoolean	Create(const JCharacter* dirName, JDirInfo** obj);
	static JBoolean	Create(const JDirInfo& source, const JCharacter* dirName,
						   JDirInfo** obj);
	static JBoolean	OKToCreate(const JCharacter* dirName);

	JDirInfo(const JDirInfo& source);
	JDirInfo(const JDirInfo& source, const JCharacter* dirName);

	virtual ~JDirInfo();

	const JDirInfo& operator=(const JDirInfo& source);
	void			CopySettings(const JDirInfo& source);

	JBoolean		DirectoryExists() const;
	const JString&	GetDirectory() const;
	JBoolean		IsWritable() const;

	JError	GoUp();
	JError	GoDown(const JCharacter* dirName);
	void	GoToClosest(const JCharacter* dirName);
	JError	GoTo(const JCharacter* dirName);

	void		ChangeSort(JCompareDirEntries* f, const JOrderedSetT::SortOrder order);

	JBoolean	FilesVisible() const;
	void		ShowFiles(const JBoolean show);

	JBoolean	DirsVisible() const;
	void		ShowDirs(const JBoolean show);

	JBoolean	HiddenVisible() const;
	void		ShowHidden(const JBoolean show);

	JBoolean	VCSDirsVisible() const;
	void		ShowVCSDirs(const JBoolean show);

	JBoolean	OthersVisible() const;
	void		ShowOthers(const JBoolean show);

	JBoolean	HasWildcardFilter() const;
	void		SetWildcardFilter(const JCharacter* filterStr,
								  const JBoolean negate = kJFalse,
								  const JBoolean caseSensitive = kJFalse);
	void		SetWildcardFilter(JRegex* filter, const JBoolean dirInfoOwnsRegex,
								  const JBoolean negate = kJFalse);
	void		ClearWildcardFilter();

	JBoolean	WillApplyWildcardFilterToDirs() const;
	void		ShouldApplyWildcardFilterToDirs(const JBoolean apply = kJTrue);

	JBoolean	HasDirEntryFilter() const;
	void		SetDirEntryFilter(JCheckPermissions* f);
	void		ClearDirEntryFilter();

	JBoolean	HasContentFilter() const;
	JError		SetContentFilter(const JCharacter* regexStr);
	void		ClearContentFilter();

	void		ResetCSFFilters();

	void		ChangeProgressDisplay(JProgressDisplay* pg);
	void		UseDefaultProgressDisplay();

	JBoolean	WillSwitchToValidDirectory() const;
	void		ShouldSwitchToValidDirectory(const JBoolean switchIfInvalid = kJTrue);

	JBoolean	Update(const JBoolean force = kJFalse);		// updates if necessary
	JBoolean	ForceUpdate();								// updates regardless

	JSize				GetEntryCount() const;
	const JDirEntry&	GetEntry(const JIndex index) const;
	JBoolean			FindEntry(const JCharacter* name, JIndex* index) const;
	JBoolean			ClosestMatch(const JCharacter* prefixStr, JIndex* index) const;

	static JBoolean	BuildRegexFromWildcardFilter(const JCharacter* filterStr,
												 JString* regexStr);

	static JBoolean	Empty(const JCharacter* dirName);

protected:

	JDirInfo(const JCharacter* dirName);

	virtual JBoolean	IsVisible(const JDirEntry& entry) const;
	JBoolean			MatchesNameFilter(const JDirEntry& entry) const;
	JBoolean			MatchesDirEntryFilter(const JDirEntry& entry) const;
	JBoolean			MatchesContentFilter(const JDirEntry& entry) const;

private:

	JBoolean	itsIsValidFlag;
	JBoolean	itsSwitchIfInvalidFlag;
	JString*	itsCWD;
	JBoolean	itsIsWritableFlag;
	time_t		itsModTime;
	time_t		itsStatusTime;

	JBoolean	itsShowFilesFlag;
	JBoolean	itsShowDirsFlag;
	JBoolean	itsShowHiddenFlag;
	JBoolean	itsShowVCSDirsFlag;
	JBoolean	itsShowOthersFlag;
	JRegex*		itsNameRegex;			// can be NULL
	JBoolean	itsOwnsNameRegexFlag;
	JBoolean	itsInvertNameRegexFlag;
	JBoolean	itsFilterDirsFlag;
	JRegex*		itsContentRegex;		// can be NULL

	JCheckPermissions*	itsPermFilter;	// can be NULL

	JPtrArray<JDirEntry>*	itsDirEntries;		// everything, current sorting fn
	JPtrArray<JDirEntry>*	itsVisEntries;		// visible, current sorting fn
	JPtrArray<JDirEntry>*	itsAlphaEntries;	// visible, sorted alphabetically

	JProgressDisplay*	itsPG;			// can be NULL

private:

	void	JDirInfoX(const JDirInfo& source);
	void	AllocateCWD(const JCharacter* dirName);
	void	PrivateCopySettings(const JDirInfo& source);
	void	CopyDirEntries(const JDirInfo& source);

	JError	BuildInfo();
	void	BuildInfo1(JProgressDisplay& pg);
	void	ApplyFilters(const JBoolean update);

	static void	AppendRegex(const JCharacter* origStr, JString* regexStr);

public:

	// JBroadcaster messages

	static const JCharacter* kContentsWillBeUpdated;
	static const JCharacter* kContentsChanged;
	static const JCharacter* kPathChanged;
	static const JCharacter* kPermissionsChanged;
	static const JCharacter* kSettingsChanged;

	class ContentsWillBeUpdated : public JBroadcaster::Message
		{
		public:

			ContentsWillBeUpdated()
				:
				JBroadcaster::Message(kContentsWillBeUpdated)
				{ };
		};

	class ContentsChanged : public JBroadcaster::Message
		{
		public:

			ContentsChanged()
				:
				JBroadcaster::Message(kContentsChanged)
				{ };
		};

	class PathChanged : public JBroadcaster::Message
		{
		public:

			PathChanged()
				:
				JBroadcaster::Message(kPathChanged)
			{ };
		};

	class PermissionsChanged : public JBroadcaster::Message
		{
		public:

			PermissionsChanged()
				:
				JBroadcaster::Message(kPermissionsChanged)
			{ };
		};

	class SettingsChanged : public JBroadcaster::Message
		{
		public:

			SettingsChanged()
				:
				JBroadcaster::Message(kSettingsChanged)
			{ };
		};
};


/******************************************************************************
 DirectoryExists

 ******************************************************************************/

inline JBoolean
JDirInfo::DirectoryExists()
	const
{
	return itsIsValidFlag;
}

/******************************************************************************
 GetDirectory

 ******************************************************************************/

inline const JString&			 
JDirInfo::GetDirectory()
	const
{
	return *itsCWD;
}

/******************************************************************************
 Filters

 ******************************************************************************/

inline JBoolean
JDirInfo::FilesVisible()
	const
{
	return itsShowFilesFlag;
}

inline JBoolean
JDirInfo::DirsVisible()
	const
{
	return itsShowDirsFlag;
}

inline JBoolean
JDirInfo::HiddenVisible()
	const
{
	return itsShowHiddenFlag;
}

inline JBoolean
JDirInfo::VCSDirsVisible()
	const
{
	return itsShowVCSDirsFlag;
}

inline JBoolean
JDirInfo::OthersVisible()
	const
{
	return itsShowOthersFlag;
}

inline JBoolean
JDirInfo::HasWildcardFilter()
	const
{
	return JNegate( itsNameRegex == NULL );
}

inline JBoolean
JDirInfo::WillApplyWildcardFilterToDirs()
	const
{
	return itsFilterDirsFlag;
}

inline JBoolean
JDirInfo::HasDirEntryFilter()
	const
{
	return JNegate( itsPermFilter == NULL );
}

inline JBoolean
JDirInfo::HasContentFilter()
	const
{
	return JNegate( itsContentRegex == NULL );
}

/******************************************************************************
 IsWritable

 ******************************************************************************/

inline JBoolean
JDirInfo::IsWritable()
	const
{
	return itsIsWritableFlag;
}

/******************************************************************************
 GetEntryCount

 ******************************************************************************/

inline JSize
JDirInfo::GetEntryCount()
	const
{
	return GetElementCount();
}

/******************************************************************************
 GetEntry

 ******************************************************************************/

inline const JDirEntry&
JDirInfo::GetEntry
	(
	const JIndex index
	)
	const
{
	return *(itsVisEntries->NthElement(index));
}

/******************************************************************************
 Switch to valid directory if curr dir is invalid

 ******************************************************************************/

inline JBoolean
JDirInfo::WillSwitchToValidDirectory()
	const
{
	return itsSwitchIfInvalidFlag;
}

inline void
JDirInfo::ShouldSwitchToValidDirectory
	(
	const JBoolean switchIfInvalid
	)
{
	itsSwitchIfInvalidFlag = switchIfInvalid;
	if (itsSwitchIfInvalidFlag && !itsIsValidFlag)
		{
		ForceUpdate();
		}
}

#endif
