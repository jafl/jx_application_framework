/******************************************************************************
 SVNRepoTreeNode.cpp

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#include "SVNRepoTreeNode.h"
#include "SVNRepoTree.h"
#include <jXGlobals.h>
#include <JTree.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jXMLUtil.h>
#include <jTime.h>
#include <jMath.h>
#include <libxml/parser.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

******************************************************************************/

SVNRepoTreeNode::SVNRepoTreeNode
	(
	JTree*			tree,
	const JString&	repoPath,
	const JString&	repoRevision,
	const JString&	name,
	const Type		type,
	const JIndex	revision,
	const time_t	modTime,
	const JString&	author,
	const JSize		size
	)
	:
	JNamedTreeNode(tree, name, type == kDirectory),
	itsRepoPath(repoPath),
	itsRepoRevision(repoRevision),
	itsNeedUpdateFlag(true),
	itsType(type),
	itsRevision(revision),
	itsModTime(modTime),
	itsAuthor(author),
	itsFileSize(size),
	itsProcess(nullptr),
	itsErrorLink(nullptr)
{
	itsErrorList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsErrorList != nullptr );
}

/******************************************************************************
 Destructor

******************************************************************************/

SVNRepoTreeNode::~SVNRepoTreeNode()
{
	DeleteLinks();

	if (itsProcess != nullptr)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	jdelete itsProcess;

	if (!itsResponseFullName.IsEmpty())
		{
		JRemoveFile(itsResponseFullName);
		}

	jdelete itsErrorList;
}

/******************************************************************************
 GetAgeString

 ******************************************************************************/

JString
SVNRepoTreeNode::GetAgeString()
	const
{
	return (itsModTime == 0 ?
			JString::empty :
			JPrintTimeInterval(JRound(difftime(time(nullptr), itsModTime))));
}

/******************************************************************************
 Rename

	If sort==false, it is the caller's responsibility to call
	(node->GetParent())->SortChildren().

 ******************************************************************************/

JError
SVNRepoTreeNode::Rename
	(
	const JString&	newName,
	const bool	sort
	)
{
	if (newName == GetName())
		{
		return JNoError();
		}

	JString path, name;
	JSplitPathAndName(itsRepoPath, &path, &name);
	const JString newRepoPath = JCombinePathAndName(path, newName);

	JString cmd("svn move ");
	cmd += itsRepoPath;
	cmd += " ";
	cmd += newRepoPath;

	JXGetApplication()->DisplayBusyCursor();

	JSimpleProcess* p;
	JSimpleProcess::Create(&p, cmd, false);
	p->WaitUntilFinished();
	if (p->SuccessfulFinish())
		{
		SetName(newName);
		itsRepoPath = newRepoPath;
		if (itsType == kDirectory)
			{
			itsNeedUpdateFlag = true;
			}

// newName may be invalid beyond this point if text is from input field

		// must be after UpdatePath() so all JDirEntries are correct,
		// because can invoke Update()

		if (sort)
			{
			GetParent()->SortChildren();		// this method maintains the selection
			}
		}

	jdelete p;
	return JNoError();
}

/******************************************************************************
 OKToOpen (virtual protected)

 ******************************************************************************/

bool
SVNRepoTreeNode::OKToOpen()
	const
{
	if (!JNamedTreeNode::OKToOpen())
		{
		return false;
		}

	auto* me = const_cast<SVNRepoTreeNode*>(this);
	if (itsNeedUpdateFlag)
		{
		me->itsNeedUpdateFlag = false;
		me->Update();
		}

	return true;
}

/******************************************************************************
 Update

 ******************************************************************************/

void
SVNRepoTreeNode::Update()
{
	if (itsProcess != nullptr)
		{
		JProcess* p = itsProcess;
		itsProcess  = nullptr;

		p->Kill();
		jdelete p;

		DeleteLinks();
		}

	if (itsType != kDirectory)
		{
		return;
		}

	JError err = JCreateTempFile(nullptr, nullptr, &itsResponseFullName);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	JString cmd("/bin/sh -c 'svn list");
	if (!itsRepoRevision.IsEmpty())
		{
		cmd += " -r ";
		cmd += itsRepoRevision;
		}
	cmd += " --xml ";
	cmd += JPrepArgForExec(itsRepoPath);
	cmd += " >> ";
	cmd += JPrepArgForExec(itsResponseFullName);
	cmd += "'";

	int errFD;
	err = JProcess::Create(&itsProcess, cmd,
						   kJIgnoreConnection, nullptr,
						   kJIgnoreConnection, nullptr,
						   kJCreatePipe, &errFD);
	if (err.OK())
		{
		itsProcess->ShouldDeleteWhenFinished();
		ListenTo(itsProcess);
		SetConnection(errFD);

		DeleteAllChildren();

		auto* node =
			jnew SVNRepoTreeNode(GetTree(), JString::empty, JString::empty,
								 JGetString("BusyLabel::SVNRepoTreeNode"),
								 kBusy, 0, 0, JString::empty, 0);
		assert( node != nullptr );
		this->Append(node);
		}
	else
		{
		err.ReportIfError();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNRepoTreeNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsErrorLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		ReceiveErrorLine();
		}
	else
		{
		JNamedTreeNode::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
SVNRepoTreeNode::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsProcess)
		{
		itsProcess = nullptr;
		DeleteLinks();

		ParseResponse();
		}
	else
		{
		JNamedTreeNode::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 ReceiveErrorLine (private)

 ******************************************************************************/

void
SVNRepoTreeNode::ReceiveErrorLine()
{
	assert( itsErrorLink != nullptr );

	JString line;
	const bool ok = itsErrorLink->GetNextMessage(&line);
	assert( ok );

	itsErrorList->Append(line);
}

/******************************************************************************
 ParseResponse (private)

 ******************************************************************************/

void
SVNRepoTreeNode::ParseResponse()
{
	DeleteAllChildren();

	xmlDoc* doc = xmlReadFile(itsResponseFullName.GetBytes(), nullptr, XML_PARSE_NOBLANKS | XML_PARSE_NOCDATA);
	if (doc != nullptr)
		{
		xmlNode* root = xmlDocGetRootElement(doc);

		if (root != nullptr &&
			strcmp((char*) root->name, "lists") == 0 &&
			strcmp((char*) root->children->name, "list") == 0)
			{
			const JString repoPath = JGetXMLNodeAttr(root->children, "path");

			xmlNode* entry = root->children->children;
			JString repoPath1;
			while (entry != nullptr)
				{
				const JString type = JGetXMLNodeAttr(entry, "kind");

				JString name, author;
				JIndex rev     = 0;
				time_t modTime = 0;
				JSize size     = 0;

				xmlNode* child = JGetXMLChildNode(entry, "name");
				if (child != nullptr && child->children != nullptr &&
					child->children->type == XML_TEXT_NODE)
					{
					name = (char*) child->children->content;
					}

				child = JGetXMLChildNode(entry, "size");
				if (child != nullptr && child->children != nullptr &&
					child->children->type == XML_TEXT_NODE)
					{
					size = atol((char*) child->children->content);
					}

				child = JGetXMLChildNode(entry, "commit");
				if (child != nullptr)
					{
					rev = atol(JGetXMLNodeAttr(child, "revision").GetBytes());

					xmlNode* child2 = JGetXMLChildNode(child, "author");
					if (child2 != nullptr && child2->children != nullptr &&
						child2->children->type == XML_TEXT_NODE)
						{
						author = (char*) child2->children->content;
						}

					child2 = JGetXMLChildNode(child, "date");
					if (child2 != nullptr && child2->children != nullptr &&
						child2->children->type == XML_TEXT_NODE)
						{
						tm tm;
						char* endPtr = strptime((char*) child2->children->content,
												"%Y-%m-%dT%H:%M:%S", &tm);
						if (*endPtr == '.')
							{
							modTime = mktime(&tm) + JGetTimezoneOffset();
							}
						}
					}

				if (!name.IsEmpty())
					{
					repoPath1 = JCombinePathAndName(repoPath, name);

					auto* node =
						jnew SVNRepoTreeNode(GetTree(), repoPath1, itsRepoRevision,
											name, type == "dir" ? kDirectory : kFile,
											rev, modTime, author, size);
					assert( node != nullptr );
					this->InsertSorted(node);
					}

				entry = entry->next;
				}

			SVNRepoTree* tree = GetRepoTree();
			const JSize count = GetChildCount();
			for (JIndex i=1; i<=count; i++)
				{
				tree->ReopenIfNeeded(GetRepoChild(i));
				}
			}

		xmlFreeDoc(doc);
		}

	JRemoveFile(itsResponseFullName);
	itsResponseFullName.Clear();

	DisplayErrors();
}

/******************************************************************************
 DisplayErrors (private)

 ******************************************************************************/

void
SVNRepoTreeNode::DisplayErrors()
{
	const JSize count = itsErrorList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* node =
			jnew SVNRepoTreeNode(GetTree(), JString::empty, JString::empty,
								 *(itsErrorList->GetElement(i)),
								 kError, 0, 0, JString::empty, 0);
		assert( node != nullptr );
		this->InsertAtIndex(i, node);
		}

	itsErrorList->DeleteAll();
}

/******************************************************************************
 GetRepoTree

 ******************************************************************************/

SVNRepoTree*
SVNRepoTreeNode::GetRepoTree()
{
	auto* tree = dynamic_cast<SVNRepoTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const SVNRepoTree*
SVNRepoTreeNode::GetRepoTree()
	const
{
	const auto* tree = dynamic_cast<const SVNRepoTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

/******************************************************************************
 GetRepoParent

 ******************************************************************************/

SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoParent()
{
	JTreeNode* p       = GetParent();
	auto* n = dynamic_cast<SVNRepoTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

const SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoParent()
	const
{
	const JTreeNode* p       = GetParent();
	const auto* n = dynamic_cast<const SVNRepoTreeNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
SVNRepoTreeNode::GetRepoParent
	(
	SVNRepoTreeNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<SVNRepoTreeNode*>(p);
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

bool
SVNRepoTreeNode::GetRepoParent
	(
	const SVNRepoTreeNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const SVNRepoTreeNode*>(p);
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

/******************************************************************************
 GetRepoChild

 ******************************************************************************/

SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<SVNRepoTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const SVNRepoTreeNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

void
SVNRepoTreeNode::SetConnection
	(
	const int errFD
	)
{
	itsErrorLink = new RecordLink(errFD);
	assert( itsErrorLink != nullptr );
	ListenTo(itsErrorLink);
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

void
SVNRepoTreeNode::DeleteLinks()
{
	StopListening(itsErrorLink);
	delete itsErrorLink;
	itsErrorLink = nullptr;
}
