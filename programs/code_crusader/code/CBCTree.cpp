/******************************************************************************
 CBCTree.cpp

	This class instantiates a C++ inheritance tree.

	BASE CLASS = CBTree

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#include "CBCTree.h"
#include "CBCClass.h"
#include "CBCTreeDirector.h"
#include "CBCTreeScanner.h"
#include "CBCPreprocessor.h"
#include <JStringIterator.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <fstream>
#include <jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --c-kinds=f";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCTree::CBCTree
	(
	CBCTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInCClass, director, kCBCHeaderFT, marginWidth),
	CBCtagsUser(kCtagsArgs),
	itsClassNameLexer(nullptr)
{
	CBCTreeX();
}

#ifndef CODE_CRUSADER_UNIT_TEST

CBCTree::CBCTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CBCTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInCClass, director, kCBCHeaderFT, marginWidth, dirList),
	CBCtagsUser(kCtagsArgs),
	itsClassNameLexer(nullptr)
{
	CBCTreeX();

	if (projVers >= 28)
		{
		itsCPP->ReadSetup(projInput, projVers);
		}

	if (projVers < 88 && !IsEmpty())		// new parser
		{
		NextUpdateMustReparseAll();
		}
}

#endif

// private

void
CBCTree::CBCTreeX()
{
	itsCPP = jnew CBCPreprocessor;
	assert( itsCPP != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCTree::~CBCTree()
{
	jdelete itsCPP;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBCTree::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const CBDirList*	dirList
	)
	const
{
	CBTree::StreamOut(projOutput, setOutput, symOutput, dirList);

	itsCPP->WriteSetup(projOutput);
}

/******************************************************************************
 StreamInCClass (static private)

	Creates a new CBCClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBCTree::StreamInCClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	auto* newClass = jnew CBCClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

bool
CBCTree::UpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = nullptr;

	DeleteProcess();

	return CBTree::UpdateFinished(deadFileList);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates CBCClasses.

 ******************************************************************************/

void
CBCTree::ParseFile
	(
	const JString&	origFileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
		{
		itsClassNameLexer = jnew CB::Tree::C::Scanner;
		assert( itsClassNameLexer != nullptr );
		}

	// Read in the entire file and apply preprocessor.

	JString fileName = origFileName;

	JString buffer;
	JReadFile(fileName, &buffer);
	if (itsCPP->Preprocess(&buffer))
		{
		JString newFileName;
		if (!JCreateTempFile(&newFileName).OK())
			{
			return;
			}

		fileName = newFileName;

		std::ofstream output(fileName.GetBytes());
		buffer.Print(output);
		}

	// extract info about classes

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	JString data;
	CBLanguage lang;

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(CB::Tree::C::Scanner::INITIAL);

	if (itsClassNameLexer->CreateClasses(id, this, &classList) &&
		ProcessFile(fileName, kCBCHeaderFT, &data, &lang))
		{
		// check for pure virtual via ctags

		icharbufstream input(data.GetBytes(), data.GetByteCount());

		input >> std::ws;
		while (input.peek() == '!')
			{
			JIgnoreLine(input);
			input >> std::ws;
			}

		JString name;
		JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
		while (true)
			{
			input >> std::ws;
			name = JReadUntil(input, '\t');			// function name
			if (input.eof() || input.fail())
				{
				break;
				}

			ReadExtensionFlags(input, &flags);		// skips file name and line number

			JString* impl;
			if (!flags.GetElement("implementation", &impl) || *impl != "abstract")
				{
				continue;
				}

			JStringIterator iter(&name, kJIteratorStartAtEnd);
			if (iter.Prev("::"))
				{
				iter.RemoveAllNext();
				iter.Invalidate();
				}
			else
				{
				continue;
				}

			for (auto* c : classList)
				{
				if (name == c->GetFullName())
					{
					c->SetAbstract();
					break;
					}
				}
			}
		}

	if (fileName != origFileName)
		{
		JRemoveFile(fileName);
		}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
CBCTree::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CBTree::Receive(sender, message);
	CBCtagsUser::Receive(sender, message);
}
