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
#include "jMissingProto.h"

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

	const JDirEntry& operator=(const JDirEntry& source);

	const JString&	GetPath() const;
	const JString&	GetName() const;
	const JString&	GetFullName() const;
	JBoolean		GetLinkName(const JString** linkName) const;
	const JString&	GetUserName() const;
	const JString&	GetGroupName() const;

	Type		GetType() const;
	JBoolean	IsFile() const;
	JBoolean	IsDirectory() const;
	JBoolean	IsLink() const;
	JBoolean	IsWorkingLink() const;
	JBoolean	IsBrokenLink() const;
	JBoolean	IsUnknown() const;

	JSize	GetSize() const;

	mode_t	GetMode() const;
	JError	SetMode(const mode_t mode);
	JError	SetMode(const ModeBit bit, const JBoolean allow);
	JString	GetModeString() const;

	JBoolean	IsReadable() const;
	JBoolean	IsWritable() const;
	JBoolean	IsExecutable() const;

	time_t	GetModTime() const;
	time_t	GetStatusTime() const;
	time_t	GetAccessTime() const;

	JIndex	GetUserID() const;
	JIndex	GetGroupID() const;

	JDirEntry	FollowLink() const;

	JBoolean	NeedsUpdate() const;
	JBoolean	Update(const JBoolean force = kJFalse);		// updates if necessary
	void		ForceUpdate();								// updates regardless

	JBoolean	MatchesContentFilter(const JRegex& regex,
									 const JSize kBlockSize = 1024) const;

	static JListT::CompareResult
		CompareNames(JDirEntry * const & e1, JDirEntry * const & e2);
	static JListT::CompareResult
		CompareSizes(JDirEntry * const & e1, JDirEntry * const & e2);
	static JListT::CompareResult
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

	JBoolean	itsIsReadableFlag;
	JBoolean	itsIsWritableFlag;
	JBoolean	itsIsExecutableFlag;

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

inline JBoolean
JDirEntry::IsFile()
	const
{
	return JConvertToBoolean( itsType == kFile || itsType == kFileLink );
}

inline JBoolean
JDirEntry::IsDirectory()
	const
{
	return JConvertToBoolean( itsType == kDir || itsType == kDirLink );
}

inline JBoolean
JDirEntry::IsLink()
	const
{
	return JConvertToBoolean( itsType == kFileLink || itsType == kDirLink ||
							  itsType == kBrokenLink || itsType == kUnknownLink );
}

inline JBoolean
JDirEntry::IsWorkingLink()
	const
{
	return JConvertToBoolean( itsType == kFileLink || itsType == kDirLink ||
							  itsType == kUnknownLink );
}

inline JBoolean
JDirEntry::IsBrokenLink()
	const
{
	return JConvertToBoolean( itsType == kBrokenLink );
}

inline JBoolean
JDirEntry::IsUnknown()
	const
{
	return JConvertToBoolean( itsType == kUnknown || itsType == kUnknownLink );
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

inline JBoolean
JDirEntry::GetLinkName
	(
	const JString** linkName
	)
	const
{
	*linkName = itsLinkName;
	return JI2B( itsLinkName != nullptr );
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

inline JBoolean
JDirEntry::IsReadable()
	const
{
	return itsIsReadableFlag;
}

inline JBoolean
JDirEntry::IsWritable()
	const
{
	return itsIsWritableFlag;
}

inline JBoolean
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
