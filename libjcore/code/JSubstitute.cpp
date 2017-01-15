/******************************************************************************
 JSubstitute.cpp

	JSubstitute replaces escaped characters like \a and variables of the
	form $name in a JString.  "name" can either be a literal, in which case
	we store the value and perform the replacement, or it can be a regular
	expression, in which case we call the virtual function GetValue().  An
	example of the latter case is [+-]?[0-9]+, which is used in regular
	expression replace patterns to denote submatches.

	By default C escapes are not expanded since this is most convenient for
	patterns specified in source code; in user-specified patterns in
	interactive programs, it may be better to add these escapes so that
	non-printing characters may be entered conveniently.

	BASE CLASS = none

	Copyright (C) 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JSubstitute.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

const JSize kEscapeCount = 256;

// JError messages

const JUtf8Byte* JSubstitute::kLoneDollar         = "LoneDollar::JSubstitute";
const JUtf8Byte* JSubstitute::kTrailingBackslash  = "TrailingBackslash::JSubstitute";
const JUtf8Byte* JSubstitute::kIllegalControlChar = "IllegalControlChar::JSubstitute";

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
	jdelete [] itsEscapeTable;

	UndefineAllVariables();
	jdelete itsVarList;
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
	itsEscapeTable = jnew JString* [ kEscapeCount ];
	assert(itsEscapeTable != NULL);

	itsVarList = jnew JArray<VarInfo>;
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
			itsEscapeTable[i] = jnew JString(*(source.itsEscapeTable[i]));
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

		newInfo.name = jnew JString(*(origInfo.name));
		assert( newInfo.name != NULL );

		if (origInfo.regex != NULL)
			{
			newInfo.regex = jnew JRegex(*(origInfo.regex));
			assert( newInfo.regex != NULL );
			}

		if (origInfo.value != NULL)
			{
			newInfo.value = jnew JString(*(origInfo.value));
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
	const JUtf8Byte*	value
	)
{
	if (itsEscapeTable[c] != NULL)
		{
		(itsEscapeTable[c])->Set(value);
		return kJTrue;
		}
	else
		{
		itsEscapeTable[c] = jnew JString(value, 0);
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
		jdelete itsEscapeTable[c];
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
		jdelete itsEscapeTable[i];
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
	const JUtf8Byte*	name,
	const JString&		value
	)
{
	if (!SetVariableValue(name, value))
		{
		JString* n = jnew JString(name, 0);
		assert( n != NULL );

		JString* v = jnew JString(value);
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
	const JUtf8Byte*	name,
	const JString&		value
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
	const JUtf8Byte* regexPattern
	)
{
	JString* name = jnew JString(regexPattern, 0);
	assert( name != NULL );

	JRegex* regex = jnew JRegex(regexPattern);
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
	const JUtf8Byte* name
	)
{
	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		VarInfo info = itsVarList->GetElement(i);
		if (*(info.name) == name)
			{
			jdelete info.name;
			jdelete info.regex;
			jdelete info.value;
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
		jdelete info.name;
		jdelete info.regex;
		jdelete info.value;
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
	const JString&		s,
	JCharacterRange*	errRange
	)
	const
{
	JString varValue;

	JStringIterator iter(s);
	JUtf8Character opChar, c;
	while (FindNextOperator(iter, &opChar))
		{
		// trailing operators will be tossed

		if (iter.AtEnd() && opChar == '\\')
			{
			errRange->SetFirstAndCount(iter.GetPrevCharacterIndex(), 1);
			return TrailingBackslash();
			}
		else if (iter.AtEnd() && opChar == '$')
			{
			errRange->SetFirstAndCount(iter.GetPrevCharacterIndex(), 1);
			return LoneDollar();
			}

		// check escaped character

		else if (opChar == '\\')	// guaranteed not to be last character
			{
			iter.Next(&c);

			// check control character

			if (c == 'c' && itsControlEscapesFlag)
				{
				if (iter.AtEnd())
					{
					errRange->SetFirstAndCount(iter.GetPrevCharacterIndex() - 1, 2);
					return IllegalControlChar();
					}

				iter.Next(&c);
				const JUtf8Byte ascii = c.GetBytes()[0];
				if (!('A' <= ascii && ascii <= '_'))	// written this way to avoid worrying about signed char
					{
					errRange->SetFirstAndCount(iter.GetPrevCharacterIndex() - 2, 3);
					return IllegalControlChar();
					}
				}

			// normal escapes never cause problems
			}

		// check for variable name

		else if (opChar == '$' && !Evaluate(iter, &varValue))
			{
			errRange->SetFirstAndCount(iter.GetPrevCharacterIndex(), 1);
			return LoneDollar();
			}
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

	JString varValue;

	JStringIterator iter(s);
	JUtf8Character opChar, c;
	while (FindNextOperator(iter, &opChar))
		{
		iter.SkipPrev();
		iter.BeginMatch();
		iter.SkipNext();

		if (iter.AtEnd())
			{
			iter.FinishMatch();
			iter.RemoveLastMatch();
			break;
			}

		// handle escaped character

		if (opChar == '\\')
			{
			JBoolean ok = iter.Next(&c);
			assert( ok );

			// toss trailing operator

			if (c == 'c' && itsControlEscapesFlag && iter.AtEnd())
				{
				iter.FinishMatch();
				iter.RemoveLastMatch();
				}

			// handle control character escapes

			else if (c == 'c' && itsControlEscapesFlag)
				{
				ok = iter.Next(&c);
				assert( ok );

				const JUtf8Byte ascii = c.GetBytes()[0];
				if ('A' <= ascii && ascii <= '_')
					{
					iter.FinishMatch();
					iter.ReplaceLastMatch(JUtf8Character(ascii - '@'));
					}
				else if (!itsIgnoreUnrecognizedFlag)
					{
					iter.FinishMatch();
					iter.ReplaceLastMatch(c);
					}
				}

			// handle normal escapes

			else
				{
				const JString* value;
				const JUtf8Byte ascii = c.GetBytes()[0];
				if (0 < ascii && ascii <= '\x7F' && GetEscape(ascii, &value))
					{
					iter.FinishMatch();
					iter.ReplaceLastMatch(*value);
					}
				else if (!itsIgnoreUnrecognizedFlag)
					{
					iter.FinishMatch();
					iter.ReplaceLastMatch(c);
					}
				}
			}

		// handle variable name

		else if (opChar == '$')
			{
			JBoolean ok = iter.Next(&c, kJFalse);
			assert( ok );

			if (c == '$')	// Special treatment for $$ (\$ cannot be used in input files for compile_jstrings)
				{
				iter.FinishMatch();
				iter.ReplaceLastMatch(c);
				}
			else
				{
				if (!Evaluate(iter, &varValue))
					{
					varValue.Clear();
					}
				iter.FinishMatch();
				iter.ReplaceLastMatch(varValue);
				}
			}
		}
}

/******************************************************************************
 FindNextOperator (private)

	Finds the next backslash or dollar.  Returns kJFalse if nothing is
	found.

 *****************************************************************************/

JBoolean
JSubstitute::FindNextOperator
	(
	JStringIterator&	iter,
	JUtf8Character*		opChar
	)
	const
{
	while (iter.Next(opChar))
		{
		if (*opChar == '\\' || (*opChar == '$' && !itsPureEscapeEngineFlag))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 Evaluate (virtual protected)

	Returns kJTrue if it found a variable name starting at startIndex.
	It checks all variables to find the longest match.  *value contains
	the variable's value.

	Derived classes can override this if they have names that can't
	be expressed as regular expressions, e.g., $(w $($(x) y) z)

 *****************************************************************************/

JBoolean
JSubstitute::Evaluate
	(
	JStringIterator&	iter,
	JString*			value
	)
	const
{
	assert( !iter.AtEnd() );

	JSize matchCharCount = 0;
	JSize matchByteCount = 0;
	JIndex varIndex      = 0;

	JString s(iter.GetString().GetBytes(),
			  JUtf8ByteRange(iter.GetNextByteIndex(),
							 iter.GetString().GetByteCount()),
			  kJFalse);

	const JSize count = itsVarList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const VarInfo info    = itsVarList->GetElement(i);
		const JSize charCount = info.name->GetCharacterCount();
		if (info.regex == NULL && s.BeginsWith(*(info.name)) &&
			charCount > matchCharCount)
			{
			varIndex       = i;
			matchCharCount = charCount;
			matchByteCount = info.name->GetByteCount();
			}
		else if (info.regex != NULL)
			{
			const JStringMatch m = info.regex->MatchForward(s, 1);
			if (!m.IsEmpty() && m.GetCharacterRange().first == 1 &&
				m.GetCharacterCount() > matchCharCount)
				{
				varIndex       = 0;
				matchCharCount = m.GetCharacterCount();
				matchByteCount = m.GetByteCount();
				}
			}
		}

	if (matchCharCount == 0)
		{
		value->Clear();
		return kJFalse;
		}
	else if (varIndex == 0)
		{
		JUtf8ByteRange r;
		r.SetFirstAndCount(iter.GetNextByteIndex(), matchByteCount);

		JString name(iter.GetString().GetBytes(), r, kJFalse);
		const JBoolean ok = GetValue(name, value);
		if (ok)
			{
			iter.SkipNext(matchCharCount);
			}
		return ok;
		}
	else
		{
		const VarInfo info = itsVarList->GetElement(varIndex);
		*value             = *(info.value);
		iter.SkipNext(matchCharCount);
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
