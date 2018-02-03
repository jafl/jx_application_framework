/******************************************************************************
 CBJavaTree.cpp

	This class instantiates a Java inheritance tree.

	BASE CLASS = CBTree, CBCtagsUser

	Copyright (C) 1995-99 John Lindal.

 ******************************************************************************/

#include "CBJavaTree.h"
#include "CBJavaClass.h"
#include "CBJavaTreeDirector.h"
#include "CBJavaTreeScanner.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <strstream>
#include <jAssert.h>

static const JCharacter* kCtagsArgs =
	"--format=2 --excmd=number --sort=no --java-kinds=me";

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
	CBCtagsUser(kCtagsArgs)
{
	itsClassNameLexer = NULL;
}

CBJavaTree::CBJavaTree
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	CBJavaTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInJavaClass, director, kCBJavaSourceFT, marginWidth, dirList),
	CBCtagsUser(kCtagsArgs)
{
	itsClassNameLexer = NULL;

	if (projVers < 86 && !IsEmpty())
		{
		NextUpdateMustReparseAll();
		}
}

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
	std::ostream&			projOutput,
	std::ostream*			setOutput,
	std::ostream*			symOutput,
	const CBDirList*	dirList
	)
	const
{
	assert( dirList == NULL );
	CBTree::StreamOut(projOutput, setOutput, symOutput, dirList);
}

/******************************************************************************
 StreamInJavaClass (static private)

	Creates a jnew CBJavaClass from the data in the given stream.

 ******************************************************************************/

CBClass*
CBJavaTree::StreamInJavaClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	CBJavaClass* newClass = jnew CBJavaClass(input, vers, tree);
	assert( newClass != NULL );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

JBoolean
CBJavaTree::UpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = NULL;

	DeleteProcess();

	return CBTree::UpdateFinished(deadFileList);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates one CBJavaClass.

 ******************************************************************************/

void
CBJavaTree::ParseFile
	(
	const JCharacter*	fileName,
	const JFAID_t		id
	)
{
	if (itsClassNameLexer == NULL)
		{
		itsClassNameLexer = jnew CBJavaTreeScanner;
		assert( itsClassNameLexer != NULL );
		}

	// extract info about classes

	JPtrArray<CBClass> classList(JPtrArrayT::kForgetAll);
	if (!itsClassNameLexer->CreateClasses(fileName, id, this, &classList))
		{
		return;
		}

	CBClass* newClass = classList.FirstElement();

/* this destroys the ability to minimize MI links

	// default superclass

	if (newClass != NULL && !newClass->HasParents())
		{
		newClass->AddParent(CBClass::kInheritPublic, "java.lang.Object");
		}
*/
	// extract functions via ctags - TODO: don't attach everything to root class

	JString data;
	CBLanguage lang;
	if (ProcessFile(fileName, kCBJavaSourceFT, &data, &lang))
		{
		std::istrstream input(data.GetCString(), data.GetLength());
		ReadFunctionList(input, newClass);
		}
}

/******************************************************************************
 ReadFunctionList (private)

 ******************************************************************************/

void
CBJavaTree::ReadFunctionList
	(
	std::istream&	input,
	CBClass*	theClass
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
CBJavaTree::DecodeAccess
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
		return CBClass::kJavaDefaultAccess;
		}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
CBJavaTree::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	CBTree::Receive(sender, message);
	CBCtagsUser::Receive(sender, message);
}
