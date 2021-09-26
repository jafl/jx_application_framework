/******************************************************************************
 JFSFileTree.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFSFileTree
#define _H_JFSFileTree

#include <jx-af/jcore/JTree.h>

class JString;
class JFSFileTreeNodeBase;

class JFSFileTree : public JTree
{
public:

	JFSFileTree(JFSFileTreeNodeBase* root);

	virtual ~JFSFileTree();

	void	BroadcastDirectoryRenamed(const JString& oldPath, const JString& newPath);

	JFSFileTreeNodeBase*		GetFSRoot();
	const JFSFileTreeNodeBase*	GetFSRoot() const;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDirectoryRenamed;

	class DirectoryRenamed : public JBroadcaster::Message
		{
		public:

			DirectoryRenamed(const JString& oldPath, const JString& newPath)
				:
				JBroadcaster::Message(kDirectoryRenamed),
				itsOldPath(oldPath), itsNewPath(newPath)
			{ };

			const JString&
			GetOldPath() const
			{
				return itsOldPath;
			};

			const JString&
			GetNewPath() const
			{
				return itsNewPath;
			};

		private:

			const JString& itsOldPath;
			const JString& itsNewPath;
		};
};

#endif
