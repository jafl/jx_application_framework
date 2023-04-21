/******************************************************************************
 JDirEntry.h

	Interface for the JDirEntry class

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JDirEntry
#define _H_JDirEntry

#include "JList.h"
#include "JError.h"
#include "JString.h"
#include <sys/stat.h>

#if defined WIN32 && !defined GetUserName
#define GetUserName	GetUserNameA
#endif

class JDirInfo;
class JRegex;

class JDirEntry 
{
	friend class JDirInfo;

public:

	enum Type
	{
		kDoesNotExist = 0,
		kDir,
		kFile,
		kUnknown,
		kDirLink,
		kFileLink,
		kBrokenLink,
		kUnknownLink
	};

	enum ModeBit
	{
		kOtherExec = 0,
		kOtherWrite,
		kOtherRead,
		kGroupExec,
		kGroupWrite,
		kGroupRead,
		kUserExec,
		kUserWrite,
		kUserRead
	};

public:

	JDirEntry(const JString& fullName);
	JDirEntry(const JString& pathName, const JString& fileName);
	JDirEntry(const JDirEntry& source);

	~JDirEntry();

	JDirEntry& operator=(const JDirEntry& source);

	const JString&	GetPath() const;
	const JString&	GetName() const;
	const JString&	GetFullName() const;
	bool			GetLinkName(const JString** linkName) const;
	const JString&	GetUserName() const;
	const JString&	GetGroupName() const;

	Type	GetType() const;
	bool	IsFile() const;
	bool	IsDirectory() const;
	bool	IsLink() const;
	bool	IsWorkingLink() const;
	bool	IsBrokenLink() const;
	bool	IsUnknown() const;

	JSize	GetSize() const;

	mode_t	GetMode() const;
	JError	SetMode(const mode_t mode);
	JError	SetMode(const ModeBit bit, const bool allow);
	JString	GetModeString() const;

	bool	IsReadable() const;
	bool	IsWritable() const;
	bool	IsExecutable() const;

	time_t	GetModTime() const;
	time_t	GetStatusTime() const;
	time_t	GetAccessTime() const;

	JIndex	GetUserID() const;
	JIndex	GetGroupID() const;

	JDirEntry	FollowLink() const;

	bool	NeedsUpdate() const;
	bool	Update(const bool force = false);		// updates if necessary
	void	ForceUpdate();								// updates regardless

	bool	MatchesContentFilter(const JRegex& regex,
								 const JSize kBlockSize = 1024) const;

	static std::weak_ordering
		CompareNames(JDirEntry * const & e1, JDirEntry * const & e2);
	static std::weak_ordering
		CompareSizes(JDirEntry * const & e1, JDirEntry * const & e2);
	static std::weak_ordering
		CompareModTimes(JDirEntry * const & e1, JDirEntry * const & e2);

private:

	JString		itsPath;
	JString		itsName;
	JString		itsFullName;
	JString*	itsLinkName;		// nullptr unless it is a link

	Type		itsType;
	JSize		itsSize;
	mode_t		itsMode;
	time_t		itsModTime;			// from lstat
	time_t		itsStatusTime;
	time_t		itsAccessTime;
	time_t		itsSModTime;		// from stat
	time_t		itsSStatusTime;
	uid_t		itsUserID;
	JString*	itsUserName;		// nullptr until first needed
	gid_t		itsGroupID;
	JString*	itsGroupName;		// nullptr until first needed

	bool	itsIsReadableFlag;
	bool	itsIsWritableFlag;
	bool	itsIsExecutableFlag;

private:

	void	JDirEntryX(const JString& fullName);

	// called by JDirInfo when building search targets

	JDirEntry(const JString& fileName, int x);
};


/*****************************************************************************
 GetType

 ******************************************************************************/

inline JDirEntry::Type
JDirEntry::GetType()
	const
{
	return itsType;
}

inline bool
JDirEntry::IsFile()
	const
{
	return itsType == kFile || itsType == kFileLink;
}

inline bool
JDirEntry::IsDirectory()
	const
{
	return itsType == kDir || itsType == kDirLink;
}

inline bool
JDirEntry::IsLink()
	const
{
	return itsType == kFileLink || itsType == kDirLink ||
							  itsType == kBrokenLink || itsType == kUnknownLink;
}

inline bool
JDirEntry::IsWorkingLink()
	const
{
	return itsType == kFileLink || itsType == kDirLink ||
							  itsType == kUnknownLink;
}

inline bool
JDirEntry::IsBrokenLink()
	const
{
	return itsType == kBrokenLink;
}

inline bool
JDirEntry::IsUnknown()
	const
{
	return itsType == kUnknown || itsType == kUnknownLink;
}

/*****************************************************************************
 GetPath

 ******************************************************************************/

inline const JString&
JDirEntry::GetPath()
	const
{
	return itsPath;
}

/*****************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
JDirEntry::GetName()
	const
{
	return itsName;
}

/*****************************************************************************
 GetFullName

 ******************************************************************************/

inline const JString&
JDirEntry::GetFullName()
	const
{
	return itsFullName;
}

/******************************************************************************
 GetLinkName

 ******************************************************************************/

inline bool
JDirEntry::GetLinkName
	(
	const JString** linkName
	)
	const
{
	*linkName = itsLinkName;
	return itsLinkName != nullptr;
}

/*****************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
JDirEntry::GetSize()
	const
{
	return itsSize;
}

/*****************************************************************************
 GetMode

 ******************************************************************************/

inline mode_t
JDirEntry::GetMode()
	const
{
	return itsMode;
}

/*****************************************************************************
 GetModTime

 ******************************************************************************/

inline time_t
JDirEntry::GetModTime()
	const
{
	return itsModTime;
}

/*****************************************************************************
 GetStatusTime

 ******************************************************************************/

inline time_t
JDirEntry::GetStatusTime()
	const
{
	return itsStatusTime;
}

/*****************************************************************************
 GetAccessTime

 ******************************************************************************/

inline time_t
JDirEntry::GetAccessTime()
	const
{
	return itsAccessTime;
}

/*****************************************************************************
 Access

 ******************************************************************************/

inline bool
JDirEntry::IsReadable()
	const
{
	return itsIsReadableFlag;
}

inline bool
JDirEntry::IsWritable()
	const
{
	return itsIsWritableFlag;
}

inline bool
JDirEntry::IsExecutable()
	const
{
	return itsIsExecutableFlag;
}

/*****************************************************************************
 GetUserID

 ******************************************************************************/

inline JIndex
JDirEntry::GetUserID()
	const
{
	return itsUserID;
}

/****************************************************************************
 GetGroupID

 ******************************************************************************/

inline JIndex
JDirEntry::GetGroupID()
	const
{
	return itsGroupID;
}

#endif
