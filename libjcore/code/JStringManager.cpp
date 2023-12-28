/******************************************************************************
 JStringManager.cpp

	Stores a hash table of strings for use by the program.  Each string is
	associated with a unique id, which is also a string.  ID's typically
	have the form "name::class".

	Every application must register a unique signature.  This is used to search
	for translation files:

	1a. /usr/lib/<signature>/string_data/<locale>
	1b. /usr/lib/<signature>/string_data/<language>
	1c. /usr/lib/<signature>/string_data/default
	2a. ~/.<signature>/string_data/<locale>
	2b. ~/.<signature>/string_data/<language>
	2c. ~/.<signature>/string_data/default

	The ones that are found are loaded into the hash table in the order shown
	to support overrides.

	Each data file has the format:

		file format (ASCII digits followed by ASCII newline)
		# comment
		id "string"
		...

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

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#include "JStringManager.h"
#include "JStringPtrMapCursor.h"
#include "JSubstitute.h"
#include "JStringIterator.h"
#include "jDirUtil.h"
#include "jStreamUtil.h"
#include "jGlobals.h"
#include <fstream>
#include "jAssert.h"

static const JUtf8Byte* kDataDirName = "string_data";
static const JString kDefaultFileName("default", JString::kNoCopy);
bool JStringManager::thePseudotranslationFlag = false;

// non-overridable strings

static const JUtf8Byte* kNoOverrideID[] =
{
	"VERSION",
	"COPYRIGHT",
	"LICENSE"
};

/******************************************************************************
 Constructor

 *****************************************************************************/

JStringManager::JStringManager()
	:
	// does kDeleteAll make quitting too slow? -- apparently not
	JStringPtrMap<JString>(JPtrArrayT::kDeleteAll),
	itsBCP47Locale("en-US")		// need something as default, until we load
{
	itsReplaceEngine = jnew JSubstitute;
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

static const JUtf8Byte* theMissingStringKey = "StringNotFound::JStringManager";
static const JString theMissingString("<string not found>", JString::kNoCopy);

const JString&
JStringManager::Get
	(
	const JUtf8Byte* id
	)
	const
{
	const JString* s;
	if (GetItem(JString(id, JString::kNoCopy), &s))
	{
		assert( s != nullptr );
	}
	else if (Contains(theMissingStringKey))	// ok to leak memory, because it should never happen
	{
		std::cerr << "missing string id: " << id << std::endl;

		const JUtf8Byte* map[] =
		{
			"s", id
		};
		s = jnew JString(Get(theMissingStringKey, map, sizeof(map)));
		assert( s != nullptr );
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
	const JUtf8Byte*	id,
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
	const JUtf8Byte*	id,
	const JString&		message
	)
	const
{
	const JUtf8Byte* map[] =
	{
		"err", message.GetBytes()
	};
	const JString s = Get(id, map, sizeof(map));
	JGetUserNotification()->ReportError(s);
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
	const JUtf8Byte*	id,
	const JUtf8Byte*	map[],
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
	itsReplaceEngine->IgnoreUnrecognized(true);
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
	const JUtf8Byte*	map[],
	const JSize			size
	)
	const
{
	JSubstitute* r = GetReplaceEngine();

	const JSize count = size/(2*sizeof(JUtf8Byte*));
	for (JUnsignedOffset i=0; i<count; i++)
	{
		r->DefineVariable(map[2*i], JString(map[2*i+1], JString::kNoCopy));
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
	const JUtf8Byte*	signature,
	const JUtf8Byte**	defaultData
	)
{
	JString tempFileName;
	const JError err = JCreateTempFile(&tempFileName);
	if (!err.OK())
	{
		return;
	}

	{
	std::ofstream tempFile(tempFileName.GetBytes());

	JIndex i = 0;
	while (defaultData[i] != nullptr)
	{
		tempFile.write(defaultData[i], strlen(defaultData[i]));
		i++;
	}
	}
	MergeFile(tempFileName);
	JRemoveFile(tempFileName);

	if (!JString::IsEmpty(signature))
	{
		JString locale(getenv("LANG"));
		if (locale.IsEmpty())
		{
			locale = "en_US";
		}

		// remove character set

		JStringIterator iter(&locale);
		if (iter.Next("."))
		{
			iter.SkipPrev();
			iter.RemoveAllNext();
		}
		iter.Invalidate();

		// split locale into language & country

		JPtrArray<JString> localeParts(JPtrArrayT::kDeleteAll);
		locale.Split("_", &localeParts);

		itsBCP47Locale = *localeParts.GetItem(1);
		itsBCP47Locale.Append("-");
		itsBCP47Locale += *localeParts.GetItem(2);

		const JString& language = *localeParts.GetItem(1);

		JString path[2];
		JGetDataDirectories(signature, kDataDirName, path, path+1);

		// merge system first, then user

		JString name;
		for (const auto& p : path)
		{
			if (!p.IsEmpty() && JDirectoryReadable(p))
			{
				name = JCombinePathAndName(p, locale);
				if (MergeFile(name))
				{
					continue;
				}

				name = JCombinePathAndName(p, language);
				if (MergeFile(name))
				{
					continue;
				}

				name = JCombinePathAndName(p, kDefaultFileName);
				if (MergeFile(name))
				{
					continue;
				}
			}
		}
	}
}

/******************************************************************************
 MergeFile

	If debug, prints every string ID that it finds to stdout.

 *****************************************************************************/

bool
JStringManager::MergeFile
	(
	const JString&	fileName,
	const bool		debug
	)
{
	std::ifstream input(fileName.GetBytes());
	if (!input.good())
	{
		return false;
	}

	if (!MergeFile(input, debug))
	{
		std::cerr << "Unable to load " << fileName << std::endl;
		return false;
	}

	return true;
}

bool
JStringManager::MergeFile
	(
	std::istream&	input,
	const bool		debug
	)
{
	JUInt format;
	input >> format;
	if (format != kASCIIFormat && format != kUTF8Format)
	{
		std::cerr << "Invalid string file format: " << format << std::endl;
		return false;
	}

	JString id;
	while (true)
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

		auto* s = jnew JString;

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
			SetItem(id, s, JPtrArrayT::kDelete);
		}
		else if (!SetNewItem(id, s))
		{
			jdelete s;
		}
	}

	return true;
}

/******************************************************************************
 CanOverride (static)

 *****************************************************************************/

bool
JStringManager::CanOverride
	(
	const JString& id
	)
{
	for (auto s : kNoOverrideID)
	{
		if (id == s)
		{
			return false;
		}
	}

	return true;
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
	JStringPtrMapCursor<JString> cursor(const_cast<JStringManager*>(this));

	bool ascii = true;
	while (cursor.Next())
	{
		if (!cursor.GetValue()->IsAscii())
		{
			ascii = false;
			break;
		}
	}

	output << (long) (ascii ? kASCIIFormat: kUTF8Format) << std::endl;

	cursor.Reset();
	while (cursor.Next())
	{
		cursor.GetKey().Print(output);
		output << ' ' << *(cursor.GetValue()) << std::endl;
	}
}

/******************************************************************************
 Pseudotranslate (static private)

 *****************************************************************************/

static const JUtf8Byte* kPseudotranslatePrefix = "[\xE6\x96\x87\xE5\xAD\x97";
static const JUtf8Byte* kPseudotranslateSuffix = "??]";

struct Pseudotranslation
{
	const JUtf8Byte  c;
	const JUtf8Byte* s;
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
{ 'W', "\xE1\xBA\x80" },
{ 'w', "\xE1\xBA\x81" },
{ '$', "\xC2\xA4" },
{ '.', "\xEF\xBD\xA1" },
{ '?', "\xC2\xBF" }
};

void
JStringManager::Pseudotranslate
	(
	JString* s
	)
{
	JStringIterator iter(s);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		for (const auto& t : kPseudotranslateList)
		{
			if (c == t.c)
			{
				iter.Insert(t.s);
				iter.SkipNext();
			}
		}
	}

	s->Prepend(kPseudotranslatePrefix);
	s->Append(kPseudotranslateSuffix);
}
