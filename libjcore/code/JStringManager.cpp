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

	BASE CLASS = public JStringPtrMap<JString>

	Copyright (C) 2000 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JStringManager.h>
#include <JStringPtrMapCursor.h>
#include <JSubstitute.h>
#include <JStringIterator.h>
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <fstream>
#include <strstream>
#include <jAssert.h>

static const JString theDataDirName("string_data", 0, kJFalse);

// non-overridable strings

static const JUtf8Byte* kNoOverrideID[] =
{
	"VERSION",
	"COPYRIGHT",
	"LICENSE"
};

const JSize kNoOverrideIDCount = sizeof(kNoOverrideID) / sizeof(JUtf8Byte*);

/******************************************************************************
 Constructor

 *****************************************************************************/

JStringManager::JStringManager()
	:
	// does kDeleteAll make quitting too slow?
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

	We assert that the id exists because it's a programmer error otherwise.
	If you don't want this behavior, use GetElement().

 *****************************************************************************/

static const JString theMissingString("<string not found>", 0, kJFalse);

const JString&
JStringManager::Get
	(
	const JUtf8Byte* id
	)
	const
{
	const JString* s;
	if (GetElement(JString(id, 0, kJFalse), &s))
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
	const JUtf8Byte*	map[],
	const JSize			size
	)
	const
{
	JSubstitute* r = GetReplaceEngine();

	const JSize count = size/(2*sizeof(JUtf8Byte*));
	for (JIndex i=0; i<count; i++)
		{
		r->DefineVariable(map[2*i], JString(map[2*i+1], 0, kJFalse));
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
	while (defaultData[i] != NULL)
		{
		tempFile.write(defaultData[i], strlen(defaultData[i]));
		i++;
		}
	}
	MergeFile(tempFileName);
	JRemoveFile(tempFileName);

	if (!JString::IsEmpty(signature))
		{
		JString locale(getenv("LANG"), 0);

		// remove character set

		JStringIterator iter(&locale);
		if (iter.Next("."))
			{
			iter.SkipPrev();
			iter.RemoveAllNext();
			}

		// split locale into language & country

		JPtrArray<JString> localeParts(JPtrArrayT::kDeleteAll);
		locale.Split("_", &localeParts);

		const JString* language = localeParts.GetElement(1);

		JString path[2];
		JGetDataDirectories(JString(signature, 0, kJFalse), theDataDirName, path, path+1);

		// merge system first, then user

		JString name;
		for (JIndex i=0; i<2; i++)
			{
			if (!path[i].IsEmpty() && JDirectoryReadable(path[i]))
				{
				name = JCombinePathAndName(path[i], locale);
				if (MergeFile(name))
					{
					continue;
					}

				name = JCombinePathAndName(path[i], *language);
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

JBoolean
JStringManager::MergeFile
	(
	const JString&	fileName,
	const JBoolean	debug
	)
{
	std::ifstream input(fileName.GetBytes());
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
	std::istream&		input,
	const JBoolean	debug
	)
{
	JUInt format;
	input >> format;
	if (format != kASCIIFormat)
		{
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
	const JString& id
	)
{
	for (JIndex i=0; i<kNoOverrideIDCount; i++)
		{
		if (id == kNoOverrideID[i])
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
		cursor.GetKey().Print(output);
		output << ' ' << *(cursor.GetValue()) << std::endl;
		}
}
