/******************************************************************************
 CBLibraryNode.cpp

	BASE CLASS = CBFileNodeBase

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBLibraryNode.h"
#include "CBSubprojectConfigDialog.h"
#include "CBProjectDocument.h"
#include "CBBuildManager.h"
#include "cbGlobals.h"
#include <JTree.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBLibraryNode::CBLibraryNode
	(
	CBProjectTree*		tree,
	const JCharacter*	fileName
	)
	:
	CBFileNodeBase(tree, kCBLibraryNT, fileName)
{
	CBLibraryNodeX();
}

CBLibraryNode::CBLibraryNode
	(
	istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
	:
	CBFileNodeBase(input, vers, parent, kCBLibraryNT)
{
	CBLibraryNodeX();
	if (vers >= 39)
		{
		input >> itsIncludeInDepListFlag;
		}
	input >> itsProjFileName;
	if (vers >= 38)
		{
		input >> itsShouldBuildFlag;
		}
}

// private

void
CBLibraryNode::CBLibraryNodeX()
{
	itsIncludeInDepListFlag = kJTrue;
	itsShouldBuildFlag      = kJTrue;
	itsSubprojDialog        = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBLibraryNode::~CBLibraryNode()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBLibraryNode::StreamOut
	(
	ostream& output
	)
	const
{
	CBFileNodeBase::StreamOut(output);
	output << ' ' << itsIncludeInDepListFlag;
	output << ' ' << itsProjFileName;
	output << ' ' << itsShouldBuildFlag << '\n';
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
CBLibraryNode::OpenFile()
	const
{
	CBLibraryNode* me = const_cast<CBLibraryNode*>(this);
	if (!me->OpenProject())
		{
		me->EditSubprojectConfig();
		}
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
CBLibraryNode::OpenComplementFile()
	const
{
}

/******************************************************************************
 IncludedInMakefile (virtual)

 ******************************************************************************/

JBoolean
CBLibraryNode::IncludedInMakefile()
	const
{
	return JI2B(itsIncludeInDepListFlag &&
				(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
					CBBuildManager::kMakemake);
}

/******************************************************************************
 BuildMakeFiles (virtual)

 ******************************************************************************/

void
CBLibraryNode::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	JString projFullName;
	const JBoolean projExists = GetFullName(itsProjFileName, &projFullName);

	if (itsShouldBuildFlag && projExists &&
		(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
			CBBuildManager::kMakemake)
		{
		libFileList->Append(GetFileName());

		JString path, name;
		JSplitPathAndName(itsProjFileName, &path, &name);
		libProjPathList->Append(path);

		CBProjectDocument* doc;
		if ((CBGetDocumentManager())->ProjectDocumentIsOpen(projFullName, &doc))
			{
			(doc->GetBuildManager())->UpdateMakeFiles();
			}
		}

	// don't complain about non-existent library if we have project to build it

	CBFileNodeBase::BuildMakeFiles(text,
								   (itsShouldBuildFlag && projExists) ?
										NULL : invalidList,
								   libFileList, libProjPathList);
}

/******************************************************************************
 EditSubprojectConfig

 ******************************************************************************/

void
CBLibraryNode::EditSubprojectConfig()
{
	assert( itsSubprojDialog == NULL );

	CBProjectDocument* doc = GetProjectDoc();

	itsSubprojDialog =
		new CBSubprojectConfigDialog(doc, itsIncludeInDepListFlag,
									 itsProjFileName, itsShouldBuildFlag,
									 doc->GetRelPathCSF());
	assert( itsSubprojDialog != NULL );
	itsSubprojDialog->BeginDialog();
	ListenTo(itsSubprojDialog);
}

/******************************************************************************
 UpdateSubprojectConfig (private)

 ******************************************************************************/

void
CBLibraryNode::UpdateSubprojectConfig()
{
	assert( itsSubprojDialog != NULL );

	JBoolean include, build;
	itsSubprojDialog->GetConfig(&include, &itsProjFileName, &build);

	const JBoolean changed =
		JI2B( include != itsIncludeInDepListFlag ||
			  build   != itsShouldBuildFlag );

	JTree* tree;
	if (changed && GetTree(&tree))
		{
		itsIncludeInDepListFlag = kJTrue;		// force rebuild
		tree->BroadcastChange(this);
		}
	itsIncludeInDepListFlag = include;
	itsShouldBuildFlag      = build;
}

/******************************************************************************
 OpenProject

 ******************************************************************************/

JBoolean
CBLibraryNode::OpenProject
	(
	const JBoolean silent
	)
{
	JString fullName;
	if (GetFullName(itsProjFileName, &fullName))
		{
		CBProjectDocument* doc;
		CBProjectDocument::Create(fullName, silent, &doc);
		return JI2B( doc != NULL );
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBLibraryNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSubprojDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateSubprojectConfig();
			}
		itsSubprojDialog = NULL;
		}

	else
		{
		CBFileNodeBase::Receive(sender, message);
		}
}
