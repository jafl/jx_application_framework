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

	Copyright © 2000 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
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

static const JCharacter* kDataDirName = "string_data";

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
	// does kDeleteAll make quitting too slow?
	JStringPtrMap<JString>(JPtrArrayT::kDeleteAll)
{
	itsReplaceEngine = new JSubstitute;
	assert( itsReplaceEngine != NULL );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JStringManager::~JStringManager()
{
	delete itsReplaceEngine;
}

/******************************************************************************
 Get

	We assert that the id exists because it's a programmer error otherwise.
	If you don't want this behavior, use GetElement().

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
	istream&		input,
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
		input >> ws;
		while (!input.eof() && input.peek() == '#')		// peek() at eof sets fail()
			{
			JIgnoreLine(input);
			input >> ws;
			}
		if (input.eof() || input.fail())
			{
			break;
			}

		id = JReadUntilws(input);
		if (debug)
			{
			cout << id << endl;
			}
		if (input.eof() || input.fail())
			{
			break;
			}

		JString* s = new JString;
		assert( s != NULL );

		input >> *s;
		if (input.eof() || input.fail())
			{
			delete s;
			break;
			}

		if (CanOverride(id))
			{
			SetElement(id, s, JPtrArrayT::kDelete);
			}
		else if (!SetNewElement(id, s))
			{
			delete s;
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
	ostream& output
	)
	const
{
	output << (long) kASCIIFormat << endl;

	JStringPtrMapCursor<JString> cursor(const_cast<JStringManager*>(this));
	while (cursor.Next())
		{
		output << cursor.GetKey();
		output << ' ' << *(cursor.GetValue()) << endl;
		}
}
