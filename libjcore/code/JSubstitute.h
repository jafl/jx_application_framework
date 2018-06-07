/******************************************************************************
 JSubstitute.h

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_JSubstitute
#define _H_JSubstitute

#include <JArray.h>
#include <JError.h>

class JRegex;
class JCharacterRange;
class JStringIterator;

class JSubstitute
{
public:

	JSubstitute();
	JSubstitute(const JSubstitute& source);

	virtual ~JSubstitute();

	const JSubstitute& operator=(const JSubstitute& source);

	void	Substitute(JString* s) const;
	JError	ContainsError(const JString& s, JCharacterRange* errRange) const;

	// control of escape substitution

	JBoolean	EscapeExists(const unsigned char character) const;
	JBoolean	GetEscape(const unsigned char character, const JString** value) const;

	JBoolean	SetEscape(const unsigned char character, const JUtf8Byte* value);

	JBoolean	ClearEscape(const unsigned char character);
	void		ClearAllEscapes();

	// useful shortcuts

	void SetNonprintingEscapes();
	void ClearNonprintingEscapes();
	void SetWhitespaceEscapes();
	void ClearWhitespaceEscapes();
	void SetCEscapes();
	void ClearCEscapes();
	void SetRegexExtensions();
	void ClearRegexExtensions();

	// control of variable substitution

	void		DefineVariable(const JUtf8Byte* name, const JString& value);
	JBoolean	SetVariableValue(const JUtf8Byte* name, const JString& value);
	void		DefineVariables(const JUtf8Byte* regexPattern);
	void		UndefineVariable(const JUtf8Byte* name);
	void		UndefineAllVariables();

	// options

	void	Reset();

	// set to kJTrue to convert \cX to control-X

	JBoolean	IsUsingControlEscapes() const;
	void		UseControlEscapes(const JBoolean use = kJTrue);

	// Set to kJTrue to skip over ignored escapes

	JBoolean	WillIgnoreUnrecognized() const;
	void		IgnoreUnrecognized(const JBoolean ignore = kJTrue);

	// set to kJTrue to turn off variable substitution

	JBoolean	IsPureEscapeEngine() const;
	void		SetPureEscapeEngine(const JBoolean is = kJTrue);

protected:

	virtual JBoolean	Evaluate(JStringIterator& iter, JString* value) const;
	virtual JBoolean	GetValue(const JString& name, JString* value) const;

private:

	struct VarInfo
	{
		JString*	name;
		JRegex*		regex;	// nullptr if name is not regex
		JString*	value;	// nullptr if name is regex

		VarInfo()
			:
			name(nullptr), regex(nullptr), value(nullptr)
		{ };

		VarInfo(JString* n, JString* v)
			:
			name(n), regex(nullptr), value(v)
		{ };

		VarInfo(JString* n, JRegex* r)
			:
			name(n), regex(r), value(nullptr)
		{ };
	};

private:

	JString**			itsEscapeTable;
	JArray<VarInfo>*	itsVarList;
	JBoolean			itsControlEscapesFlag;		// kJTrue => use \cX
	JBoolean			itsIgnoreUnrecognizedFlag;	// kJTrue => leave backslash if no defn
	JBoolean			itsPureEscapeEngineFlag;	// kJTrue => $ not special

private:

	void	AllocateInternals();
	void	CopyInternals(const JSubstitute& source);

	JBoolean	FindNextOperator(JStringIterator& iter, JUtf8Character* opChar) const;

public:

	// JError classes

	static const JUtf8Byte* kLoneDollar;
	static const JUtf8Byte* kTrailingBackslash;
	static const JUtf8Byte* kIllegalControlChar;

	class LoneDollar : public JError
		{
		public:

			LoneDollar()
				:
				JError(kLoneDollar)
			{ };
		};

	class TrailingBackslash : public JError
		{
		public:

			TrailingBackslash()
				:
				JError(kTrailingBackslash)
			{ };
		};

	class IllegalControlChar : public JError
		{
		public:

			IllegalControlChar()
				:
				JError(kIllegalControlChar)
			{ };
		};
};


/******************************************************************************
 EscapeExists

	Returns kJTrue if an escape exists for the given character, kJFalse otherwise.

 *****************************************************************************/

inline JBoolean
JSubstitute::EscapeExists
	(
	const unsigned char c
	)
	const
{
	return JI2B( itsEscapeTable[c] != nullptr );
}

/******************************************************************************
 GetEscape

	Returns kJTrue if there is a definition for the given character.

 *****************************************************************************/

inline JBoolean
JSubstitute::GetEscape
	(
	const unsigned char	c,
	const JString**		value
	)
	const
{
	*value = itsEscapeTable[c];
	return JI2B( *value != nullptr );
}

/******************************************************************************
 Control escapes

	This flag enables special Perl-style two-character escapes for control
	characters; "\cX" is replaced with the control-X character.  The
	default is no control escapes.  When enabled '\c' obeys the special
	rules below; when disabled it obeys the ordinary rules.  Any ordinary
	value bound to '\c' is untouched.

	Refer to the documentation for Substitute() for more information.

 *****************************************************************************/

inline JBoolean
JSubstitute::IsUsingControlEscapes()
	const
{
	return itsControlEscapesFlag;
}

inline void
JSubstitute::UseControlEscapes
	(
	const JBoolean use
	)
{
	itsControlEscapesFlag = use;
}

/******************************************************************************
 Ignore unrecognized

	This flag controls the behavior when an escape is not recognized.
	kJFalse (the default) indicates that unknown escapes should be ignored,
	leaving the backslash in place.  kJTrue means that the backslash should
	be removed, so that unrecognized escapes just represent the escaped
	character itself.

 *****************************************************************************/

inline JBoolean
JSubstitute::WillIgnoreUnrecognized()
	const
{
	return itsIgnoreUnrecognizedFlag;
}

inline void
JSubstitute::IgnoreUnrecognized
	(
	const JBoolean ignore
	)
{
	itsIgnoreUnrecognizedFlag = ignore;
}

/******************************************************************************
 Pure escape engine

	Set this flag to kJTrue to avoid treating $ as an operator.  The
	default value is kJFalse.

 *****************************************************************************/

inline JBoolean
JSubstitute::IsPureEscapeEngine()
	const
{
	return itsPureEscapeEngineFlag;
}

inline void
JSubstitute::SetPureEscapeEngine
	(
	const JBoolean is
	)
{
	itsPureEscapeEngineFlag = is;
}

/******************************************************************************
 Reset

	Resets the object to its initial state.

 *****************************************************************************/

inline void
JSubstitute::Reset()
{
	ClearAllEscapes();
	UndefineAllVariables();

	UseControlEscapes(kJFalse);
	IgnoreUnrecognized(kJFalse);
	SetPureEscapeEngine(kJFalse);
}

#endif
