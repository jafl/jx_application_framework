/******************************************************************************
 JDirEntry.cpp

	Class representing an item in a directory.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/jcore/JDirEntry.h"
#include "jx-af/jcore/jDirUtil.h"
#include "jx-af/jcore/jSysUtil.h"
#include "jx-af/jcore/JPtrArray-JString.h"
#include "jx-af/jcore/JListUtil.h"
#include "jx-af/jcore/JRegex.h"
#include <unistd.h>
#include <utime.h>
#include "jx-af/jcore/jFStreamUtil.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JDirEntry::JDirEntry
	(
	const JString& fullName
	)
{
	JDirEntryX(fullName);
}

JDirEntry::JDirEntry
	(
	const JString& pathName,
	const JString& fileName
	)
{
	const JString fullName = JCombinePathAndName(pathName, fileName);
	JDirEntryX(fullName);
}

// private

void
JDirEntry::JDirEntryX
	(
	const JString& origFullName
	)
{
	// parse path and name

	JString fullName;
	if (!JConvertToAbsolutePath(origFullName, JString::empty, &fullName))
	{
		fullName = origFullName;	// fail gracefully with type kDoesNotExist
	}
	JStripTrailingDirSeparator(&fullName);

	if (JIsRootDirectory(fullName))
	{
		itsPath = itsName = fullName;
	}
	else
	{
		JSplitPathAndName(fullName, &itsPath, &itsName);
	}

	// set rest of instance variables

	itsLinkName  = nullptr;
	itsUserName  = nullptr;
	itsGroupName = nullptr;

	ForceUpdate();
}

/******************************************************************************
 Constructors for search targets (private)

 ******************************************************************************/

JDirEntry::JDirEntry
	(
	const JString&	fileName,
	int				x
	)
	:
	itsName(fileName),
	itsLinkName(nullptr),
	itsUserName(nullptr),
	itsGroupName(nullptr)
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JDirEntry::JDirEntry
	(
	const JDirEntry& source
	)
	:
	itsPath(source.itsPath),
	itsName(source.itsName),
	itsFullName(source.itsFullName),
	itsLinkName(nullptr),
	itsUserName(nullptr),
	itsGroupName(nullptr)
{
	itsType        = source.itsType;
	itsSize        = source.itsSize;
	itsMode        = source.itsMode;
	itsModTime     = source.itsModTime;
	itsStatusTime  = source.itsStatusTime;
	itsAccessTime  = source.itsAccessTime;
	itsSModTime    = source.itsSModTime;
	itsSStatusTime = source.itsSStatusTime;
	itsUserID      = source.itsUserID;
	itsGroupID     = source.itsGroupID;

	itsIsReadableFlag   = source.itsIsReadableFlag;
	itsIsWritableFlag   = source.itsIsWritableFlag;
	itsIsExecutableFlag = source.itsIsExecutableFlag;

	if (source.itsLinkName != nullptr)
	{
		itsLinkName = jnew JString(*(source.itsLinkName));
		assert( itsLinkName != nullptr );
	}
}

/*****************************************************************************
 Destructor

 ******************************************************************************/

JDirEntry::~JDirEntry()
{
	jdelete itsLinkName;
	jdelete itsUserName;
	jdelete itsGroupName;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JDirEntry&
JDirEntry::operator=
	(
	const JDirEntry& source
	)
{
	if (this == &source)
	{
		return *this;
	}

	itsPath     = source.itsPath;
	itsName     = source.itsName;
	itsFullName = source.itsFullName;

	if (source.itsLinkName != nullptr && itsLinkName != nullptr)
	{
		*itsLinkName = *(source.itsLinkName);
	}
	else if (source.itsLinkName != nullptr)
	{
		itsLinkName = jnew JString(*(source.itsLinkName));
		assert( itsLinkName != nullptr );
	}
	else
	{
		jdelete itsLinkName;
		itsLinkName = nullptr;
	}

	itsType        = source.itsType;
	itsSize        = source.itsSize;
	itsMode        = source.itsMode;
	itsModTime     = source.itsModTime;
	itsStatusTime  = source.itsStatusTime;
	itsAccessTime  = source.itsAccessTime;
	itsSModTime    = source.itsSModTime;
	itsSStatusTime = source.itsSStatusTime;
	itsUserID      = source.itsUserID;
	itsGroupID     = source.itsGroupID;

	if (itsUserName != nullptr && source.itsUserName != nullptr)
	{
		*itsUserName = *(source.itsUserName);
	}
	else if (itsUserName != nullptr)
	{
		jdelete itsUserName;
		itsUserName = nullptr;
	}

	if (itsGroupName != nullptr && source.itsGroupName != nullptr)
	{
		*itsGroupName = *(source.itsGroupName);
	}
	else if (itsGroupName != nullptr)
	{
		jdelete itsGroupName;
		itsGroupName = nullptr;
	}

	itsIsReadableFlag   = source.itsIsReadableFlag;
	itsIsWritableFlag   = source.itsIsWritableFlag;
	itsIsExecutableFlag = source.itsIsExecutableFlag;

	return *this;
}

/******************************************************************************
 FollowLink

	Returns a new object that describes what we point to.  If we are not
	a link, we return a copy of ourselves.

 ******************************************************************************/

JDirEntry
JDirEntry::FollowLink()
	const
{
	if (itsLinkName != nullptr && !itsLinkName->IsEmpty())
	{
		return JDirEntry(itsPath, *itsLinkName);
	}
	else
	{
		return *this;
	}
}

/******************************************************************************
 GetUserName

 ******************************************************************************/

const JString&
JDirEntry::GetUserName()
	const
{
	if (itsUserName == nullptr)
	{
		const_cast<JDirEntry*>(this)->itsUserName = jnew JString(JGetUserName(itsUserID));
		assert( itsUserName != nullptr );
	}

	return *itsUserName;
}

/******************************************************************************
 GetGroupName

 ******************************************************************************/

const JString&
JDirEntry::GetGroupName()
	const
{
	if (itsGroupName == nullptr)
	{
		const_cast<JDirEntry*>(this)->itsGroupName = jnew JString(JGetGroupName(itsGroupID));
		assert( itsGroupName != nullptr );
	}

	return *itsGroupName;
}

/*****************************************************************************
 SetMode

 ******************************************************************************/

JError
JDirEntry::SetMode
	(
	const mode_t mode
	)
{
	return JSetPermissions(itsFullName, mode);
}

JError
JDirEntry::SetMode
	(
	const ModeBit	bit,
	const bool	allow
	)
{
	mode_t mode;
	JError err = JGetPermissions(itsFullName, &mode);
	if (!err.OK())
	{
		return err;
	}

	if (allow)
	{
		mode |= (1 << bit);
	}
	else
	{
		mode &= ~(1 << bit);
	}

	return JSetPermissions(itsFullName, mode);
}

/*****************************************************************************
 GetModeString

 ******************************************************************************/

JString
JDirEntry::GetModeString()
	const
{
	return JGetPermissionsString(itsMode);
}

/******************************************************************************
 NeedsUpdate

	Returns true if the entry needs to be updated.

 ******************************************************************************/

bool
JDirEntry::NeedsUpdate()
	const
{
	ACE_stat linfo, info;
	const int lstatErr = ACE_OS::lstat(itsFullName.GetBytes(), &linfo);
	const int statErr  = ACE_OS::stat(itsFullName.GetBytes(), &info);
	if (lstatErr == 0 && statErr == 0)
	{
		const_cast<JDirEntry*>(this)->itsAccessTime = linfo.st_atime;

		return itsModTime     != (time_t) linfo.st_mtime ||
					itsStatusTime  != (time_t) linfo.st_ctime ||
					itsSModTime    != (time_t)  info.st_mtime ||
					itsSStatusTime != (time_t)  info.st_ctime;
	}
	else if (lstatErr == 0 && statErr == -1)
	{
		return itsType != kBrokenLink;
	}
	else
	{
		return itsType != kDoesNotExist;
	}
}

/******************************************************************************
 Update

	If necessary, updates the entry.  Returns true if the entry needed to
	be updated.

 ******************************************************************************/

bool
JDirEntry::Update
	(
	const bool force
	)
{
	if (force || NeedsUpdate())
	{
		ForceUpdate();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ForceUpdate

	Updates the entry, regardless of whether or not it needs it.

 ******************************************************************************/

void
JDirEntry::ForceUpdate()
{
	// clear everything

	itsType = kDoesNotExist;

	itsSize        = 0;
	itsMode        = 0;
	itsModTime     = 0;
	itsStatusTime  = 0;
	itsAccessTime  = 0;
	itsSModTime    = 0;
	itsSStatusTime = 0;
	itsUserID      = 0;
	itsGroupID     = 0;

	itsIsReadableFlag = itsIsWritableFlag = itsIsExecutableFlag = false;

	jdelete itsLinkName;
	itsLinkName = nullptr;

	jdelete itsUserName;
	itsUserName = nullptr;

	jdelete itsGroupName;
	itsGroupName = nullptr;

	// get info from system

	itsFullName = JCombinePathAndName(itsPath, itsName);

	ACE_stat lstbuf;
	if (ACE_OS::lstat(itsFullName.GetBytes(), &lstbuf) != 0)
	{
		return;
	}

	ACE_stat stbuf;
	const int statErr = ACE_OS::stat(itsFullName.GetBytes(), &stbuf);

	// simple information

	itsSize        = lstbuf.st_size;	
	itsMode        = lstbuf.st_mode;
	itsModTime     = lstbuf.st_mtime;
	itsStatusTime  = lstbuf.st_ctime;
	itsAccessTime  = lstbuf.st_atime;
	itsSModTime    =  stbuf.st_mtime;
	itsSStatusTime =  stbuf.st_ctime;
	itsUserID      = lstbuf.st_uid;
	itsGroupID     = lstbuf.st_gid;

	// file type

	const mode_t ltype = lstbuf.st_mode;
	const mode_t ftype = stbuf.st_mode;

	if (S_ISLNK(ltype))
	{			
		if (statErr == -1)
		{
			itsType = kBrokenLink;
		}
		else if (S_ISREG(ftype))
		{
			itsType = kFileLink;
		}
		else if (S_ISDIR(ftype))
		{
			itsType = kDirLink;
		}
		else
		{
			itsType = kUnknownLink;
		}

		itsLinkName = jnew JString;
		assert( itsLinkName != nullptr );
		if (!(JGetSymbolicLinkTarget(itsFullName, itsLinkName)).OK())
		{
			jdelete itsLinkName;
			itsLinkName = nullptr;
		}
	}
	else if (S_ISREG(ftype))
	{
		itsType = kFile;
	}
	else if (S_ISDIR(ftype))
	{
		itsType = kDir;
	}
	else
	{
		itsType = kUnknown;
	}

	// permissions

	if (JUserIsAdmin())
	{
		itsIsReadableFlag   = true;
		itsIsWritableFlag   = true;
		itsIsExecutableFlag = (stbuf.st_mode & S_IXUSR) != 0;
	}
	else
	{
		itsIsReadableFlag   = access(itsFullName.GetBytes(), R_OK) == 0;
		itsIsWritableFlag   = access(itsFullName.GetBytes(), W_OK) == 0;
		itsIsExecutableFlag = access(itsFullName.GetBytes(), X_OK) == 0;
	}
}

/******************************************************************************
 MatchesContentFilter

	This returns true for any file that matches the regex.

 ******************************************************************************/

bool
JDirEntry::MatchesContentFilter
	(
	const JRegex&	regex,
	const JSize		kBlockSize
	)
	const
{
	if (IsFile())
	{
		ACE_stat lstbuf;
		if (ACE_OS::lstat(itsFullName.GetBytes(), &lstbuf) != 0)
		{
			return false;
		}

		const int fd = open(itsFullName.GetBytes(), O_RDONLY);
		if (fd == -1)
		{
			return false;
		}

		auto* data = jnew JUtf8Byte [ kBlockSize+1 ];
		const ssize_t count = read(fd, data, kBlockSize);
		close(fd);
		if (count < 0)
		{
			return false;
		}
		data[ count ] = '\0';

		utimbuf ubuf;
		ubuf.actime  = lstbuf.st_atime;
		ubuf.modtime = lstbuf.st_mtime;
		utime(itsFullName.GetBytes(), &ubuf);		// restore access time

		const bool match = regex.Match(JString(data, count, JString::kNoCopy));

		jdelete [] data;
		return match;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Comparison (static)

 ******************************************************************************/

JListT::CompareResult
JDirEntry::CompareNames
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
}

JListT::CompareResult
JDirEntry::CompareSizes
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	const JListT::CompareResult result = JCompareSizes(e1->itsSize, e2->itsSize);
	if (result != JListT::kFirstEqualSecond)
	{
		return result;
	}
	else
	{
		return CompareNames(e1, e2);
	}
}

JListT::CompareResult
JDirEntry::CompareModTimes
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	const JListT::CompareResult result = JCompareSizes(e1->itsModTime, e2->itsModTime);
	if (result != JListT::kFirstEqualSecond)
	{
		return result;
	}
	else
	{
		return CompareNames(e1, e2);
	}
}
