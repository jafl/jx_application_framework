/******************************************************************************
 CBPHPTree.cpp

	This class instantiates a PHP inheritance tree.

	BASE CLASS = CBTree

	Copyright (C) 2014-2021 John Lindal.

 ******************************************************************************/

#include "CBPHPTree.h"
#include "CBPHPClass.h"
#include "CBPHPTreeDirector.h"
#include "CBPHPTreeScanner.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPHPTree::CBPHPTree
	(
	CBPHPTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInPHPClass, director, kCBPHPFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

CBPHPTree::CBPHPTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBPHPTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInPHPClass, director, kCBPHPFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
	if (projVers < 81 && !IsEmpty())
		{
		NextUpdateMustReparseAll();
		}
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPHPTree::~CBPHPTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBPHPTree::StreamOut
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
 StreamInPHPClass (static private)

	Creates a new CBPHPClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBPHPTree::StreamInPHPClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	CBPHPClass* newClass = jnew CBPHPClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBPHPTree::UpdateFinished
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

	Parses the given file and creates one CBPHPClass.

 ******************************************************************************/

void
CBPHPTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
		{
		itsClassNameLexer = jnew CBPHPTreeScanner;
		assert( itsClassNameLexer != nullptr );
		}

	// extract info about class

	CBClass* newClass;
	itsClassNameLexer->CreateClass(fileName, id, this, &newClass);
}
