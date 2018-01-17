/******************************************************************************
 SVNRepoTree.cpp

	BASE CLASS = JTree

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNRepoTree.h"
#include "SVNRepoTreeNode.h"
#include "SVNRepoTreeList.h"
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNRepoTree::SVNRepoTree
	(
	SVNRepoTreeNode* root
	)
	:
	JTree(root),
	itsView(NULL)
{
	itsSavedOpenNodes = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSavedOpenNodes != NULL );
	itsSavedOpenNodes->SetCompareFunction(JCompareStringsCaseSensitive);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNRepoTree::~SVNRepoTree()
{
	jdelete itsSavedOpenNodes;
}

/******************************************************************************
 GetRepoPath

 ******************************************************************************/

const JString&
SVNRepoTree::GetRepoPath()
	const
{
	return GetRepoRoot()->GetRepoPath();
}

/******************************************************************************
 Update

 ******************************************************************************/

void
SVNRepoTree::Update
	(
	SVNRepoTreeList*	view,
	const JBoolean		fresh
	)
{
	if (fresh)
		{
		itsSavedOpenNodes->CleanOut();
		}

	itsView = view;
	SaveOpenNodes();

	GetRepoRoot()->Update();
}

/******************************************************************************
 SaveOpenNodes (private)

 ******************************************************************************/

void
SVNRepoTree::SaveOpenNodes()
	const
{
	if (itsView != NULL)
		{
		JPtrArray<JString> paths(JPtrArrayT::kDeleteAll);
		const JSize count = itsView->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (itsView->IsOpen(i))
				{
				JString* s = jnew JString((itsView->GetRepoNode(i))->GetRepoPath());
				assert( s != NULL );

				itsSavedOpenNodes->InsertSorted(s);
				}
			}
		}
}

/******************************************************************************
 SavePathToOpen

 ******************************************************************************/

void
SVNRepoTree::SavePathToOpen
	(
	const JCharacter* origUrl
	)
{
	JString url = origUrl;
	JStripTrailingDirSeparator(&url);

	JString baseUrl = GetRepoRoot()->GetRepoPath();
	JAppendDirSeparator(&baseUrl);
	if (!url.BeginsWith(baseUrl))
		{
		return;
		}
	JStripTrailingDirSeparator(&baseUrl);

	JString path, name;
	while (url.GetLength() > baseUrl.GetLength())
		{
		JString* s = jnew JString(url);
		assert( s != NULL );
		itsSavedOpenNodes->InsertSorted(s);

		JSplitPathAndName(url, &path, &name);
		url = path;
		JStripTrailingDirSeparator(&url);
		}
}

/******************************************************************************
 ReopenIfNeeded

 ******************************************************************************/

void
SVNRepoTree::ReopenIfNeeded
	(
	SVNRepoTreeNode* node
	)
{
	const JString& repoPath = node->GetRepoPath();
	JIndex i;
	if (itsView != NULL &&
		itsSavedOpenNodes->SearchSorted(const_cast<JString*>(&repoPath), JListT::kAnyMatch, &i))
		{
		itsView->Open(node);
		}
}

/******************************************************************************
 SkipSetup (static)

 ******************************************************************************/

void
SVNRepoTree::SkipSetup
	(
	std::istream&		input,
	JFileVersion	vers
	)
{
	if (vers >= 1)
		{
		JPtrArray<JString> savedOpenNodes(JPtrArrayT::kDeleteAll);
		input >> savedOpenNodes;
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SVNRepoTree::ReadSetup
	(
	std::istream&		input,
	JFileVersion	vers
	)
{
	if (vers >= 1)
		{
		input >> *itsSavedOpenNodes;
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
SVNRepoTree::WriteSetup
	(
	std::ostream&			output,
	SVNRepoTreeList*	view
	)
	const
{
	itsSavedOpenNodes->CleanOut();
	const_cast<SVNRepoTree*>(this)->itsView = view;
	SaveOpenNodes();

	output << *itsSavedOpenNodes;
}

/******************************************************************************
 GetRepoRoot

 ******************************************************************************/

SVNRepoTreeNode*
SVNRepoTree::GetRepoRoot()
{
	SVNRepoTreeNode* root = dynamic_cast<SVNRepoTreeNode*>(GetRoot());
	assert( root != NULL );
	return root;
}

const SVNRepoTreeNode*
SVNRepoTree::GetRepoRoot()
	const
{
	const SVNRepoTreeNode* root = dynamic_cast<const SVNRepoTreeNode*>(GetRoot());
	assert( root != NULL );
	return root;
}
