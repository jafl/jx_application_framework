/******************************************************************************
 CBProjectTree.cpp

	BASE CLASS = JTree

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "CBProjectTree.h"
#include "CBProjectNode.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectTree::CBProjectTree
	(
	CBProjectDocument* doc
	)
	:
	JTree(jnew CBProjectNode(nullptr, kCBRootNT, JString("root", JString::kNoCopy), true))
{
	itsDoc = doc;
}

CBProjectTree::CBProjectTree
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBProjectDocument*	doc
	)
	:
	JTree(CBProjectNode::StreamIn(input, vers, nullptr))
{
	itsDoc = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectTree::~CBProjectTree()
{
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
CBProjectTree::StreamOut
	(
	std::ostream& output
	)
	const
{
	GetProjectRoot()->StreamOut(output);
}

/******************************************************************************
 GetProjectRoot

 ******************************************************************************/

CBProjectNode*
CBProjectTree::GetProjectRoot()
	const
{
	auto* rootNode =
		dynamic_cast<CBProjectNode*>(const_cast<JTreeNode*>(GetRoot()));
	assert( rootNode != nullptr );
	return rootNode;
}

/******************************************************************************
 BuildMakeFiles

	Returns true if all the source files exist.

 ******************************************************************************/

bool
CBProjectTree::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	text->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 BuildCMakeData

	Returns true if all the source files exist.

 ******************************************************************************/

bool
CBProjectTree::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	src->Clear();
	hdr->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildCMakeData(src, hdr, invalidList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 BuildQMakeData

	Returns true if all the source files exist.

 ******************************************************************************/

bool
CBProjectTree::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	src->Clear();
	hdr->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildQMakeData(src, hdr, invalidList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 ParseFiles

 ******************************************************************************/

void
CBProjectTree::ParseFiles
	(
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBDTree*					dTree,
	CBGoTree*					goTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	GetProjectRoot()->ParseFiles(parser, allSuffixList, symbolList,
								 cTree, dTree, goTree, javaTree, phpTree, pg);
}

/******************************************************************************
 Print

 ******************************************************************************/

void
CBProjectTree::Print
	(
	JString* text
	)
	const
{
	GetProjectRoot()->Print(text);
}

/******************************************************************************
 CreateFilesForTemplate

 ******************************************************************************/

void
CBProjectTree::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	GetProjectRoot()->CreateFilesForTemplate(input, vers);
}

/******************************************************************************
 SaveFilesInTemplate

 ******************************************************************************/

void
CBProjectTree::SaveFilesInTemplate
	(
	std::ostream& output
	)
	const
{
	GetProjectRoot()->SaveFilesInTemplate(output);
}
