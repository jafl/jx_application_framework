/******************************************************************************
 JDirInfo.h

	Interface for the JDirInfo class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JDirInfo
#define _H_JDirInfo

#include <JContainer.h>
#include <JDirEntry.h>	// for convenience
#include <JPtrArray-JString.h>
#include <JError.h>
#include <jTime.h>

class JRegex;
class JProgressDisplay;

typedef JListT::CompareResult
	(JCompareDirEntries)(JDirEntry * const &, JDirEntry * const &);

typedef JBoolean (JCheckPermissions)(const JDirEntry&);

class JDirInfo : public JContainer
{
public:

	static JBoolean	Create(const JString& dirName, JDirInfo** obj);
	static JBoolean	Create(const JDirInfo& source, const JString& dirName,
						   JDirInfo** obj);
	static JBoolean	OKToCreate(const JString& dirName);

	JDirInfo(const JDirInfo& source);
	JDirInfo(const JDirInfo& source, const JString& dirName);

	virtual ~JDirInfo();

	const JDirInfo& operator=(const JDirInfo& source);
	void			CopySettings(const JDirInfo& source);

	JBoolean		DirectoryExists() const;
	const JString&	GetDirectory() const;
	JBoolean		IsWritable() const;

	JError	GoUp();
	JError	GoDown(const JString& dirName);
	void	GoToClosest(const JString& dirName);
	JError	GoTo(const JString& dirName);

	void		ChangeSort(JCompareDirEntries* f, const JListT::SortOrder order);

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
	void		SetWildcardFilter(const JString& filterStr,
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
	JError		SetContentFilter(const JString& regexStr);
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
	JBoolean			FindEntry(const JString& name, JIndex* index) const;
	JBoolean			ClosestMatch(const JString& prefixStr, JIndex* index) const;

	static JBoolean	BuildRegexFromWildcardFilter(const JString& filterStr,
												 JString* regexStr);

	static JBoolean	Empty(const JString& dirName);

	// range-based for loop

	JDirEntry**	begin() const;
	JDirEntry**	end() const;

protected:

	JDirInfo(const JString& dirName);

	virtual JBoolean	IsVisible(const JDirEntry& entry) const;
	JBoolean			MatchesNameFilter(const JDirEntry& entry) const;
	JBoolean			MatchesDirEntryFilter(const JDirEntry& entry) const;
	JBoolean			MatchesContentFilter(const JDirEntry& entry) const;

private:

	JBoolean	itsIsValidFlag;
	JBoolean	itsSwitchIfInvalidFlag;
	JString		itsCWD;
	JBoolean	itsIsWritableFlag;
	time_t		itsModTime;
	time_t		itsStatusTime;

	JBoolean	itsShowFilesFlag;
	JBoolean	itsShowDirsFlag;
	JBoolean	itsShowHiddenFlag;
	JBoolean	itsShowVCSDirsFlag;
	JBoolean	itsShowOthersFlag;
	JRegex*		itsNameRegex;			// can be nullptr
	JBoolean	itsOwnsNameRegexFlag;
	JBoolean	itsInvertNameRegexFlag;
	JBoolean	itsFilterDirsFlag;
	JRegex*		itsContentRegex;		// can be nullptr

	JCheckPermissions*	itsPermFilter;	// can be nullptr

	JPtrArray<JDirEntry>*	itsDirEntries;		// everything, current sorting fn
	JPtrArray<JDirEntry>*	itsVisEntries;		// visible, current sorting fn
	JPtrArray<JDirEntry>*	itsAlphaEntries;	// visible, sorted alphabetically

	JProgressDisplay*	itsPG;			// can be nullptr

private:

	void	JDirInfoX(const JDirInfo& source);
	void	BuildCWD(const JString& dirName);
	void	PrivateCopySettings(const JDirInfo& source);
	void	CopyDirEntries(const JDirInfo& source);

	JError	BuildInfo();
	void	BuildInfo1(JProgressDisplay& pg);
	void	ApplyFilters(const JBoolean update);

	static void	AppendRegex(const JString& origStr, JString* regexStr);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kContentsWillBeUpdated;
	static const JUtf8Byte* kContentsChanged;
	static const JUtf8Byte* kPathChanged;
	static const JUtf8Byte* kPermissionsChanged;
	static const JUtf8Byte* kSettingsChanged;

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
	return itsCWD;
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
	return JNegate( itsNameRegex == nullptr );
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
	return JNegate( itsPermFilter == nullptr );
}

inline JBoolean
JDirInfo::HasContentFilter()
	const
{
	return JNegate( itsContentRegex == nullptr );
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
	return *(itsVisEntries->GetElement(index));
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

/******************************************************************************
 Range-based for loop

 ******************************************************************************/

inline JDirEntry**
JDirInfo::begin()
	const
{
	return ::begin(*itsVisEntries);
}

inline JDirEntry**
JDirInfo::end()
	const
{
	return ::end(*itsVisEntries);
}

#endif
