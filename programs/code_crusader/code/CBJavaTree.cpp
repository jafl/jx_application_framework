/******************************************************************************
 CBJavaTree.cpp

	This class instantiates a Java inheritance tree.

	BASE CLASS = CBTree

	Copyright © 1995-2021 John Lindal.

 ******************************************************************************/

#include "CBJavaTree.h"
#include "CBJavaClass.h"
#include "CBJavaTreeDirector.h"
#include "CBJavaTreeScanner.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <fstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBJavaTree::CBJavaTree
	(
	CBJavaTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInJavaClass, director, kCBJavaSourceFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

CBJavaTree::CBJavaTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBJavaTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInJavaClass, director, kCBJavaSourceFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
	if (projVers < 88 && !IsEmpty())	// new parser
		{
		NextUpdateMustReparseAll();
		}
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaTree::~CBJavaTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBJavaTree::StreamOut
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
 StreamInJavaClass (static private)

	Creates a jnew CBJavaClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBJavaTree::StreamInJavaClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	auto* newClass = jnew CBJavaClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

bool
CBJavaTree::UpdateFinished
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

	Parses the given file and creates CBJavaClasses.

	We do not create java.lang.Object, because that destroys the ability to
	minimize MI links.

 ******************************************************************************/

void
CBJavaTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
		{
		itsClassNameLexer = jnew CB::Tree::Java::Scanner;
		assert( itsClassNameLexer != nullptr );
		}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(CB::Tree::Java::Scanner::INITIAL);

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(id, this, &classList);
}
