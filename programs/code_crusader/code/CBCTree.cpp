/******************************************************************************
 CBCTree.cpp

	This class instantiates a C++ inheritance tree.

	BASE CLASS = CBTree

	Copyright © 1995-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBCTree.h"
#include "CBCClass.h"
#include "CBCTreeDirector.h"
#include "CBCPreprocessor.h"
#include "cbmUtil.h"
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <sstream>
#include <ctype.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCTree::CBCTree
	(
	CBCTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	CBTree(StreamInCClass, director, kCBCHeaderFT, marginWidth)
{
	CBCTreeX();
}

CBCTree::CBCTree
	(
	istream&			projInput,
	const JFileVersion	projVers,
	istream*			setInput,
	const JFileVersion	setVers,
	istream*			symInput,
	const JFileVersion	symVers,
	CBCTreeDirector*	director,
	const JSize			marginWidth,
	CBDirList*			dirList
	)
	:
	CBTree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInCClass, director, kCBCHeaderFT, marginWidth, dirList)
{
	CBCTreeX();

	if (projVers >= 28)
		{
		itsCPP->ReadSetup(projInput, projVers);
		}

	if (projVers < 58)		// parse namespaces correctly
		{
		NextUpdateMustReparseAll();
		}
}

// private

void
CBCTree::CBCTreeX()
{
	itsCPP = new CBCPreprocessor;
	assert( itsCPP != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCTree::~CBCTree()
{
	delete itsCPP;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBCTree::StreamOut
	(
	ostream&			projOutput,
	ostream*			setOutput,
	ostream*			symOutput,
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
	istream&			input,
	const JFileVersion	vers,
	CBTree*				tree
	)
{
	CBCClass* newClass = new CBCClass(input, vers, tree);
	assert( newClass != NULL );
	return newClass;
}

/******************************************************************************
 C++ header file parser

 ******************************************************************************/

static const JCharacter* kNamespaceKW = "namespace ";
static const JSize kNamespaceKWLength = strlen(kNamespaceKW);
static const JCharacter* kClassKW     = "class ";
static const JSize kClassKWLength     = strlen(kClassKW);
static const JCharacter* kStructKW    = "struct ";
static const JSize kStructKWLength    = strlen(kStructKW);
static const JCharacter* kEnumKW      = "enum ";
static const JSize kEnumKWLength      = strlen(kEnumKW);

static const JCharacter* kVirtualKW = "virtual";
static const JSize kVirtualKWLength = strlen(kVirtualKW);

static const JRegex pureVirtualRegex = "=[[:space:]]*0[[:space:]]*$";
static const JRegex fnNameRegex      = "[_~[:alpha:]][_[:alnum:]]*$|operator.+$";

struct InheritAccessInfo
{
	const JCharacter*		str;
	CBClass::InheritType	type;
};

static const InheritAccessInfo kInheritAccess[] =
{
	{ "public ",    CBClass::kInheritPublic    },
	{ "protected ", CBClass::kInheritProtected },
	{ "private ",   CBClass::kInheritPrivate   }
};
const JSize kInheritAccessCount = sizeof(kInheritAccess)/sizeof(InheritAccessInfo);

struct FnAccessInfo
{
	const JCharacter*		str;
	CBClass::FnAccessLevel	access;
};

static const FnAccessInfo kFnAccess[] =
{
	{ "public:",          CBClass::kPublicAccess          },
	{ "protected:",       CBClass::kProtectedAccess       },
	{ "private:",         CBClass::kPrivateAccess         },
	{ "signals:",         CBClass::kQtSignalAccess        },
	{ "public slots:",    CBClass::kQtPublicSlotAccess    },
	{ "protected slots:", CBClass::kQtProtectedSlotAccess },
	{ "private slots:",   CBClass::kQtPrivateSlotAccess   }
};
const JSize kFnAccessCount = sizeof(kFnAccess)/sizeof(FnAccessInfo);

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates CBCClasses.

 ******************************************************************************/

void
CBCTree::ParseFile
	(
	const JCharacter*	fileName,
	const JFAID_t		id
	)
{
	// Read in the entire file.

	JString buffer;
	ReadHeaderFile(fileName, &buffer);

	// Now that we have the entire file, we can search for the keywords.

	ParseClasses(&buffer, "", id);
}

/******************************************************************************
 ParseClasses

	Parses buffer and creates CBCClasses.
	This function destroys the data stored in buffer.

 ******************************************************************************/

void
CBCTree::ParseClasses
	(
	JString*			buffer,
	const JCharacter*	classNamePrefix,
	const JFAID_t		fileID
	)
{
	// Search for the class definition keywords.

	while (1)
		{
		JIndex nsIndex, classIndex, structIndex, enumIndex;
		if (!buffer->LocateSubstring(kNamespaceKW, &nsIndex))
			{
			nsIndex = buffer->GetLength()+1;
			}
		if (!buffer->LocateSubstring(kClassKW, &classIndex))
			{
			classIndex = buffer->GetLength()+1;
			}
		if (!buffer->LocateSubstring(kStructKW, &structIndex))
			{
			structIndex = buffer->GetLength()+1;
			}
		if (!buffer->LocateSubstring(kEnumKW, &enumIndex))
			{
			enumIndex = buffer->GetLength()+1;
			}

		JSize kwIndex, kwLength;
		CBClass::DeclareType declType;
		JBoolean isNamespace = kJFalse;
		if (nsIndex < classIndex && nsIndex < structIndex && nsIndex < enumIndex)
			{
			kwIndex     = nsIndex;
			kwLength    = kNamespaceKWLength;
			declType    = CBClass::kClassType;
			isNamespace = kJTrue;
			}
		else if (classIndex < nsIndex && classIndex < structIndex && classIndex < enumIndex)
			{
			kwIndex  = classIndex;
			kwLength = kClassKWLength;
			declType = CBClass::kClassType;
			}
		else if (structIndex < nsIndex && structIndex < classIndex && structIndex < enumIndex)
			{
			kwIndex  = structIndex;
			kwLength = kStructKWLength;
			declType = CBClass::kStructType;
			}
		else if (enumIndex < nsIndex && enumIndex < classIndex && enumIndex < structIndex)
			{
			kwIndex  = enumIndex;
			kwLength = kEnumKWLength;
			declType = CBClass::kEnumType;
			}
		else
			{
			break;
			}

		// If it's not surrounded by non-word characters, it's not a keyword.

		JSize bufLength = buffer->GetLength();
		if (kwIndex + kwLength >= bufLength)		// protects assert() below
			{
			break;
			}
		else if (kwIndex > 1 && CBMIsCharacterInWord(*buffer, kwIndex-1))
			{
			buffer->RemoveSubstring(1, kwIndex + kwLength-1);
			continue;
			}

		// check for template (space after > is enforced during read)

		JBoolean isTemplate = kJFalse;
		if ((declType == CBClass::kClassType || declType == CBClass::kStructType) &&
			(kwIndex > 2 &&
			 buffer->GetCharacter(kwIndex-2) == '>' &&
			 isspace(buffer->GetCharacter(kwIndex-1))))
			{
			isTemplate = kJTrue;
			}

		// We have to find a single word followed by an opening brace or a colon.
		// This indicates that it is the start of a class definition and not just
		// a forward declaration or the return type of a function.

		buffer->RemoveSubstring(1, kwIndex + kwLength-1);
		bufLength = buffer->GetLength();
		assert( !isspace(buffer->GetFirstCharacter()) );

		if (buffer->GetFirstCharacter() == '{')		// typedef or unnamed enum
			{
			continue;
			}

		JIndex braceIndex;
		if (!buffer->LocateSubstring("{", &braceIndex))
			{
			break;
			}

		// extract class name -- must be followed by single colon or open brace to be true decl

		JBoolean isClassDecl = kJFalse;
		JIndex endNameIndex  = 1;
		while (endNameIndex < bufLength)
			{
			JCharacter c1 = buffer->GetCharacter(endNameIndex);
			JCharacter c2 = buffer->GetCharacter(endNameIndex+1);
			if (c1 == ' ' || c1 == '{' || (c1 == ':' && c2 != ':'))
				{
				assert( c1 != '{' || endNameIndex == braceIndex );

				if (c1 == ' ' && endNameIndex < bufLength)
					{
					c1 = buffer->GetCharacter(endNameIndex+1);
					}
				if (c1 == ':' || c1 == '{')
					{
					isClassDecl = kJTrue;
					}
				break;
				}
			else if (c1 == ':' && c2 == ':')
				{
				endNameIndex++;		// skip past double colon
				}
			else if (c1 == ';')
				{
				break;
				}

			endNameIndex++;
			}
		endNameIndex--;

		if (isClassDecl && isNamespace)
			{
			JString nsName = buffer->GetSubstring(1, endNameIndex);
			nsName.Prepend(classNamePrefix);
			nsName.Append("::");

			ParseNamespace(nsName, fileID, buffer, braceIndex);
			}
		else if (isClassDecl)
			{
			JString className = buffer->GetSubstring(1, endNameIndex);
			className.Prepend(classNamePrefix);

			CBCClass* newClass = new CBCClass(className, declType, fileID, this);
			assert( newClass != NULL );

			newClass->SetTemplate(isTemplate);

			// If we find a colon before the brace, we need to parse the inheritance.

			JIndex colonIndex = endNameIndex + 1;
			if (buffer->LocateNextSubstring(":", &colonIndex) && colonIndex < braceIndex-1)
				{
				JString inheritance = buffer->GetSubstring(colonIndex+1, braceIndex-1);
				inheritance.TrimWhitespace();
				ParseInheritance(classNamePrefix, newClass, &inheritance);
				}

			// parse functions or enum values

			if (declType == CBClass::kClassType || declType == CBClass::kStructType)
				{
				ParseFunctions(newClass, fileID, buffer, braceIndex);
				}
			else
				{
				assert( declType == CBClass::kEnumType );
				ParseEnumValues(newClass, buffer, braceIndex);
				}
			}
		}
}

/******************************************************************************
 ParseNamespace (private)

	Parse all the classes inside the namespace declaration.

 ******************************************************************************/

void
CBCTree::ParseNamespace
	(
	const JString&	nsName,
	const JFAID_t	fileID,
	JString*		buffer,
	const JIndex	braceIndex
	)
{
	JBoolean hasNestedBraces;
	const JIndex closeBraceIndex =
		FindEndOfBlock(braceIndex, '{', '}', *buffer, &hasNestedBraces);
	if (closeBraceIndex <= braceIndex+1)
		{
		return;
		}

	JString data = buffer->GetSubstring(braceIndex+1, closeBraceIndex-1);
	data.TrimWhitespace();
	ParseClasses(&data, nsName, fileID);

	// We remove it from the main buffer because we don't want to
	// parse contained classes twice.

	const JSize bufLength = buffer->GetLength();
	if (closeBraceIndex < bufLength)
		{
		*buffer = buffer->GetSubstring(closeBraceIndex+1, bufLength);
		buffer->TrimWhitespace();
		}
	else
		{
		buffer->Clear();
		}
}

/******************************************************************************
 ParseInheritance (private)

	Extract the name and access level for each base class.
	This function destroys the data stored in the string.

 ******************************************************************************/

void
CBCTree::ParseInheritance
	(
	const JCharacter*	origNamespace,
	CBCClass*			theClass,
	JString*			inheritance
	)
{
	// remove template arguments so all remaining commas indicate
	// multiple inheritance

	RemoveBalancedBlocks(inheritance, '<', '>');

	// remove "virtual"

	JIndex vkwIndex;
	while (inheritance->LocateSubstring(kVirtualKW, &vkwIndex) &&
		   vkwIndex + kVirtualKWLength <= inheritance->GetLength() &&
		   inheritance->GetCharacter(vkwIndex + kVirtualKWLength) == ' ')
		{
		inheritance->ReplaceSubstring(vkwIndex, vkwIndex + kVirtualKWLength, " ");
		}

	inheritance->TrimWhitespace();

	// parse inheritance

	while (!inheritance->IsEmpty())
		{
		// get type of inheritance (public, protected, private)

		CBClass::InheritType inheritType = CBClass::kInheritPrivate;
		for (JIndex i=0; i<kInheritAccessCount; i++)
			{
			if (inheritance->BeginsWith(kInheritAccess[i].str))
				{
				inheritType = kInheritAccess[i].type;
				inheritance->RemoveSubstring(1, strlen(kInheritAccess[i].str));
				inheritance->TrimWhitespace();
				break;
				}
			}

		// get name of base class

		JString inheritName;
		JIndex endNameIndex;
		if (inheritance->LocateSubstring(",", &endNameIndex))
			{
			inheritName = inheritance->GetSubstring(1, endNameIndex-1);
			inheritance->RemoveSubstring(1, endNameIndex);
			inheritance->TrimWhitespace();
			}
		else
			{
			inheritName = *inheritance;
			inheritance->Clear();
			}
		inheritName.TrimWhitespace();

		// add the parent

		theClass->AddParent(inheritType, inheritName);
		}
}

/******************************************************************************
 ParseFunctions (private)

	Find all the function declarations in the given class.
	The trick is to look for a left parenthesis '(' anywhere
	before the closing brace of the class definition.

	First we have to parse and remove all nested classes, however.
	(Note that is also removes all inline function definitions.)

 ******************************************************************************/

void
CBCTree::ParseFunctions
	(
	CBCClass*		theClass,
	const JFAID_t	fileID,
	JString*		buffer,
	const JIndex	braceIndex
	)
{
	JBoolean hasNestedBraces;
	const JIndex closeBraceIndex =
		FindEndOfBlock(braceIndex, '{', '}', *buffer, &hasNestedBraces);
	if (closeBraceIndex <= braceIndex+1)
		{
		return;
		}

	JString funcDef = buffer->GetSubstring(braceIndex+1, closeBraceIndex-1);
	funcDef.TrimWhitespace();
	if (hasNestedBraces)
		{
		JString newClassPrefix = theClass->GetFullName() + "::";
		JString funcDefCopy    = funcDef;	// gets chewed up
		ParseClasses(&funcDefCopy, newClassPrefix, fileID);
		RemoveBalancedBlocks(&funcDef, '{', '}');
		}

	// We remove it from the main buffer because we don't want to
	// parse nested classes twice.

	const JSize bufLength = buffer->GetLength();
	if (closeBraceIndex < bufLength)
		{
		*buffer = buffer->GetSubstring(closeBraceIndex+1, bufLength);
		buffer->TrimWhitespace();
		}
	else
		{
		buffer->Clear();
		}

	// set the initial access level

	CBClass::FnAccessLevel accessLevel = CBClass::kPrivateAccess;
	if (theClass->GetDeclareType() == CBClass::kStructType)
		{
		accessLevel = CBClass::kPublicAccess;
		}

	JIndex parenIndex;
	while (funcDef.LocateSubstring("(", &parenIndex) &&
		   parenIndex < funcDef.GetLength())
		{
		assert( funcDef.GetCharacter(parenIndex+1) == ')' );
		const JIndex closeParenIndex = parenIndex + 1;

		if (parenIndex == 1)
			{
			cerr << "Non-function-definition parenthesis found:" << endl;
			cerr << funcDef << endl;
			funcDef.RemoveSubstring(1,2);
			continue;
			}

		// check for a change in access level

		accessLevel = ParseFnAccessLevel(accessLevel, funcDef, parenIndex);

		// Find the end of the candidate function definition.

		const JSize length = funcDef.GetLength();
		JString fnPrefix, fnSuffix;
		if (closeParenIndex == length)
			{
			cerr << "Missing semicolon:" << endl;
			cerr << funcDef << endl;
			funcDef.Clear();
			break;	// nothing left
			}
		else
			{
			fnPrefix = funcDef.GetSubstring(1, parenIndex-1);
			fnPrefix.TrimWhitespace();

			funcDef.RemoveSubstring(1, closeParenIndex);
			JIndex semiIndex;
			if (!funcDef.LocateSubstring(";", &semiIndex))
				{
				cerr << "Missing semicolon:" << endl;
				cerr << funcDef << endl;
				continue;
				}
			else if (semiIndex > 1)
				{
				fnSuffix = funcDef.GetSubstring(1, semiIndex-1);
				funcDef.RemoveSubstring(1, semiIndex);

				JIndex colonIndex;
				if (fnSuffix.LocateSubstring(":", &colonIndex))			// constructor initializers
					{
					fnSuffix.RemoveSubstring(colonIndex, fnSuffix.GetLength());
					}

				JIndex throwIndex;
				if (fnSuffix.LocateSubstring("throw()", &throwIndex))	// catch "throw()"
					{
					fnSuffix.RemoveSubstring(throwIndex, throwIndex+6);
					}

				if (fnPrefix.EndsWith(" operator"))		// catch "operator()"
					{
					fnPrefix.Append("()");
					}
				else if (fnSuffix.Contains("()"))		// function pointer declaration
					{
					continue;
					}
				}
			}

		// The word before the parenthesis is the function name.

		JIndexRange fnNameRange;
		if (!fnNameRegex.Match(fnPrefix, &fnNameRange))
			{
			cerr << "Missing function name:" << endl;
			cerr << fnPrefix << endl;
			continue;
			}

		JString funcName = fnPrefix.GetSubstring(fnNameRange);
		funcName.TrimWhitespace();		// "operator X & (" is legal

		const JBoolean pureVirtual = pureVirtualRegex.Match(fnSuffix);

		// Add the function to the class

		theClass->AddFunction(funcName, accessLevel, !pureVirtual);
		}
}

/******************************************************************************
 ParseFnAccessLevel

	If there is a function access keyword before endIndex, return the
	result of the last one.  Otherwise, return the current level.

	JAFL 1/9/98:
		Added line to set maxKWIndex inside loop.

	JAFL 4/2/98:
		Changed to LocatePrevSubstring() to catch -last- one of each.

 ******************************************************************************/

CBClass::FnAccessLevel
CBCTree::ParseFnAccessLevel
	(
	const CBClass::FnAccessLevel	currLevel,
	const JString&					buffer,
	const JIndex					endIndex
	)
{
	CBClass::FnAccessLevel newLevel = currLevel;

	JIndex maxKWIndex = 0;
	for (JIndex i=0; i<kFnAccessCount; i++)
		{
		JIndex kwIndex = endIndex;
		if (buffer.LocatePrevSubstring(kFnAccess[i].str, &kwIndex) &&
			kwIndex > maxKWIndex)
			{
			maxKWIndex = kwIndex;
			newLevel   = kFnAccess[i].access;
			}
		}

	return newLevel;
}

/******************************************************************************
 ParseEnumValues

	Find all the values defined in the enum.

 ******************************************************************************/

void
CBCTree::ParseEnumValues
	(
	CBCClass*		theClass,
	JString*		buffer,
	const JIndex	braceIndex
	)
{
	const JCharacter delimStr[] = " =,}";
	const JSize delimCount      = strlen(delimStr);

	const JCharacter delim1Str[] = ",}";
	const JSize delim1Count      = strlen(delim1Str);

	const std::string s(*buffer);
	std::istringstream input(s);
	JSeekg(input, braceIndex+1);
	while (!input.eof() && !input.fail())
		{
		JString valueName;
		JCharacter delim;
		input >> ws;
		if (JReadUntil(input, delimCount, delimStr, &valueName, &delim))
			{
			theClass->AddFunction(valueName, CBClass::kPublicAccess, kJTrue);
			if (delim == ' ' || delim == '=')
				{
				delim = '}';
				JIgnoreUntil(input, delim1Count, delim1Str, &delim);
				}
			if (delim == '}')	// catch delim from JReadUntil() and JIgnoreUntil()
				{
				break;
				}
			}
		else
			{
			cerr << "Unclosed enum found:" << endl;
			cerr << buffer << endl;
			break;
			}
		}
}

/******************************************************************************
 ReadHeaderFile (private)

	Return the filtered contents of the file as a JString.

	7/11/2000:  Don't toss newline after a construct because otherwise the
				preprocessor code will get confused.

 ******************************************************************************/

static const JRegex wsRegex = " {2,}";

void
CBCTree::ReadHeaderFile
	(
	const JCharacter*		fileName,
	JString*				buffer
	)
{
	ifstream input(fileName);
	input >> ws;

	buffer->SetBlockSize(JGetFStreamLength(input));

	JSize parenCount = 0;

	JCharacter c1 = '\n';	// catch preprocessor on first line
	while (1)
		{
		JCharacter c2 = input.peek();
		if (c2 == '\r')
			{
			c2 = '\n';
			}

	// C++ style comment -- toss rest of line

		if (c1 == '/' && c2 == '/')
			{
			JIgnoreUntil(input, 2, "\n\r");
			input.unget();
			}

	// C style comment -- toss until */ since not nestable

		else if (c1 == '/' && c2 == '*')
			{
			JIgnoreUntil(input, "*/");
			}

	// string constant

		else if (c1 == '\"')
			{
			do
				{
				c1 = input.get();
				if (c1 == '\\')
					{
					input.ignore();
					}
				}
				while (!input.eof() && !input.fail() &&
					   c1 != '\"' && c1 != '\n' && c1 != '\r');
				// newline is err, but we still quit

				if (isspace(c1))
					{
					input.unget();
					}
			}

	// character constant

		else if (c1 == '\'')
			{
			do
				{
				c1 = input.get();
				if (c1 == '\\' &&
					input.peek() != '\n' && input.peek() != '\r')
					{
					input.ignore();
					}
				}
				while (!input.eof() && !input.fail() &&
					   c1 != '\'' && c1 != '\n' && c1 != '\r');
				// newline is err, but we still quit

				if (isspace(c1))
					{
					input.unget();
					}
			}

	// function arguments -- convert to "()"

		else if (c1 == '(')
			{
			if (parenCount == 0)
				{
				buffer->TrimWhitespace();		// no space in front of paren makes parsing easier
				*buffer += "(";
				}
			parenCount++;
			}
		else if (c1 == ')' && parenCount > 0)
			{
			parenCount--;
			if (parenCount == 0)
				{
				*buffer += ")";
				}
			}

	// append character if not part of function argument

		else if (parenCount == 0)
			{
			JCharacter c3 = c1;
			if (isspace(c3))				// convert all whitespace to single space
				{
				c3 = ' ';
				}
			if (c3 != ' ' ||
				(!buffer->IsEmpty() && buffer->GetLastCharacter() != ' '))
				{
				buffer->AppendCharacter(c3);
				}

			// make parsing easier

			if (c1 == '}')
				{
				*buffer += "; ";
				}
			else if (c1 == ';' || c1 == '>')
				{
				*buffer += " ";
				}

			while (c1 == '\n')				// preprocessor code
				{
				input >> ws;
				if (!input.eof() && !input.fail() && input.peek() == '#')
					{
					JString line;
					do
						{
						JReadUntil(input, 2, "\n\r", &line);
						}
						while (!line.IsEmpty() && line.GetLastCharacter() == '\\');
					}
				else
					{
					c1 = ' ';	// break out of loop
					}
				}
			}

		// read next character here so we can set it independently when we
		// first enter the loop

		c1 = input.get();
		if (input.eof() || input.fail())
			{
			break;
			}
		if (c1 == '\r')
			{
			c1 = '\n';
			}
		}

	// replace preprocessor macros

	const JBoolean replaced = itsCPP->Preprocess(buffer);

	// maintain the invariant that all whitespace collapses to a single space

	buffer->TrimWhitespace();
	if (replaced)
		{
		JIndexRange wsRange, newRange;
		while (wsRegex.MatchAfter(*buffer, wsRange, &wsRange))
			{
			buffer->ReplaceSubstring(wsRange, " ", &newRange);
			}
		}

	// for debugging
/*
	JString outputName = fileName;
	outputName += ".slurped";
	ofstream output(outputName);
	buffer->Print(output);
*/
}

/******************************************************************************
 FindEndOfBlock (private)

	Starting at blockStartIndex, balance the given items until the overall closing
	item is found.  Returns the index of the closing item.

	*** The closing item is assumed to exist.

 ******************************************************************************/

JIndex
CBCTree::FindEndOfBlock
	(
	const JIndex		blockStartIndex,
	const JCharacter	openBlockChar,
	const JCharacter	closeBlockChar,
	const JString&		buffer,
	JBoolean*			hasNestedBlocks
	)
{
	const JSize bufLength = buffer.GetLength();
	assert( blockStartIndex < bufLength );

	JSize itemCount = 1;
	*hasNestedBlocks = kJFalse;
	for (JIndex i=blockStartIndex+1; i<=bufLength; i++)
		{
		const JCharacter c = buffer.GetCharacter(i);
		if (c == openBlockChar)
			{
			itemCount++;
			*hasNestedBlocks = kJTrue;
			}
		else if (c == closeBlockChar)
			{
			itemCount--;
			}

		if (itemCount == 0)
			{
			return i;
			}
		}

	// There has to be a closing item, but there could be more than one
	// open because of #if clauses.

	return bufLength;
}

/******************************************************************************
 RemoveBalancedBlocks (private)

	Removes all pairs of the given tokens and everything enclosed by them.

 ******************************************************************************/

void
CBCTree::RemoveBalancedBlocks
	(
	JString*			buffer,
	const JCharacter	openChar,
	const JCharacter	closeChar
	)
{
	JSize openCount   = 0;
	JIndex startIndex = 0;

	JSize bufLength = buffer->GetLength();
	for (JIndex i=1; i<=bufLength; i++)
		{
		const JCharacter c = buffer->GetCharacter(i);
		if (c == openChar)
			{
			if (openCount == 0)
				{
				startIndex = i;
				}
			openCount++;
			}
		else if (c == closeChar)
			{
			openCount--;
			if (openCount == 0)
				{
				buffer->RemoveSubstring(startIndex, i);
				bufLength = buffer->GetLength();
				i = startIndex;
				}
			}
		}
}
