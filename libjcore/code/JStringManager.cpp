/******************************************************************************
 JStringManager.cpp

	Stores a hash table of strings for use by the program.  Each string is
	associated with a unique id, which is also a string.  ID's typically
	have the form "name::class".

	Every application must register a unique signature.  Each registered
	directory is searched for the file <signature>_$LANG and then
	<signature>.  The first one that is found is loaded into the hash table.

	Each data file has the format:

		file format (ASCII digits followed by ASCII newline)
		# comment
		id "string"

	The comment ends at the end of the line.  Inside the string, quotes and
	backslashes must be preceded by a backslash.

	Certain strings cannot be overridden because tampering with them
	would be considered illegal.  These include VERSION, COPYRIGHT, and
	LICENSE.

	Menus present a special case.  If one puts the entire menu definition
	into the database as a single item, then an outdated database file may
	have the wrong number of menu items.  The safe solution is to store
	each item's text as a separate string, keyed to the ID of the menu item.

	Pseudotranslation is useful for testing layout expansion and for
	catching string concatenation (which should instead be done via
	parameter insertion).

	BASE CLASS = public JStringPtrMap<JString>

	Copyright (C) 2000 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JStringManager.h>
#include <JStringPtrMapCursor.h>
#include <JSubstitute.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <fstream>
#include <strstream>
#include <jAssert.h>

static const JCharacter* kDataDirName             = "string_data";
JBoolean JStringManager::thePseudotranslationFlag = kJFalse;

// non-overridable strings

static const JCharacter* kNoOverrideID[] =
{
	"VERSION",
	"COPYRIGHT",
	"LICENSE"
};

const JSize kNoOverrideIDCount = sizeof(kNoOverrideID) / sizeof(JCharacter*);

/******************************************************************************
 Constructor

 *****************************************************************************/

JStringManager::JStringManager()
	:
	// does kDeleteAll make quitting too slow? -- apparently not
	JStringPtrMap<JString>(JPtrArrayT::kDeleteAll)
{
	itsReplaceEngine = jnew JSubstitute;
	assert( itsReplaceEngine != NULL );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JStringManager::~JStringManager()
{
	jdelete itsReplaceEngine;
}

/******************************************************************************
 Get

 *****************************************************************************/

static const JString theMissingString = "<string not found>";

const JString&
JStringManager::Get
	(
	const JCharacter* id
	)
	const
{
	const JString* s;
	const JBoolean found = GetElement(id, &s);
	if (found)
		{
		assert( s != NULL );
		}
	else
		{
		s = &theMissingString;
		}
	return *s;
}

/******************************************************************************
 ReportError

	Replaces $err in the specified string with the error message and
	calls JUserNotification::ReportError().

	This is more helpful to the user than calling JError::ReportError()

 *****************************************************************************/

void
JStringManager::ReportError
	(
	const JCharacter*	id,
	const JError&		err
	)
	const
{
	if (!err.OK())
		{
		ReportError(id, err.GetMessage());
		}
}

void
JStringManager::ReportError
	(
	const JCharacter*	id,
	const JCharacter*	message
	)
	const
{
	const JCharacter* map[] =
		{
		"err", message
		};
	const JString s = Get(id, map, sizeof(map));
	(JGetUserNotification())->ReportError(s);
}

/******************************************************************************
 Get

	Uses the given associative array to replace $ variables in the
	specified string and then returns the result.  'size' must be the
	result of sizeof(map).

 *****************************************************************************/

JString
JStringManager::Get
	(
	const JCharacter*	id,
	const JCharacter*	map[],
	const JSize			size
	)
	const
{
	JString s = Get(id);
	Replace(&s, map, size);
	return s;
}

/******************************************************************************
 GetReplaceEngine

	Resets the replace engine and returns a pointer to it so clients
	can use it.

 *****************************************************************************/

JSubstitute*
JStringManager::GetReplaceEngine()
	const
{
	itsReplaceEngine->Reset();
	itsReplaceEngine->IgnoreUnrecognized(kJTrue);
	return itsReplaceEngine;
}

/******************************************************************************
 Replace

	Uses the given associative array to replace $ variables in the given
	string.  'size' must be the result of sizeof(map).

 *****************************************************************************/

void
JStringManager::Replace
	(
	JString*			str,
	const JCharacter*	map[],
	const JSize			size
	)
	const
{
	JSubstitute* r = GetReplaceEngine();

	const JSize count = size/(2*sizeof(JCharacter*));
	for (JIndex i=0; i<count; i++)
		{
		r->DefineVariable(map[2*i], map[2*i+1]);
		}

	r->Substitute(str);
}

/******************************************************************************
 Register

	We have to merge the defaults first in case the file is outdated or does
	not include all necessary strings.

	defaultDataSize should be the result of sizeof().

 *****************************************************************************/

void
JStringManager::Register
	(
	const JCharacter*	signature,
	const JCharacter**	defaultData
	)
{
	JString tempFileName;
	const JError err = JCreateTempFile(&tempFileName);
	if (!err.OK())
		{
		return;
		}

	{
	std::ofstream tempFile(tempFileName);

	JIndex i = 0;
	while (defaultData[i] != NULL)
		{
		tempFile.write(defaultData[i], strlen(defaultData[i]));
		i++;
		}
	}
	MergeFile(tempFileName);
	JRemoveFile(tempFileName);

	if (!JStringEmpty(signature))
		{
		const JCharacter* lang = getenv("LANG");

		JString path[2];
		JGetJDataDirectories(kDataDirName, path, path+1);

		JString name1, name2;
		for (JIndex i=0; i<2; i++)
			{
			if (!path[i].IsEmpty() &&
				(JDirectoryReadable(path[i]) ||
				 JCreateDirectory(path[i]) == kJNoError))
				{
				name1 = JCombinePathAndName(path[i], signature);

				if (lang != NULL)
					{
					name2 = name1;
					name2.AppendCharacter('_');
					name2.Append(lang);

					if (MergeFile(name2))
						{
						break;
						}
					}

				if (MergeFile(name1))
					{
					break;
					}
				}
			}
		}
}

/******************************************************************************
 MergeFile

	If debug, prints every string ID that it finds to stdout.

 *****************************************************************************/

JBoolean
JStringManager::MergeFile
	(
	const JCharacter*	fileName,
	const JBoolean		debug
	)
{
	std::ifstream input(fileName);
	if (input.good())
		{
		MergeFile(input, debug);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

void
JStringManager::MergeFile
	(
	std::istream&	input,
	const JBoolean	debug
	)
{
	JUInt format;
	input >> format;
	if (format != kASCIIFormat)
		{
		std::cerr << "Invalid string file format: " << format << std::endl;
		return;
		}

	JString id;
	while (1)
		{
		input >> std::ws;
		while (!input.eof() && input.peek() == '#')		// peek() at eof sets fail()
			{
			JIgnoreLine(input);
			input >> std::ws;
			}
		if (input.eof() || input.fail())
			{
			break;
			}

		id = JReadUntilws(input);
		if (debug)
			{
			std::cout << id << std::endl;
			}
		if (input.eof() || input.fail())
			{
			break;
			}

		JString* s = jnew JString;
		assert( s != NULL );

		input >> *s;
		if (input.eof() || input.fail())
			{
			jdelete s;
			break;
			}

		if (CanOverride(id))
			{
			if (thePseudotranslationFlag)
				{
				Pseudotranslate(s);
				}
			SetElement(id, s, JPtrArrayT::kDelete);
			}
		else if (!SetNewElement(id, s))
			{
			jdelete s;
			}
		}
}

/******************************************************************************
 CanOverride (static)

 *****************************************************************************/

JBoolean
JStringManager::CanOverride
	(
	const JCharacter* id
	)
{
	for (JIndex i=0; i<kNoOverrideIDCount; i++)
		{
		if (JStringCompare(id, kNoOverrideID[i]) == 0)
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 WriteFile

 *****************************************************************************/

void
JStringManager::WriteFile
	(
	std::ostream& output
	)
	const
{
	output << (long) kASCIIFormat << std::endl;

	JStringPtrMapCursor<JString> cursor(const_cast<JStringManager*>(this));
	while (cursor.Next())
		{
		output << cursor.GetKey();
		output << ' ' << *(cursor.GetValue()) << std::endl;
		}
}

/******************************************************************************
 Pseudotranslate (static private)

 *****************************************************************************/

static const JCharacter* kPseudotranslatePrefix = "[\xE6\x96\x87\xE5\xAD\x97";
static const JCharacter* kPseudotranslateSuffix = "??]";

struct Pseudotranslation
{
	const JCharacter  c;
	const JCharacter* s;
};

static const Pseudotranslation kPseudotranslateList[] =
{
	{ 'A', "\xC3\x85" },
	{ 'a', "\xC3\xA5" },
	{ 'E', "\xC3\x89" },
	{ 'e', "\xC3\xA9" },
	{ 'I', "\xC3\x8E" },
	{ 'i', "\xC3\xAE" },
	{ 'O', "\xC3\x98" },
	{ 'o', "\xC3\xB8" },
	{ 'U', "\xC3\x9C" },
	{ 'u', "\xC3\xBC" },
	{ 'C', "\xC3\x87" },
	{ 'c', "\xC3\xA7" },
	{ 'G', "\xC4\x9C" },
	{ 'g', "\xC4\x9D" },
	{ 'K', "\xC6\x98" },
	{ 'k', "\xC6\x99" },
	{ 'N', "\xC3\x91" },
	{ 'n', "\xC3\xB1" },
	{ 'R', "\xD0\xAF" },
	{ 'r', "\xD1\x8F" },
	{ 'S', "\xC5\xA0" },
	{ 's', "\xC5\xA1" },
	{ 'U', "\xC3\x9C" },
	{ 'u', "\xC3\xBC" },
	{ 'W', "\xE1\xBA\x80" },
	{ 'w', "\xE1\xBA\x81" },
	{ '$', "\xC2\xA4" },
	{ '.', "\xEF\xBD\xA1" },
	{ '?', "\xC2\xBF" }
};

const JSize kPseudotranslateCount = sizeof(kPseudotranslateList)/sizeof(Pseudotranslation);

void
JStringManager::Pseudotranslate
	(
	JString* s
	)
{
	for (JIndex i = s->GetLength(); i>=1; i--)
		{
		const JCharacter c = s->GetCharacter(i);
		for (JIndex j=0; j<kPseudotranslateCount; j++)
			{
			if (c == kPseudotranslateList[j].c)
				{
//				s->InsertSubstring(kPseudotranslateList[j].s, i+1);
				s->InsertCharacter(kPseudotranslateList[j].c, i+1);
				}
			}
		}

	s->Prepend(kPseudotranslatePrefix);
	s->Append(kPseudotranslateSuffix);
}
