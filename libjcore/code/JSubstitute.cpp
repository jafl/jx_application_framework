/******************************************************************************
 JSubstitute.cpp

	JSubstitute replaces escaped characters like \a and variables of the
	form $name in a JString.  "name" can either be a literal, in which case
	we store the value and perform the replacement, or it can be a regular
	expression, in which case we call the virtual function GetValue().  An
	example of the latter case is [+-]?[0-9]+, which is used in regular
	expression replace patterns to denote submatches.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JSubstitute.h>
#include <JRegex.h>
#include <jAssert.h>

const JSize kEscapeCount = 256;

// JError messages

const JCharacter* JSubstitute::kLoneDollar         = "LoneDollar::JSubstitute";
const JCharacter* JSubstitute::kTrailingBackslash  = "TrailingBackslash::JSubstitute";
const JCharacter* JSubstitute::kIllegalControlChar = "IllegalControlChar::JSubstitute";

/******************************************************************************
 Constructor

 *****************************************************************************/

JSubstitute::JSubstitute()
{
	itsControlEscapesFlag     = kJFalse;
	itsIgnoreUnrecognizedFlag = kJFalse;
	itsPureEscapeEngineFlag   = kJFalse;

	AllocateInternals();

	for (JIndex i=0; i<kEscapeCount; i++)
		{
		itsEscapeTable[i] = NULL;
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JSubstitute::JSubstitute
	(
	const JSubstitute& source
	)
{
	AllocateInternals();
	CopyInternals(source);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JSubstitute::~JSubstitute()
{
	ClearAllEscapes();
	delete [] itsEscapeTable;

	UndefineAllVariables();
	delete itsVarList;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JSubstitute&
JSubstitute::operator=
	(
	const JSubstitute& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	ClearAllEscapes();
	UndefineAllVariables();
	CopyInternals(source);
	return *this;
}

/******************************************************************************
 AllocateInternals (private)

 ******************************************************************************/

void
JSubstitute::AllocateInternals()
{
	itsEscapeTable = new JString* [ kEscapeCount ];
	assert(itsEscapeTable != NULL);

	itsVarList = new JArray<VarInfo>;
	assert( itsVarList != NULL );
}

/******************************************************************************
 CopyInternals (private)

 ******************************************************************************/

void
JSubstitute::CopyInternals
	(
	const JSubstitute& source
	)
{
JIndex i;

	assert( itsEscapeTable != NULL && itsVarList != NULL && itsVarList->IsEmpty() );

	for (i=0; i<kEscapeCount; i++)
		{
		if (source.itsEscapeTable[i] != NULL)
			{
			itsEscapeTable[i] = new JString(*(source.itsEscapeTable[i]));
			assert( itsEscapeTable[i] != NULL );
			}
		else
			{
			itsEscapeTable[i] = NULL;
			}
		}

	const JSize varCount = (source.itsVarList)->GetElementCount();
	VarInfo newInfo;
	for (i=1; i<=varCount; i++)
		{
		const VarInfo origInfo = (source.itsVarList)->GetElement(i);

		newInfo.name = new JString(*(origInfo.name));
		assert( newInfo.name != NULL );

		if (origInfo.regex != NULL)
			{
			newInfo.regex = new JRegex(*(origInfo.regex));
			assert( newInfo.regex != NULL );
			}

		if (origInfo.value != NULL)
			{
			newInfo.value = new JString(*(origInfo.value));
			assert( newInfo.value != NULL );
			}

		itsVarList->AppendElement(newInfo);
		}

	itsControlEscapesFlag     = source.itsControlEscapesFlag;
	itsIgnoreUnrecognizedFlag = source.itsIgnoreUnrecognizedFlag;
	itsPureEscapeEngineFlag   = source.itsPureEscapeEngineFlag;
}

/******************************************************************************
 SetEscape

	Changes the value of the 'character' escape to the given value.  Returns
	kJTrue if 'character' already had a value, kJFalse otherwise.

 *****************************************************************************/

JBoolean
JSubstitute::SetEscape
	(
	const unsigned char	c,
	const JString&		value
	)
{
	return SetEscape(c, value.GetCString(), value.GetLength());
}

JBoolean
JSubstitute::SetEscape
	(
	const unsigned char	c,
	const JCharacter*	value
	)
{
	return SetEscape(c, value, strlen(value));
}

JBoolean
JSubstitute::SetEscape
	(
	const unsigned char	c,
	const JCharacter*	value,
	const JSize			length
	)
{
	if (itsEscapeTable[c] != NULL)
		{
		(itsEscapeTable[c])->Set(value, length);
		return kJTrue;
		}
	else
		{
		itsEscapeTable[c] = new JString(value, length);
		assert( itsEscapeTable[c] != NULL );
		return kJFalse;
		}
}

/******************************************************************************
 ClearEscape

	Clears any value for the given character and returns kJTrue, if it exists,
	otherwise returns kJFalse.

 *****************************************************************************/

JBoolean
JSubstitute::ClearEscape
	(
	const unsigned char c
	)
{
	if (itsEscapeTable[c] != NULL)
		{
		delete itsEscapeTable[c];
		itsEscapeTable[c] = NULL;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ClearAllEscapes

	Clears all ordinary escape bindings.

 *****************************************************************************/

void
JSubstitute::ClearAllEscapes()
{
	for (JIndex i=0; i<kEscapeCount; i++)
		{
		delete itsEscapeTable[i];
		itsEscapeTable[i] = NULL;
		}
}

/******************************************************************************
 Nonprinting escapes

	Adds entries to the table corresponding to standard escapes for certain
	non-printing characters.  The escapes and their values are:

		\a   bell
		\b   backspace
		\e   escape

	The numeric values for \a and \b are those chosen by the compiler for
	those escape sequences; \e is not a C escape (though it appears in
	Perl) and has the value 1B hex.

	Note that ANSI does not define what happens if you backslash a
	character other than these or an octal or hex code, but you do.  If you
	want other backslashed characters to represent themselves (so that the
	backslash is effectively removed) call SetIgnoreUnrecognized() first.  If
	you want no changes to be made other than those listed here, call
	SetIgnoreUnrecognized(kJFalse).

 *****************************************************************************/

void
JSubstitute::SetNonprintingEscapes()
{
	SetEscape('a', "\a");
	SetEscape('b', "\b");
	SetEscape('e', "\x1B");
}

void
JSubstitute::ClearNonprintingEscapes()
{
	ClearEscape('a');
	ClearEscape('b');
	ClearEscape('e');
}

/******************************************************************************
 Whitespace escapes

	Adds entries to the table corresponding to the standard (in C, Perl,
	and other unixy things anyway) codes for whitespace.  The escapes and
	their values are:

		\f   form feed
		\n   newline
		\r   carriage return
		\t   horizontal tab
		\v   vertical tab

	The numeric values are naturally those chosen by the compiler for those
	escape sequences; this means they not only vary by character set but
	also by system.

 *****************************************************************************/

void
JSubstitute::SetWhitespaceEscapes()
{
	SetEscape('f', "\f");
	SetEscape('n', "\n");
	SetEscape('r', "\r");
	SetEscape('t', "\t");
	SetEscape('v', "\v");
}

void
JSubstitute::ClearWhitespaceEscapes()
{
	ClearEscape('f');
	ClearEscape('n');
	ClearEscape('r');
	ClearEscape('t');
	ClearEscape('v');
}

/******************************************************************************
 C escapes

	Adds entries to the table corresponding to the non-numeric escapes
	specific to C (to get exactly C's escape set you also need to call
	SetNonprintingEscapes and SetWhitespaceEscapes, then remove the '\e'
	escape). The escapes and their values are:

		\\	\
		\'	'
		\"	"
		\?	?

	The numeric values are naturally those chosen by the compiler for those
	escape sequences; this means they not only vary by character set but
	also by system.

	Note that ANSI does not define what happens if you backslash a
	character other than one of the character escapes or an octal or hex
	code, so you have to choose the behavior you want with
	IgnoreUnrecognized().

 *****************************************************************************/

void
JSubstitute::SetCEscapes()
{
	SetEscape('\\', "\\");
	SetEscape('\'', "\'");
	SetEscape('"', "\"");
	SetEscape('?', "\?");
}

void
JSubstitute::ClearCEscapes()
{
	ClearEscape('\\');
	ClearEscape('\'');
	ClearEscape('"');
	ClearEscape('?');
}

/******************************************************************************
 Regex extensions

	Adds entries to the table corresponding to escapes useful as shorthands
	in defining regular expressions with JRegex.  The escapes and their
	values are:

		\d   a digit, [0-9]
		\D   a non-digit
		\w   a word character, [a-zA-Z0-9_]
		\W   a non-word character
		\s   a whitespace character, [ \f\n\r\t\v]
		\S   a non-whitespace character
		\<   an anchor just before a word (between \W and \w)
		\>   an anchor just after a word (between \w and \W)

	These escapes behave as atoms so they can be quantified normally and
	will not affect parenthesis numbering (this last requirement is why
	certain popular shorthands will not be added until Spencer's regexes
	acquire non-capturing parentheses so they can be defined atomically).

	Note: these are normally most useful when the behavior for unrecognized
	escapes is to leave them alone.  This is true whenever the string will
	be passed to another object which will do further backslash escape
	processing, such as JRegex (where "[" begins a character class while
	"\[" inserts a literal "[").

 *****************************************************************************/

void
JSubstitute::SetRegexExtensions()
{
	SetEscape('d', "[0-9]");
	SetEscape('D', "[^0-9]");
	SetEscape('w', "[a-zA-Z0-9_]");
	SetEscape('W', "[^a-zA-Z0-9_]");
	SetEscape('s', "[ \f\n\r\t\v]");
	SetEscape('S', "[^ \f\n\r\t\v]");
	SetEscape('<', "[[:<:]]");		// Aren't you glad I abbreviated these?!
	SetEscape('>', "[[:>:]]");
}

void
JSubstitute::ClearRegexExtensions()
{
	ClearEscape('d');
	ClearEscape('D');
	ClearEscape('w');
	ClearEscape('W');
	ClearEscape('s');
	ClearEscape('S');
	ClearEscape('<');
	ClearEscape('>');
}

/******************************************************************************
 DefineVariable

 *****************************************************************************/

void
JSubstitute::DefineVariable
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
	if (!SetVariableValue(name, value))
		{
		JString* n = new JString(name);
		assert( n != NULL );

		JString* v = new JString(value);
		assert( v != NULL );

		itsVarList->AppendElement(VarInfo(n, v));
		}
}

/******************************************************************************
 SetVariableValue

	Set the value of a non-regex variable.

 *****************************************************************************/

JBoolean
JSubstitute::SetVariableValue
	(
	const JCharacter* name,
	const JCharacter* value
	)
{
	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		VarInfo info = itsVarList->GetElement(i);
		if (info.regex == NULL && *(info.name) == name)
			{
			*(info.value) = value;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 DefineVariables

	If a regex matches after $, GetValue() is called.

 *****************************************************************************/

void
JSubstitute::DefineVariables
	(
	const JCharacter* regexPattern
	)
{
	JString* name = new JString(regexPattern);
	assert( name != NULL );

	JRegex* regex = new JRegex(regexPattern);
	assert( regex != NULL );

	itsVarList->AppendElement(VarInfo(name, regex));
}

/******************************************************************************
 UndefineVariable

	We remove all variables that match, in case the same string is used
	as both a literal and a regular expression.  (evil, but possible)

 *****************************************************************************/

void
JSubstitute::UndefineVariable
	(
	const JCharacter* name
	)
{
	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		VarInfo info = itsVarList->GetElement(i);
		if (*(info.name) == name)
			{
			delete info.name;
			delete info.regex;
			delete info.value;
			itsVarList->RemoveElement(i);
			}
		}
}

/******************************************************************************
 UndefineAllVariables

 *****************************************************************************/

void
JSubstitute::UndefineAllVariables()
{
	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		VarInfo info = itsVarList->GetElement(i);
		delete info.name;
		delete info.regex;
		delete info.value;
		}

	itsVarList->RemoveAll();
}

/******************************************************************************
 ContainsError

	Checks that every unescaped $ is followed by a valid variable name
	and every \c is followed by [A-_].

	If an error is found, returns one of our JError objects and sets
	*errRange to the offending character range.  If there are multiple
	errors, only the first one is reported.

 *****************************************************************************/

JError
JSubstitute::ContainsError
	(
	const JString&	s,
	JIndexRange*	errRange
	)
	const
{
	JIndex i = 1;
	JString varValue;
	while (1)
		{
		JCharacter opChar;
		if (!FindNextOperator(s, &i, &opChar))
			{
			break;
			}

		const JSize len = s.GetLength();

		// trailing operators will be tossed

		if (i == len && opChar == '\\')
			{
			errRange->Set(i, len);
			return TrailingBackslash();
			}
		else if (i == len && opChar == '$')
			{
			errRange->Set(i, len);
			return LoneDollar();
			}
		else if (itsControlEscapesFlag && opChar == '\\' &&
				 i == len-1 && s.GetLastCharacter() == 'c')
			{
			errRange->Set(i, len);
			return IllegalControlChar();
			}

		// check escaped character

		else if (opChar == '\\')
			{
			const JCharacter escapeChar = s.GetCharacter(i+1);

			// check control character

			if (escapeChar == 'c' && itsControlEscapesFlag)
				{
				const JCharacter controlChar = s.GetCharacter(i+2);
				if ('A' <= controlChar && controlChar <= '_')
					{
					i += 2;
					}
				else
					{
					errRange->Set(i, i+2);
					return IllegalControlChar();
					}
				}

			// normal escapes never cause problems

			else
				{
				i++;
				}
			}

		// check for variable name

		else if (opChar == '$')
			{
			JIndexRange range;
			if (!Evaluate(s, i+1, &range, &varValue))
				{
				errRange->Set(i, i);
				return LoneDollar();
				}
			i = range.last;
			}

		// move past valid text

		++i;
		}

	// falling through means that no errors were found

	errRange->SetToNothing();
	return JNoError();
}

/******************************************************************************
 Substitute

	Scans the given JString for each backslash and dollar symbol.

	If the backslash is followed by a character that has a value, that
	value is substituted for the backslash plus character.  Otherwise, the
	backslash is removed if IgnoreUnrecognized is not set.

	If ControlEscapes is set, and the backslash is followed by 'c', then
	the next character is converted to a control character if it is between
	'A' and '_'.  Otherwise, the '\\c' is removed if IgnoreUnrecognized is
	not set.  '@' is not included because this would produce NULL, which is
	the C string terminator.

	If PureEscapeEngine is not set and a $ is found, the value of the
	longest matching variable name is used to replace the $ and the
	variable name.  If nothing matches, the $ is removed.

	If a special character ('\\', '\\c' if ControlEscapes, '$' if not
	PureEscapeEngine) is found at the end of the string, it is removed.

	To avoid infinite loops, substituted values are not re-scanned, so
	backslashes and dollars in value strings are left untouched.

 *****************************************************************************/

void
JSubstitute::Substitute
	(
	JString* s
	)
	const
{
	assert( s != NULL );

	JIndex i = 1;
	JString varValue;
	while (1)
		{
		JCharacter opChar;
		if (!FindNextOperator(*s, &i, &opChar))
			{
			break;
			}

		const JSize len = s->GetLength();

		// toss trailing operator

		if (i == len ||
			(itsControlEscapesFlag && opChar == '\\' &&
			 i == len-1 && s->GetLastCharacter() == 'c'))
			{
			s->RemoveSubstring(i, len);
			}

		// handle escaped character

		else if (opChar == '\\')
			{
			const JCharacter escapeChar = s->GetCharacter(i+1);

			// handle control character escapes

			if (escapeChar == 'c' && itsControlEscapesFlag)
				{
				const JCharacter controlChar = s->GetCharacter(i+2);
				if ('A' <= controlChar && controlChar <= '_')
					{
					s->RemoveSubstring(i, i+1);
					s->SetCharacter(i, controlChar - '@');
					}
				else if (!itsIgnoreUnrecognizedFlag)
					{
					s->RemoveSubstring(i, i+1);
					}
				else
					{
					i += 2;
					}
				}

			// handle normal escapes

			else
				{
				const JString* value;
				if (GetEscape(escapeChar, &value))
					{
					JIndexRange range(i,i+1);
					s->ReplaceSubstring(range, *value, &range);
					i = range.last;
					}
				else if (!itsIgnoreUnrecognizedFlag)
					{
					s->RemoveSubstring(i, i);
					}
				else
					{
					i++;
					}
				}
			}

		// Special treatment for $$
		// (\$ cannot be used in input files for compile_jstrings)

		else if (opChar == '$' && s->GetCharacter(i+1) == '$')
			{
			s->RemoveSubstring(i,i);
			}

		// handle variable name

		else if (opChar == '$')
			{
			JIndexRange range;
			if (!Evaluate(*s, i+1, &range, &varValue))
				{
				varValue.Clear();
				}
			range.first = i;
			s->ReplaceSubstring(range, varValue, &range);
			i = range.last;
			}

		// move past new text

		i++;
		}
}

/******************************************************************************
 FindNextOperator (private)

	Starting from *index, finds the index of the next backslash or dollar.
	Returns kJFalse if nothing is found.

 *****************************************************************************/

JBoolean
JSubstitute::FindNextOperator
	(
	const JString&	s,
	JIndex*			index,
	JCharacter*		opChar
	)
	const
{
	const JSize len = s.GetLength();
	while (*index <= len)
		{
		*opChar = s.GetCharacter(*index);
		if (*opChar == '\\' || (*opChar == '$' && !itsPureEscapeEngineFlag))
			{
			break;
			}
		(*index)++;
		}
	return JI2B(*index <= len);
}

/******************************************************************************
 Evaluate (virtual protected)

	Returns kJTrue if it found a variable name starting at startIndex.
	It checks all variables to find the longest match.  *value contains
	the variable's value.

	Derived classes can override this if they have names that can't
	be expressed as regular expressions. (e.g. $(w $($(x) y) z))

 *****************************************************************************/

JBoolean
JSubstitute::Evaluate
	(
	const JString&	s,
	const JIndex	startIndex,
	JIndexRange*	matchRange,
	JString*		value
	)
	const
{
	JSize matchLength = 0;
	JIndex varIndex   = 0;

	const JCharacter* str = s.GetCString() + startIndex-1;

	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const VarInfo info = itsVarList->GetElement(i);
		const JSize len    = (info.name)->GetLength();
		if (info.regex == NULL &&
			len > matchLength &&
			JCompareMaxN(str, *(info.name), len))
			{
			matchLength = len;
			varIndex    = i;
			}
		else if (info.regex != NULL)
			{
			JIndexRange range;
			if ((info.regex)->Match(str, &range) &&
				range.first == 1 &&
				range.GetLength() > matchLength)
				{
				matchLength = range.GetLength();
				varIndex    = 0;
				}
			}
		}

	matchRange->SetFirstAndLength(startIndex, matchLength);
	if (matchLength == 0)
		{
		value->Clear();
		return kJFalse;
		}
	else if (varIndex == 0)
		{
		return GetValue(s.GetSubstring(*matchRange), value);
		}
	else
		{
		const VarInfo info = itsVarList->GetElement(varIndex);
		*value             = *(info.value);
		return kJTrue;
		}
}

/******************************************************************************
 GetValue (virtual protected)

	Returns kJTrue if there is a variable with the given name.  The default
	is to return kJFalse.  This function can't be pure virtual because one
	shouldn't have to create a derived class if one only has literal variable
	names.

 *****************************************************************************/

JBoolean
JSubstitute::GetValue
	(
	const JString&	name,
	JString*		value
	)
	const
{
	value->Clear();
	return kJFalse;
}
