/******************************************************************************
 jMountUtil.h

	Copyright (C) 2000-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_jMountUtil
#define _H_jMountUtil

#include <JPtrArray-JString.h>
#include <JError.h>

class JProcess;

// Do not change the values once they are allocated, because they may be
// stored in files.

enum JMountType
{
	kJHardDisk = 0,
	kJFloppyDisk,
	kJCDROM,
	kJObsolete1,
	kJUnknownMountType
};

enum JFileSystemType
{
	kOtherFSType = 0,
	kVFATType
};

struct JMountPoint
{
	JString*		path;
	JMountType		type;
	dev_t			device;
	JString*		devicePath;
	JFileSystemType	fsType;

	JMountPoint()
		:
		path(nullptr), type(kJHardDisk), device(0), devicePath(nullptr), fsType(kOtherFSType)
	{ };

	JMountPoint(JString* p, const JMountType t, const dev_t d, JString* dp, const JFileSystemType fst)
		:
		path(p), type(t), device(d), devicePath(dp), fsType(fst)
	{ };
};

union JMountState
{
	time_t		modTime;
	JString*	mountCmdOutput;

	JMountState()
		:
		mountCmdOutput(nullptr)	// can only init one member, and ptr is critical
	{ };

	~JMountState();
};

class JMountPointList : public JArray<JMountPoint>
{
public:

	JMountPointList(const JPtrArrayT::CleanUpAction action);

	virtual ~JMountPointList();

	void	CleanOut();		// safest
	void	DeleteAll();

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

private:

	JPtrArrayT::CleanUpAction	itsCleanUpAction;
};

JBoolean	JGetUserMountPointList(JMountPointList* list, JMountState* state);
JMountType	JGetUserMountPointType(const JString& path,
								   const JString& device, const JString& fsType);
JBoolean	JIsMounted(const JString& path,
					   JBoolean* writable = nullptr, JBoolean* isTop = nullptr,
					   JString* device = nullptr,
					   JFileSystemType* fsType = nullptr, JString* fsTypeString = nullptr);
JBoolean	JFindUserMountPoint(const JString& path, const JMountPointList& list,
								JIndex* index);

void		JMount(const JString& path, const JBoolean mount = kJTrue,
				   const JBoolean block = kJFalse);

JBoolean	JTranslateLocalToRemote(const JString& localPathStr,
									JString* host, JString* remotePath);
JBoolean	JTranslateRemoteToLocal(const JString& hostStr,
									const JString& remotePathStr,
									JString* localPath);

JError		JFormatPartition(const JString& path, const JString& type,
							 JProcess** process);
JBoolean	JIsSamePartition(const JString& path1, const JString& path2);

inline JBoolean
JMountSupportsExecFlag
	(
	const JFileSystemType type
	)
{
	return JI2B(type != kVFATType);
}

#endif
