/******************************************************************************
 SVNRepoTreeNode.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNRepoTreeNode
#define _H_SVNRepoTreeNode

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

	SVNRepoTreeNode(JTree* tree, const JString& repoPath,
					const JString& repoRevision,
					const JString& name, const Type type,
					const JIndex revision, const time_t modTime,
					const JString& author, const JSize size);

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
	JError	Rename(const JString& newName, const JBoolean sort = kJTrue);

	SVNRepoTree*			GetRepoTree();
	const SVNRepoTree*		GetRepoTree() const;

	SVNRepoTreeNode*		GetRepoParent();
	const SVNRepoTreeNode*	GetRepoParent() const;
	JBoolean				GetRepoParent(SVNRepoTreeNode** parent);
	JBoolean				GetRepoParent(const SVNRepoTreeNode** parent) const;

	SVNRepoTreeNode*		GetRepoChild(const JIndex index);
	const SVNRepoTreeNode*	GetRepoChild(const JIndex index) const;

protected:

	virtual JBoolean	OKToOpen() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JString		itsRepoPath;
	JString		itsRepoRevision;
	JBoolean	itsNeedUpdateFlag;

	Type		itsType;
	JIndex		itsRevision;
	time_t		itsModTime;
	JString		itsAuthor;
	JSize		itsFileSize;			// only relevant for files

	JProcess*			itsProcess;		// can be nullptr
	JString				itsResponseFullName;
	RecordLink*			itsErrorLink;	// can be nullptr
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
