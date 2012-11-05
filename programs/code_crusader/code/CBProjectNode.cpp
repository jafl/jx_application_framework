/******************************************************************************
 CBProjectNode.cpp

	Base class for all items in a project window.

	BASE CLASS = JNamedTreeNode

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBProjectNode.h"
#include "CBProjectTree.h"
#include "CBGroupNode.h"
#include "CBFileNode.h"
#include "CBLibraryNode.h"
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectNode::CBProjectNode
	(
	CBProjectTree*			tree,
	const CBProjectNodeType	type,
	const JCharacter*		name,
	const JBoolean			isOpenable
	)
	:
	JNamedTreeNode(tree, name, isOpenable),
	itsType(type)
{
}

// protected

CBProjectNode::CBProjectNode
	(
	istream&				input,
	const JFileVersion		vers,
	CBProjectNode*			parent,
	const CBProjectNodeType	type,
	const JBoolean			isOpenable
	)
	:
	JNamedTreeNode(NULL, "", isOpenable),
	itsType(type)
{
	if (parent != NULL)
		{
		parent->Append(this);
		}

	JString name;
	input >> name;
	SetName(name);

	if (vers <= 71)
		{
		JSize childCount;
		input >> childCount;

		for (JIndex i=1; i<=childCount; i++)
			{
			StreamIn(input, vers, this);		// connects itself
			}
		}
	else
		{
		while (1)
			{
			JBoolean keepGoing;
			input >> keepGoing;
			if (!keepGoing)
				{
				break;
				}

			StreamIn(input, vers, this);
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectNode::~CBProjectNode()
{
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

CBProjectDocument*
CBProjectNode::GetProjectDoc()
	const
{
	const CBProjectTree* tree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert( tree != NULL );
	return tree->GetProjectDoc();
}

/******************************************************************************
 StreamIn (static)

 ******************************************************************************/

CBProjectNode*
CBProjectNode::StreamIn
	(
	istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
{
	CBProjectNodeType type;
	input >> type;

	CBProjectNode* node = NULL;
	if (type == kCBRootNT)
		{
		assert( parent == NULL );
		node = new CBProjectNode(input, vers, parent, kCBRootNT, kJTrue);
		}
	else if (type == kCBGroupNT)
		{
		node = new CBGroupNode(input, vers, parent);
		}
	else if (type == kCBFileNT)
		{
		node = new CBFileNode(input, vers, parent);
		}
	else if (type == kCBLibraryNT)
		{
		node = new CBLibraryNode(input, vers, parent);
		}
	assert( node != NULL );

	return node;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBProjectNode::StreamOut
	(
	ostream& output
	)
	const
{
	output << itsType;
	output << ' ' << GetName() << '\n';

	const JSize childCount = GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		output << kJTrue << '\n';
		child->StreamOut(output);
		}

	output << kJFalse << '\n';
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
CBProjectNode::OpenFile()
	const
{
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
CBProjectNode::OpenComplementFile()
	const
{
}

/******************************************************************************
 GetFullName (virtual)

	Returns kJTrue if this is a file that can be found.

 ******************************************************************************/

JBoolean
CBProjectNode::GetFullName
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return kJFalse;
}

/******************************************************************************
 ShowFileLocation (virtual)

	Don't automatically propagate, to avoid execing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
CBProjectNode::ShowFileLocation()
	const
{
}

/******************************************************************************
 ViewPlainDiffs (virtual)

	Don't automatically propagate, to avoid diffing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
CBProjectNode::ViewPlainDiffs
	(
	const JBoolean silent
	)
	const
{
}

/******************************************************************************
 ViewVCSDiffs (virtual)

	Don't automatically propagate, to avoid diffing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
CBProjectNode::ViewVCSDiffs
	(
	const JBoolean silent
	)
	const
{
}

/******************************************************************************
 FindFile

 ******************************************************************************/

JBoolean
CBProjectNode::FindFile
	(
	const JCharacter*		fullName,
	const CBProjectNode**	node
	)
	const
{
	return const_cast<CBProjectNode*>(this)->
		FindFile(fullName, const_cast<CBProjectNode**>(node));
}

JBoolean
CBProjectNode::FindFile
	(
	const JCharacter*	fullName,
	CBProjectNode**		node
	)
{
	if (JFileExists(fullName))
		{
		return FindFile1(fullName, node);
		}
	else
		{
		*node = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 FindFile1 (virtual protected)

	Derived classes must override to check if they are the specified file.

 ******************************************************************************/

JBoolean
CBProjectNode::FindFile1
	(
	const JCharacter*	fullName,
	CBProjectNode**		node
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBProjectNode* child = dynamic_cast<CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		if (child->FindFile1(fullName, node))
			{
			return kJTrue;
			}
		}

	*node = NULL;
	return kJFalse;
}

/******************************************************************************
 BuildMakeFiles (virtual)

	If all source files existed, text contains the text for Make.files.
	Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
CBProjectNode::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
		}
}

/******************************************************************************
 BuildCMakeData (virtual)

	If all source files existed, strings contain the text for CMakeLists.txt
	file.  Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
CBProjectNode::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->BuildCMakeData(src, hdr, invalidList);
		}
}

/******************************************************************************
 BuildQMakeData (virtual)

	If all source files existed, strings contain the text for .pro file.
	Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
CBProjectNode::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->BuildQMakeData(src, hdr, invalidList);
		}
}

/******************************************************************************
 ParseFiles (virtual)

	If the derived class represents a file, it should pass it to the parser.
	Returns kJFalse if process was cancelled by user.

 ******************************************************************************/

JBoolean
CBProjectNode::ParseFiles
	(
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBJavaTree*					javaTree,
	JProgressDisplay&			pg
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		if (!child->ParseFiles(parser, allSuffixList, symbolList, cTree, javaTree, pg))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 Print (virtual)

	Derived classes must override to add themselves to the text.

 ******************************************************************************/

void
CBProjectNode::Print
	(
	JString* text
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->Print(text);
		}
}

/******************************************************************************
 FileRenamed (virtual)

 ******************************************************************************/

void
CBProjectNode::FileRenamed
	(
	const JCharacter* origFullName,
	const JCharacter* newFullName
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		CBProjectNode* child = dynamic_cast<CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->FileRenamed(origFullName, newFullName);
		}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

JBoolean
CBProjectNode::IncludedInMakefile()
	const
{
	return kJFalse;
}

/******************************************************************************
 IncludedInCMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

JBoolean
CBProjectNode::IncludedInCMakeData()
	const
{
	return kJFalse;
}

/******************************************************************************
 IncludedInQMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

JBoolean
CBProjectNode::IncludedInQMakeData()
	const
{
	return kJFalse;
}

/******************************************************************************
 CreateFilesForTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
CBProjectNode::CreateFilesForTemplate
	(
	istream&			input,
	const JFileVersion	vers
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->CreateFilesForTemplate(input, vers);
		}
}

/******************************************************************************
 SaveFilesInTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
CBProjectNode::SaveFilesInTemplate
	(
	ostream& output
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBProjectNode* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != NULL );

		child->SaveFilesInTemplate(output);
		}
}

/******************************************************************************
 GetProjectTree

 ******************************************************************************/

CBProjectTree*
CBProjectNode::GetProjectTree()
{
	CBProjectTree* tree = dynamic_cast<CBProjectTree*>(GetTree());
	assert (tree != NULL);
	return tree;
}

const CBProjectTree*
CBProjectNode::GetProjectTree()
	const
{
	const CBProjectTree* tree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert (tree != NULL);
	return tree;
}

/******************************************************************************
 GetProjectParent

 ******************************************************************************/

CBProjectNode*
CBProjectNode::GetProjectParent()
{
	JTreeNode* p     = GetParent();
	CBProjectNode* n = dynamic_cast<CBProjectNode*>(p);
	assert( n != NULL );
	return n;
}

const CBProjectNode*
CBProjectNode::GetProjectParent()
	const
{
	const JTreeNode* p     = GetParent();
	const CBProjectNode* n = dynamic_cast<const CBProjectNode*>(p);
	assert( n != NULL );
	return n;
}

JBoolean
CBProjectNode::GetProjectParent
	(
	CBProjectNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<CBProjectNode*>(p);
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
CBProjectNode::GetProjectParent
	(
	const CBProjectNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const CBProjectNode*>(p);
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
 GetProjectChild

 ******************************************************************************/

CBProjectNode*
CBProjectNode::GetProjectChild
	(
	const JIndex index
	)
{
	CBProjectNode* node = dynamic_cast<CBProjectNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}

const CBProjectNode*
CBProjectNode::GetProjectChild
	(
	const JIndex index
	)
	const
{
	const CBProjectNode* node = dynamic_cast<const CBProjectNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}
