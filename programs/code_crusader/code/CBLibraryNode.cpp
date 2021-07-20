/******************************************************************************
 CBLibraryNode.cpp

	BASE CLASS = CBFileNodeBase

	Copyright © 1999 John Lindal.

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
	CBProjectTree*	tree,
	const JString&	fileName
	)
	:
	CBFileNodeBase(tree, kCBLibraryNT, fileName)
{
	CBLibraryNodeX();
}

CBLibraryNode::CBLibraryNode
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
	:
	CBFileNodeBase(input, vers, parent, kCBLibraryNT)
{
	CBLibraryNodeX();
	if (vers >= 39)
		{
		input >> JBoolFromString(itsIncludeInDepListFlag);
		}
	input >> itsProjFileName;
	if (vers >= 38)
		{
		input >> JBoolFromString(itsShouldBuildFlag);
		}
}

// private

void
CBLibraryNode::CBLibraryNodeX()
{
	itsIncludeInDepListFlag = true;
	itsShouldBuildFlag      = true;
	itsSubprojDialog        = nullptr;
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
	std::ostream& output
	)
	const
{
	CBFileNodeBase::StreamOut(output);
	output << ' ' << JBoolToString(itsIncludeInDepListFlag);
	output << ' ' << itsProjFileName;
	output << ' ' << JBoolToString(itsShouldBuildFlag) << '\n';
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
CBLibraryNode::OpenFile()
	const
{
	auto* me = const_cast<CBLibraryNode*>(this);
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

bool
CBLibraryNode::IncludedInMakefile()
	const
{
	return itsIncludeInDepListFlag &&
				(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
					CBBuildManager::kMakemake;
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
	const bool projExists = GetFullName(itsProjFileName, &projFullName);

	if (itsShouldBuildFlag && projExists &&
		(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
			CBBuildManager::kMakemake)
		{
		libFileList->Append(GetFileName());

		JString path, name;
		JSplitPathAndName(itsProjFileName, &path, &name);
		libProjPathList->Append(path);

		CBProjectDocument* doc;
		if (CBGetDocumentManager()->ProjectDocumentIsOpen(projFullName, &doc))
			{
			(doc->GetBuildManager())->UpdateMakeFiles();
			}
		}

	// don't complain about non-existent library if we have project to build it

	CBFileNodeBase::BuildMakeFiles(text,
								   (itsShouldBuildFlag && projExists) ?
										nullptr : invalidList,
								   libFileList, libProjPathList);
}

/******************************************************************************
 EditSubprojectConfig

 ******************************************************************************/

void
CBLibraryNode::EditSubprojectConfig()
{
	assert( itsSubprojDialog == nullptr );

	CBProjectDocument* doc = GetProjectDoc();

	itsSubprojDialog =
		jnew CBSubprojectConfigDialog(doc, itsIncludeInDepListFlag,
									 itsProjFileName, itsShouldBuildFlag,
									 doc->GetRelPathCSF());
	assert( itsSubprojDialog != nullptr );
	itsSubprojDialog->BeginDialog();
	ListenTo(itsSubprojDialog);
}

/******************************************************************************
 UpdateSubprojectConfig (private)

 ******************************************************************************/

void
CBLibraryNode::UpdateSubprojectConfig()
{
	assert( itsSubprojDialog != nullptr );

	bool include, build;
	itsSubprojDialog->GetConfig(&include, &itsProjFileName, &build);

	const bool changed =
		include != itsIncludeInDepListFlag ||
			  build   != itsShouldBuildFlag;

	JTree* tree;
	if (changed && GetTree(&tree))
		{
		itsIncludeInDepListFlag = true;		// force rebuild
		tree->BroadcastChange(this);
		}
	itsIncludeInDepListFlag = include;
	itsShouldBuildFlag      = build;
}

/******************************************************************************
 OpenProject

 ******************************************************************************/

bool
CBLibraryNode::OpenProject
	(
	const bool silent
	)
{
	JString fullName;
	if (GetFullName(itsProjFileName, &fullName))
		{
		CBProjectDocument* doc;
		CBProjectDocument::Create(fullName, silent, &doc);
		return doc != nullptr;
		}
	else
		{
		return false;
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
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateSubprojectConfig();
			}
		itsSubprojDialog = nullptr;
		}

	else
		{
		CBFileNodeBase::Receive(sender, message);
		}
}
