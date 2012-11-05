/******************************************************************************
 SVNRepoTreeNode.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNRepoTreeNode
#define _H_SVNRepoTreeNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JNamedTreeNode.h"
#include <JError.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes

class JProcess;
class SVNRepoTree;

class SVNRepoTreeNode : public JNamedTreeNode
{
public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	RecordLink;

public:

	enum Type
	{
		kFile,
		kDirectory,
		kError,
		kBusy
	};

public:

	SVNRepoTreeNode(JTree* tree, const JCharacter* repoPath,
					const JCharacter* repoRevision,
					const JCharacter* name, const Type type,
					const JIndex revision, const time_t modTime,
					const JCharacter* author, const JSize size);

	virtual ~SVNRepoTreeNode();

	const JString&	GetRepoPath() const;
	JBoolean		GetRepoRevision(JString* rev) const;
	Type			GetType() const;
	JIndex			GetRevision() const;
	time_t			GetModTime() const;
	JString			GetAgeString() const;
	const JString&	GetAuthor() const;
	JSize			GetFileSize() const;

	void	Update();
	JError	Rename(const JCharacter* newName, const JBoolean sort = kJTrue);

	SVNRepoTree*			GetRepoTree();
	const SVNRepoTree*		GetRepoTree() const;

	SVNRepoTreeNode*		GetRepoParent();
	const SVNRepoTreeNode*	GetRepoParent() const;
	JBoolean				GetRepoParent(SVNRepoTreeNode** parent);
	JBoolean				GetRepoParent(const SVNRepoTreeNode** parent) const;

	SVNRepoTreeNode*		GetRepoChild(const JIndex index);
	const SVNRepoTreeNode*	GetRepoChild(const JIndex index) const;

protected:

	virtual JBoolean	OKToOpen() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JString		itsRepoPath;
	JString		itsRepoRevision;
	JBoolean	itsNeedUpdateFlag;

	Type		itsType;
	JIndex		itsRevision;
	time_t		itsModTime;
	JString		itsAuthor;
	JSize		itsFileSize;			// only relevant for files

	JProcess*			itsProcess;		// can be NULL
	JString				itsResponseFullName;
	RecordLink*			itsErrorLink;	// can be NULL
	JPtrArray<JString>*	itsErrorList;	// cache while process is running

private:

	void	SetConnection(const int errFD);
	void	DeleteLinks();
	void	ReceiveErrorLine();
	void	ParseResponse();
	void	DisplayErrors();

	// not allowed

	SVNRepoTreeNode(const SVNRepoTreeNode& source);
	const SVNRepoTreeNode& operator=(const SVNRepoTreeNode& source);
};


/******************************************************************************
 GetRepoPath

 ******************************************************************************/

inline const JString&
SVNRepoTreeNode::GetRepoPath()
	const
{
	return itsRepoPath;
}

/******************************************************************************
 GetRepoRevision

 ******************************************************************************/

inline JBoolean
SVNRepoTreeNode::GetRepoRevision
	(
	JString* rev
	)
	const
{
	*rev = itsRepoRevision;
	return JNegate( itsRepoRevision.IsEmpty() );
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline SVNRepoTreeNode::Type
SVNRepoTreeNode::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetRevision

 ******************************************************************************/

inline JIndex
SVNRepoTreeNode::GetRevision()
	const
{
	return itsRevision;
}

/******************************************************************************
 GetModTime

 ******************************************************************************/

inline time_t
SVNRepoTreeNode::GetModTime()
	const
{
	return itsModTime;
}

/******************************************************************************
 GetAuthor

 ******************************************************************************/

inline const JString&
SVNRepoTreeNode::GetAuthor()
	const
{
	return itsAuthor;
}

/******************************************************************************
 GetFileSize

 ******************************************************************************/

inline JSize
SVNRepoTreeNode::GetFileSize()
	const
{
	return itsFileSize;
}

#endif
