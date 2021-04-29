/******************************************************************************
 CBPHPTree.cpp

	This class instantiates a PHP inheritance tree.

	BASE CLASS = CBTree, CBCtagsUser

	Copyright (C) 2014 John Lindal.

 ******************************************************************************/

#include "CBPHPTree.h"
#include "CBPHPClass.h"
#include "CBPHPTreeDirector.h"
#include "CBPHPTreeScanner.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <strstream>
#include <jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --php-kinds=f";

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
	CBCtagsUser(kCtagsArgs)
{
	itsClassNameLexer = nullptr;
}

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
	CBCtagsUser(kCtagsArgs)
{
	itsClassNameLexer = nullptr;

	if (projVers < 81 && !IsEmpty())
		{
		NextUpdateMustReparseAll();
		}
}

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
	std::ostream&			projOutput,
	std::ostream*			setOutput,
	std::ostream*			symOutput,
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

	DeleteProcess();

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
	if (!itsClassNameLexer->CreateClass(fileName, id, this, &newClass))
		{
		return;
		}

	// extract functions via ctags

	JString data;
	CBLanguage lang;
	if (ProcessFile(fileName, kCBPHPFT, &data, &lang))
		{
		std::istrstream input(data.GetBytes(), data.GetByteCount());
		ReadFunctionList(input, newClass);
		}
}

/******************************************************************************
 ReadFunctionList (private)

 ******************************************************************************/

void
CBPHPTree::ReadFunctionList
	(
	std::istream&	input,
	CBClass*		theClass
	)
{
	input >> std::ws;
	while (input.peek() == '!')
		{
		JIgnoreLine(input);
		input >> std::ws;
		}

	JString name;
	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (1)
		{
		input >> std::ws;
		name = JReadUntil(input, '\t');			// function name
		if (input.eof() || input.fail())
			{
			break;
			}

		ReadExtensionFlags(input, &flags);		// skips file name and line number

		JString* impl;
		const JBoolean implemented =
			JNegate(flags.GetElement("implementation", &impl) && *impl == "abstract");

		theClass->AddFunction(name, DecodeAccess(flags), implemented);
		}
}

/******************************************************************************
 DecodeAccess (private)

 ******************************************************************************/

CBClass::FnAccessLevel
CBPHPTree::DecodeAccess
	(
	const JStringPtrMap<JString>& flags
	)
	const
{
	const JString* value;
	const JBoolean exists = flags.GetElement("access", &value);

	if (exists && *value == "public")
		{
		return CBClass::kPublicAccess;
		}
	else if (exists && *value == "private")
		{
		return CBClass::kPrivateAccess;
		}
	else if (exists && *value == "protected")
		{
		return CBClass::kProtectedAccess;
		}
	else	// default
		{
		return CBClass::kPublicAccess;
		}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
CBPHPTree::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CBTree::Receive(sender, message);
	CBCtagsUser::Receive(sender, message);
}
