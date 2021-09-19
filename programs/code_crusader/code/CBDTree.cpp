/******************************************************************************
 CBDTree.cpp

	This class instantiates a D inheritance tree.

	BASE CLASS = CBTree

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "CBDTree.h"
#include "CBDClass.h"
#include "CBDTreeDirector.h"
#include "CBDTreeScanner.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDTree::CBDTree
	(
	CBDTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInCClass, director, kCBDFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

CBDTree::CBDTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBDTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInCClass, director, kCBDFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDTree::~CBDTree()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBDTree::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const CBDirList*	dirList
	)
	const
{
	assert( dirList == nullptr );
	CBTree::StreamOut(projOutput, setOutput, symOutput, dirList);
}

/******************************************************************************
 StreamInCClass (static private)

	Creates a new CBDClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBDTree::StreamInCClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	auto* newClass = jnew CBDClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

bool
CBDTree::UpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = nullptr;

	return CBTree::UpdateFinished(deadFileList);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates CBDClasses.

 ******************************************************************************/

void
CBDTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
		{
		itsClassNameLexer = jnew CB::Tree::D::Scanner;
		assert( itsClassNameLexer != nullptr );
		}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(CB::Tree::D::Scanner::INITIAL);

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(fileName, id, this, &classList);
}
