/******************************************************************************
 CBProjectNode.cpp

	Base class for all items in a project window.

	BASE CLASS = JNamedTreeNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

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
	const JString&			name,
	const bool			isOpenable
	)
	:
	JNamedTreeNode(tree, name, isOpenable),
	itsType(type)
{
}

// protected

CBProjectNode::CBProjectNode
	(
	std::istream&			input,
	const JFileVersion		vers,
	CBProjectNode*			parent,
	const CBProjectNodeType	type,
	const bool			isOpenable
	)
	:
	JNamedTreeNode(nullptr, JString::empty, isOpenable),
	itsType(type)
{
	if (parent != nullptr)
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
		while (true)
			{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
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
	const auto* tree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert( tree != nullptr );
	return tree->GetProjectDoc();
}

/******************************************************************************
 StreamIn (static)

 ******************************************************************************/

CBProjectNode*
CBProjectNode::StreamIn
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
{
	CBProjectNodeType type;
	input >> type;

	CBProjectNode* node = nullptr;
	if (type == kCBRootNT)
		{
		assert( parent == nullptr );
		node = jnew CBProjectNode(input, vers, parent, kCBRootNT, true);
		}
	else if (type == kCBGroupNT)
		{
		node = jnew CBGroupNode(input, vers, parent);
		}
	else if (type == kCBFileNT)
		{
		node = jnew CBFileNode(input, vers, parent);
		}
	else if (type == kCBLibraryNT)
		{
		node = jnew CBLibraryNode(input, vers, parent);
		}
	assert( node != nullptr );

	return node;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBProjectNode::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << itsType;
	output << ' ' << GetName() << '\n';

	const JSize childCount = GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
		{
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		output << JBoolToString(true) << '\n';
		child->StreamOut(output);
		}

	output << JBoolToString(false) << '\n';
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

	Returns true if this is a file that can be found.

 ******************************************************************************/

bool
CBProjectNode::GetFullName
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return false;
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
	const bool silent
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
	const bool silent
	)
	const
{
}

/******************************************************************************
 FindFile

 ******************************************************************************/

bool
CBProjectNode::FindFile
	(
	const JString&			fullName,
	const CBProjectNode**	node
	)
	const
{
	return const_cast<CBProjectNode*>(this)->
		FindFile(fullName, const_cast<CBProjectNode**>(node));
}

bool
CBProjectNode::FindFile
	(
	const JString&	fullName,
	CBProjectNode**	node
	)
{
	if (JFileExists(fullName))
		{
		return FindFile1(fullName, node);
		}
	else
		{
		*node = nullptr;
		return false;
		}
}

/******************************************************************************
 FindFile1 (virtual protected)

	Derived classes must override to check if they are the specified file.

 ******************************************************************************/

bool
CBProjectNode::FindFile1
	(
	const JString&	fullName,
	CBProjectNode**	node
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* child = dynamic_cast<CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		if (child->FindFile1(fullName, node))
			{
			return true;
			}
		}

	*node = nullptr;
	return false;
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
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

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
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

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
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->BuildQMakeData(src, hdr, invalidList);
		}
}

/******************************************************************************
 ParseFiles (virtual)

	If the derived class represents a file, it should pass it to the parser.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
CBProjectNode::ParseFiles
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
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		if (!child->ParseFiles(parser, allSuffixList, symbolList,
							   cTree, dTree, goTree, javaTree, phpTree, pg))
			{
			return false;
			}
		}

	return true;
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
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->Print(text);
		}
}

/******************************************************************************
 FileRenamed (virtual)

 ******************************************************************************/

void
CBProjectNode::FileRenamed
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* child = dynamic_cast<CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->FileRenamed(origFullName, newFullName);
		}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

bool
CBProjectNode::IncludedInMakefile()
	const
{
	return false;
}

/******************************************************************************
 IncludedInCMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
CBProjectNode::IncludedInCMakeData()
	const
{
	return false;
}

/******************************************************************************
 IncludedInQMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
CBProjectNode::IncludedInQMakeData()
	const
{
	return false;
}

/******************************************************************************
 CreateFilesForTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
CBProjectNode::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

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
	std::ostream& output
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		const auto* child = dynamic_cast<const CBProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->SaveFilesInTemplate(output);
		}
}

/******************************************************************************
 GetProjectTree

 ******************************************************************************/

CBProjectTree*
CBProjectNode::GetProjectTree()
{
	auto* tree = dynamic_cast<CBProjectTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const CBProjectTree*
CBProjectNode::GetProjectTree()
	const
{
	const auto* tree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

/******************************************************************************
 GetProjectParent

 ******************************************************************************/

CBProjectNode*
CBProjectNode::GetProjectParent()
{
	JTreeNode* p     = GetParent();
	auto* n = dynamic_cast<CBProjectNode*>(p);
	assert( n != nullptr );
	return n;
}

const CBProjectNode*
CBProjectNode::GetProjectParent()
	const
{
	const JTreeNode* p     = GetParent();
	const auto* n = dynamic_cast<const CBProjectNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
CBProjectNode::GetProjectParent
	(
	CBProjectNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<CBProjectNode*>(p);
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
 GetProjectChild

 ******************************************************************************/

CBProjectNode*
CBProjectNode::GetProjectChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<CBProjectNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const CBProjectNode*
CBProjectNode::GetProjectChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const CBProjectNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}
