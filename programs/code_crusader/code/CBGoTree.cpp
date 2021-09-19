/******************************************************************************
 CBGoTree.cpp

	This class instantiates a Go inheritance tree.

	BASE CLASS = CBTree

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "CBGoTree.h"
#include "CBGoClass.h"
#include "CBGoTreeDirector.h"
#include "CBGoTreeScanner.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <fstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBGoTree::CBGoTree
	(
	CBGoTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInGoClass, director, kCBGoFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

CBGoTree::CBGoTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBGoTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInGoClass, director, kCBGoFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

CBGoTree::~CBGoTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBGoTree::StreamOut
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
 StreamInGoClass (static private)

	Creates a jnew CBGoClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBGoTree::StreamInGoClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	auto* newClass = jnew CBGoClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

bool
CBGoTree::UpdateFinished
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

	Parses the given file and creates CBGoClasses.

 ******************************************************************************/

void
CBGoTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
		{
		itsClassNameLexer = jnew CB::Tree::Go::Scanner;
		assert( itsClassNameLexer != nullptr );
		}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(CB::Tree::Go::Scanner::INITIAL);

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(id, this, &classList);
}
