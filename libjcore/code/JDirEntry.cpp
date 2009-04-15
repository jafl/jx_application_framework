/******************************************************************************
 JDirEntry.cpp

	Class representing an item in a directory.

	This class was not designed to be a base class.

	BASE CLASS = none

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JDirEntry.h>
#include <jDirUtil.h>
#include <jSysUtil.h>
#include <JPtrArray-JString.h>
#include <JOrderedSetUtil.h>
#include <JRegex.h>
#include <unistd.h>
#include <utime.h>
#include <jFStreamUtil.h>
#include <jMissingProto.h>
#include <ace/OS.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JDirEntry::JDirEntry
	(
	const JCharacter* fullName
	)
{
	JDirEntryX(fullName);
}

JDirEntry::JDirEntry
	(
	const JCharacter* pathName,
	const JCharacter* fileName
	)
{
	const JString fullName = JCombinePathAndName(pathName, fileName);
	JDirEntryX(fullName);
}

// private

void
JDirEntry::JDirEntryX
	(
	const JCharacter* origFullName
	)
{
	// parse path and name

	JString fullName;
	if (!JConvertToAbsolutePath(origFullName, NULL, &fullName))
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

	itsLinkName  = NULL;
	itsUserName  = NULL;
	itsGroupName = NULL;

	ForceUpdate();
}

/******************************************************************************
 Constructors for search targets (private)

 ******************************************************************************/

JDirEntry::JDirEntry
	(
	const JCharacter*	fileName,
	int					x
	)
	:
	itsName(fileName),
	itsLinkName(NULL),
	itsUserName(NULL),
	itsGroupName(NULL)
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
	itsLinkName(NULL),
	itsUserName(NULL),
	itsGroupName(NULL)
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

	if (source.itsLinkName != NULL)
		{
		itsLinkName = new JString(*(source.itsLinkName));
		assert( itsLinkName != NULL );
		}
}

/*****************************************************************************
 Destructor

 ******************************************************************************/

JDirEntry::~JDirEntry()
{
	delete itsLinkName;
	delete itsUserName;
	delete itsGroupName;
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

	if (source.itsLinkName != NULL && itsLinkName != NULL)
		{
		*itsLinkName = *(source.itsLinkName);
		}
	else if (source.itsLinkName != NULL)
		{
		itsLinkName = new JString(*(source.itsLinkName));
		assert( itsLinkName != NULL );
		}
	else
		{
		delete itsLinkName;
		itsLinkName = NULL;
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

	if (itsUserName != NULL && source.itsUserName != NULL)
		{
		*itsUserName = *(source.itsUserName);
		}
	else if (itsUserName != NULL)
		{
		delete itsUserName;
		itsUserName = NULL;
		}

	if (itsGroupName != NULL && source.itsGroupName != NULL)
		{
		*itsGroupName = *(source.itsGroupName);
		}
	else if (itsGroupName != NULL)
		{
		delete itsGroupName;
		itsGroupName = NULL;
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
	if (itsLinkName != NULL && !itsLinkName->IsEmpty())
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
	if (itsUserName == NULL)
		{
		const_cast<JDirEntry*>(this)->itsUserName = new JString(JGetUserName(itsUserID));
		assert( itsUserName != NULL );
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
	if (itsGroupName == NULL)
		{
		const_cast<JDirEntry*>(this)->itsGroupName = new JString(JGetGroupName(itsGroupID));
		assert( itsGroupName != NULL );
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
	const JBoolean	allow
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

	Returns kJTrue if the entry needs to be updated.

 ******************************************************************************/

JBoolean
JDirEntry::NeedsUpdate()
	const
{
	ACE_stat linfo, info;
	if (ACE_OS::lstat(itsFullName, &linfo) == 0 &&
		ACE_OS::stat(itsFullName, &info) == 0)
		{
		const_cast<JDirEntry*>(this)->itsAccessTime = linfo.st_atime;

		return JI2B(itsModTime     != (time_t) linfo.st_mtime ||
					itsStatusTime  != (time_t) linfo.st_ctime ||
					itsSModTime    != (time_t)  info.st_mtime ||
					itsSStatusTime != (time_t)  info.st_ctime);
		}
	else
		{
		return JNegate(itsType == kDoesNotExist);
		}
}

/******************************************************************************
 Update

	If necessary, updates the entry.  Returns kJTrue if the entry needed to
	be updated.

 ******************************************************************************/

JBoolean
JDirEntry::Update
	(
	const JBoolean force
	)
{
	if (force || NeedsUpdate())
		{
		ForceUpdate();
		return kJTrue;
		}
	else
		{
		return kJFalse;
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

	itsIsReadableFlag = itsIsWritableFlag = itsIsExecutableFlag = kJFalse;

	delete itsLinkName;
	itsLinkName = NULL;

	delete itsUserName;
	itsUserName = NULL;

	delete itsGroupName;
	itsGroupName = NULL;

	// get info from system

	itsFullName = JCombinePathAndName(itsPath, itsName);

	ACE_stat lstbuf;
	if (ACE_OS::lstat(itsFullName, &lstbuf) != 0)
		{
		return;
		}

	ACE_stat stbuf;
	const int statErr = ACE_OS::stat(itsFullName, &stbuf);

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

		itsLinkName = new JString;
		assert( itsLinkName != NULL );
		if (!(JGetSymbolicLinkTarget(itsFullName, itsLinkName)).OK())
			{
			delete itsLinkName;
			itsLinkName = NULL;
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
		itsIsReadableFlag   = kJTrue;
		itsIsWritableFlag   = kJTrue;
		itsIsExecutableFlag = JI2B( (stbuf.st_mode & S_IXUSR) != 0 );
		}
	else
		{
		itsIsReadableFlag   = JI2B( access(itsFullName, R_OK) == 0 );
		itsIsWritableFlag   = JI2B( access(itsFullName, W_OK) == 0 );
		itsIsExecutableFlag = JI2B( access(itsFullName, X_OK) == 0 );
		}
}

/******************************************************************************
 MatchesContentFilter

	This returns kJTrue for any file that matches the regex.

 ******************************************************************************/

JBoolean
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
		if (ACE_OS::lstat(itsFullName, &lstbuf) != 0)
			{
			return kJFalse;
			}

		const int fd = open(itsFullName, O_RDONLY);
		if (fd == -1)
			{
			return kJFalse;
			}

		JCharacter* data = new JCharacter [ kBlockSize ];
		const ssize_t count = read(fd, data, kBlockSize);
		close(fd);
		if (count < 0)
			{
			return kJFalse;
			}

		utimbuf ubuf;
		ubuf.actime  = lstbuf.st_atime;
		ubuf.modtime = lstbuf.st_mtime;
		utime(itsFullName, &ubuf);		// restore access time

		const JBoolean match = regex.MatchWithin(data, JIndexRange(1, count));

		delete [] data;
		return match;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Comparison (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JDirEntry::CompareNames
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
}

JOrderedSetT::CompareResult
JDirEntry::CompareSizes
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	const JOrderedSetT::CompareResult result = JCompareSizes(e1->itsSize, e2->itsSize);
	if (result != JOrderedSetT::kFirstEqualSecond)
		{
		return result;
		}
	else
		{
		return CompareNames(e1, e2);
		}
}

JOrderedSetT::CompareResult
JDirEntry::CompareModTimes
	(
	JDirEntry * const & e1,
	JDirEntry * const & e2
	)
{
	const JOrderedSetT::CompareResult result = JCompareSizes(e1->itsModTime, e2->itsModTime);
	if (result != JOrderedSetT::kFirstEqualSecond)
		{
		return result;
		}
	else
		{
		return CompareNames(e1, e2);
		}
}
