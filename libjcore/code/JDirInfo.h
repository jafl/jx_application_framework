/******************************************************************************
 JDirInfo.h

	Interface for the JDirInfo class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JDirInfo
#define _H_JDirInfo

#include "jx-af/jcore/JContainer.h"
#include "jx-af/jcore/JDirEntry.h"	// for convenience
#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JError.h"
#include "jx-af/jcore/jTime.h"

class JRegex;
class JProgressDisplay;

class JDirInfo : public JContainer
{
public:

	static bool	Create(const JString& dirName, JDirInfo** obj);
	static bool	Create(const JDirInfo& source, const JString& dirName,
					   JDirInfo** obj);
	static bool	OKToCreate(const JString& dirName);

	JDirInfo(const JDirInfo& source);
	JDirInfo(const JDirInfo& source, const JString& dirName);

	~JDirInfo() override;

	JDirInfo&	operator=(const JDirInfo& source);
	void		CopySettings(const JDirInfo& source);

	bool			DirectoryExists() const;
	const JString&	GetDirectory() const;
	bool			IsWritable() const;

	JError	GoUp();
	JError	GoDown(const JString& dirName);
	void	GoToClosest(const JString& dirName);
	JError	GoTo(const JString& dirName);

	void	ChangeSort(const std::function<JListT::CompareResult(JDirEntry * const &, JDirEntry * const &)> f,
					   const JListT::SortOrder order);

	bool	FilesVisible() const;
	void	ShowFiles(const bool show);

	bool	DirsVisible() const;
	void	ShowDirs(const bool show);

	bool	HiddenVisible() const;
	void	ShowHidden(const bool show);

	bool	VCSDirsVisible() const;
	void	ShowVCSDirs(const bool show);

	bool	OthersVisible() const;
	void	ShowOthers(const bool show);

	bool	HasWildcardFilter() const;
	void	SetWildcardFilter(const JString& filterStr,
							  const bool negate = false,
							  const bool caseSensitive = false);
	void	SetWildcardFilter(JRegex* filter, const bool dirInfoOwnsRegex,
							  const bool negate = false);
	void	ClearWildcardFilter();

	bool	WillApplyWildcardFilterToDirs() const;
	void	ShouldApplyWildcardFilterToDirs(const bool apply = true);

	bool	HasDirEntryFilter() const;
	void	SetDirEntryFilter(const std::function<bool(const JDirEntry&)> f);
	void	ClearDirEntryFilter();

	bool	HasContentFilter() const;
	JError	SetContentFilter(const JString& regexStr);
	void	ClearContentFilter();

	void	ResetCSFFilters();

	void	ChangeProgressDisplay(JProgressDisplay* pg);
	void	UseDefaultProgressDisplay();

	bool	WillSwitchToValidDirectory() const;
	void	ShouldSwitchToValidDirectory(const bool switchIfInvalid = true);

	bool	Update(const bool force = false);		// updates if necessary
	bool	ForceUpdate();								// updates regardless

	JSize				GetEntryCount() const;
	const JDirEntry&	GetEntry(const JIndex index) const;
	bool				FindEntry(const JString& name, JIndex* index) const;
	bool				ClosestMatch(const JString& prefixStr, JIndex* index) const;

	static bool	BuildRegexFromWildcardFilter(const JString& filterStr,
											 JString* regexStr);

	static bool	Empty(const JString& dirName);

	// range-based for loop

	JDirEntry**	begin() const;
	JDirEntry**	end() const;

protected:

	JDirInfo(const JString& dirName);

	virtual bool	IsVisible(const JDirEntry& entry) const;
	bool			MatchesNameFilter(const JDirEntry& entry) const;
	bool			MatchesDirEntryFilter(const JDirEntry& entry) const;
	bool			MatchesContentFilter(const JDirEntry& entry) const;

private:

	bool	itsIsValidFlag;
	bool	itsSwitchIfInvalidFlag;
	JString	itsCWD;
	bool	itsIsWritableFlag;
	time_t	itsModTime;
	time_t	itsStatusTime;

	bool	itsShowFilesFlag;
	bool	itsShowDirsFlag;
	bool	itsShowHiddenFlag;
	bool	itsShowVCSDirsFlag;
	bool	itsShowOthersFlag;
	JRegex*	itsNameRegex;			// can be nullptr
	bool	itsOwnsNameRegexFlag;
	bool	itsInvertNameRegexFlag;
	bool	itsFilterDirsFlag;
	JRegex*	itsContentRegex;		// can be nullptr

	std::function<bool(const JDirEntry&)>*	itsPermFilter;	// can be nullptr

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
	void	CalledByBuildInfo(JProgressDisplay& pg);
	void	ApplyFilters(const bool update);

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

inline bool
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

inline bool
JDirInfo::FilesVisible()
	const
{
	return itsShowFilesFlag;
}

inline bool
JDirInfo::DirsVisible()
	const
{
	return itsShowDirsFlag;
}

inline bool
JDirInfo::HiddenVisible()
	const
{
	return itsShowHiddenFlag;
}

inline bool
JDirInfo::VCSDirsVisible()
	const
{
	return itsShowVCSDirsFlag;
}

inline bool
JDirInfo::OthersVisible()
	const
{
	return itsShowOthersFlag;
}

inline bool
JDirInfo::HasWildcardFilter()
	const
{
	return itsNameRegex != nullptr;
}

inline bool
JDirInfo::WillApplyWildcardFilterToDirs()
	const
{
	return itsFilterDirsFlag;
}

inline bool
JDirInfo::HasDirEntryFilter()
	const
{
	return itsPermFilter != nullptr;
}

inline bool
JDirInfo::HasContentFilter()
	const
{
	return itsContentRegex != nullptr;
}

/******************************************************************************
 IsWritable

 ******************************************************************************/

inline bool
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

inline bool
JDirInfo::WillSwitchToValidDirectory()
	const
{
	return itsSwitchIfInvalidFlag;
}

inline void
JDirInfo::ShouldSwitchToValidDirectory
	(
	const bool switchIfInvalid
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
