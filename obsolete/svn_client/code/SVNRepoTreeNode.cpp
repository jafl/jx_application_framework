/******************************************************************************
 SVNRepoTreeNode.cpp

	Copyright (C) 2008 by John Lindal. All rights reserved.

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

// string ID's

static const JCharacter* kBusyLabel = "BusyLabel::SVNRepoTreeNode";

/******************************************************************************
 Constructor

******************************************************************************/

SVNRepoTreeNode::SVNRepoTreeNode
	(
	JTree*				tree,
	const JCharacter*	repoPath,
	const JCharacter*	repoRevision,
	const JCharacter*	name,
	const Type			type,
	const JIndex		revision,
	const time_t		modTime,
	const JCharacter*	author,
	const JSize			size
	)
	:
	JNamedTreeNode(tree, name, JI2B(type == kDirectory)),
	itsRepoPath(repoPath),
	itsRepoRevision(JStringEmpty(repoRevision) ? "" : repoRevision),
	itsNeedUpdateFlag(kJTrue),
	itsType(type),
	itsRevision(revision),
	itsModTime(modTime),
	itsAuthor(author),
	itsFileSize(size),
	itsProcess(NULL),
	itsErrorLink(NULL)
{
	itsErrorList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsErrorList != NULL );
}

/******************************************************************************
 Destructor

******************************************************************************/

SVNRepoTreeNode::~SVNRepoTreeNode()
{
	DeleteLinks();

	if (itsProcess != NULL)
		{
		StopListening(itsProcess);
		itsProcess->Kill();
		}
	delete itsProcess;

	if (!itsResponseFullName.IsEmpty())
		{
		JRemoveFile(itsResponseFullName);
		}

	delete itsErrorList;
}

/******************************************************************************
 GetAgeString

 ******************************************************************************/

JString
SVNRepoTreeNode::GetAgeString()
	const
{
	return (itsModTime == 0 ? "" : JPrintTimeInterval(JRound(difftime(time(NULL), itsModTime))));
}

/******************************************************************************
 Rename

	If sort==kJFalse, it is the caller's responsibility to call
	(node->GetParent())->SortChildren().

 ******************************************************************************/

JError
SVNRepoTreeNode::Rename
	(
	const JCharacter*	newName,
	const JBoolean		sort
	)
{
	if (newName == GetName())
		{
		return JNoError();
		}

	JString path, name;
	JSplitPathAndName(itsRepoPath, &path, &name);
	const JString newRepoPath = JCombinePathAndName(path, newName);

	JString cmd = "svn move ";
	cmd        += itsRepoPath;
	cmd        += " ";
	cmd        += newRepoPath;

	(JXGetApplication())->DisplayBusyCursor();

	JSimpleProcess* p;
	JSimpleProcess::Create(&p, cmd, kJFalse);
	p->WaitUntilFinished();
	if (p->SuccessfulFinish())
		{
		SetName(newName);
		itsRepoPath = newRepoPath;
		if (itsType == kDirectory)
			{
			itsNeedUpdateFlag = kJTrue;
			}

// newName may be invalid beyond this point if text is from input field

		// must be after UpdatePath() so all JDirEntries are correct,
		// because can invoke Update()

		if (sort)
			{
			GetParent()->SortChildren();		// this method maintains the selection
			}
		}

	delete p;
	return JNoError();
}

/******************************************************************************
 OKToOpen (virtual protected)

 ******************************************************************************/

JBoolean
SVNRepoTreeNode::OKToOpen()
	const
{
	if (!JNamedTreeNode::OKToOpen())
		{
		return kJFalse;
		}

	SVNRepoTreeNode* me = const_cast<SVNRepoTreeNode*>(this);
	if (itsNeedUpdateFlag)
		{
		me->itsNeedUpdateFlag = kJFalse;
		me->Update();
		}

	return kJTrue;
}

/******************************************************************************
 Update

 ******************************************************************************/

void
SVNRepoTreeNode::Update()
{
	if (itsProcess != NULL)
		{
		JProcess* p = itsProcess;
		itsProcess  = NULL;

		p->Kill();
		delete p;

		DeleteLinks();
		}

	if (itsType != kDirectory)
		{
		return;
		}

	JError err = JCreateTempFile(NULL, NULL, &itsResponseFullName);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	JString cmd = "/bin/sh -c 'svn list";
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

	int outFD, errFD;
	err = JProcess::Create(&itsProcess, cmd,
						   kJIgnoreConnection, NULL,
						   kJIgnoreConnection, NULL,
						   kJCreatePipe, &errFD);
	if (err.OK())
		{
		itsProcess->ShouldDeleteWhenFinished();
		ListenTo(itsProcess);
		SetConnection(errFD);

		DeleteAllChildren();

		SVNRepoTreeNode* node =
			new SVNRepoTreeNode(GetTree(), "", 0, JGetString(kBusyLabel),
								kBusy, 0, 0, "", 0);
		assert( node != NULL );
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
		itsProcess = NULL;
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
	assert( itsErrorLink != NULL );

	JString line;
	const JBoolean ok = itsErrorLink->GetNextMessage(&line);
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

	xmlDoc* doc = xmlReadFile(itsResponseFullName, NULL, XML_PARSE_NOBLANKS | XML_PARSE_NOCDATA);
	if (doc != NULL)
		{
		xmlNode* root = xmlDocGetRootElement(doc);

		if (root != NULL &&
			strcmp((char*) root->name, "lists") == 0 &&
			strcmp((char*) root->children->name, "list") == 0)
			{
			const JString repoPath = JGetXMLNodeAttr(root->children, "path");

			xmlNode* entry = root->children->children;
			JString repoPath1;
			while (entry != NULL)
				{
				const JString type = JGetXMLNodeAttr(entry, "kind");

				JString name, author;
				JIndex rev     = 0;
				time_t modTime = 0;
				JSize size     = 0;

				xmlNode* child = JGetXMLChildNode(entry, "name");
				if (child != NULL && child->children != NULL &&
					child->children->type == XML_TEXT_NODE)
					{
					name = (char*) child->children->content;
					}

				child = JGetXMLChildNode(entry, "size");
				if (child != NULL && child->children != NULL &&
					child->children->type == XML_TEXT_NODE)
					{
					size = atol((char*) child->children->content);
					}

				child = JGetXMLChildNode(entry, "commit");
				if (child != NULL)
					{
					rev = atol(JGetXMLNodeAttr(child, "revision"));

					xmlNode* child2 = JGetXMLChildNode(child, "author");
					if (child2 != NULL && child2->children != NULL &&
						child2->children->type == XML_TEXT_NODE)
						{
						author = (char*) child2->children->content;
						}

					child2 = JGetXMLChildNode(child, "date");
					if (child2 != NULL && child2->children != NULL &&
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

					SVNRepoTreeNode* node =
						new SVNRepoTreeNode(GetTree(), repoPath1, itsRepoRevision,
											name, type == "dir" ? kDirectory : kFile,
											rev, modTime, author, size);
					assert( node != NULL );
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
		SVNRepoTreeNode* node =
			new SVNRepoTreeNode(GetTree(), "", 0, *(itsErrorList->NthElement(i)),
								kError, 0, 0, "", 0);
		assert( node != NULL );
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
	SVNRepoTree* tree = dynamic_cast<SVNRepoTree*>(GetTree());
	assert (tree != NULL);
	return tree;
}

const SVNRepoTree*
SVNRepoTreeNode::GetRepoTree()
	const
{
	const SVNRepoTree* tree = dynamic_cast<const SVNRepoTree*>(GetTree());
	assert (tree != NULL);
	return tree;
}

/******************************************************************************
 GetRepoParent

 ******************************************************************************/

SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoParent()
{
	JTreeNode* p       = GetParent();
	SVNRepoTreeNode* n = dynamic_cast<SVNRepoTreeNode*>(p);
	assert( n != NULL );
	return n;
}

const SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoParent()
	const
{
	const JTreeNode* p       = GetParent();
	const SVNRepoTreeNode* n = dynamic_cast<const SVNRepoTreeNode*>(p);
	assert( n != NULL );
	return n;
}

JBoolean
SVNRepoTreeNode::GetRepoParent
	(
	SVNRepoTreeNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<SVNRepoTreeNode*>(p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
		}
}

JBoolean
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
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
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
	SVNRepoTreeNode* node = dynamic_cast<SVNRepoTreeNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}

const SVNRepoTreeNode*
SVNRepoTreeNode::GetRepoChild
	(
	const JIndex index
	)
	const
{
	const SVNRepoTreeNode* node = dynamic_cast<const SVNRepoTreeNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}

/******************************************************************************
 SetConnection (private)

 ******************************************************************************/

// This function has to be last so JCore::new works for everything else.

#undef new

void
SVNRepoTreeNode::SetConnection
	(
	const int errFD
	)
{
	itsErrorLink = new RecordLink(errFD);
	assert( itsErrorLink != NULL );
	ListenTo(itsErrorLink);
}

/******************************************************************************
 DeleteLinks (private)

 ******************************************************************************/

// This function has to be last so JCore::delete works for everything else.

#undef delete

void
SVNRepoTreeNode::DeleteLinks()
{
	StopListening(itsErrorLink);
	delete itsErrorLink;
	itsErrorLink = NULL;
}
