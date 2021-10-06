/******************************************************************************
 JSubstitute.h

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_JSubstitute
#define _H_JSubstitute

#include "jx-af/jcore/JArray.h"
#include "jx-af/jcore/JError.h"

class JRegex;
class JCharacterRange;
class JStringIterator;

class JSubstitute
{
public:

	JSubstitute();
	JSubstitute(const JSubstitute& source);

	virtual ~JSubstitute();

	JSubstitute& operator=(const JSubstitute& source);

	void	Substitute(JString* s) const;
	JError	ContainsError(const JString& s, JCharacterRange* errRange) const;

	// control of escape substitution

	bool	EscapeExists(const unsigned char character) const;
	bool	GetEscape(const unsigned char character, const JString** value) const;

	bool	SetEscape(const unsigned char character, const JUtf8Byte* value);

	bool	ClearEscape(const unsigned char character);
	void	ClearAllEscapes();

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

	void	DefineVariable(const JUtf8Byte* name, const JString& value);
	bool	SetVariableValue(const JUtf8Byte* name, const JString& value);
	void	DefineVariables(const JUtf8Byte* regexPattern);
	void	UndefineVariable(const JUtf8Byte* name);
	void	UndefineAllVariables();

	// options

	void	Reset();

	// set to true to convert \cX to control-X

	bool	IsUsingControlEscapes() const;
	void	UseControlEscapes(const bool use = true);

	// Set to true to skip over ignored escapes

	bool	WillIgnoreUnrecognized() const;
	void	IgnoreUnrecognized(const bool ignore = true);

	// set to true to turn off variable substitution

	bool	IsPureEscapeEngine() const;
	void	SetPureEscapeEngine(const bool is = true);

protected:

	virtual bool	Evaluate(JStringIterator& iter, JString* value) const;
	virtual bool	GetValue(const JString& name, JString* value) const;

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
	bool			itsControlEscapesFlag;		// true => use \cX
	bool			itsIgnoreUnrecognizedFlag;	// true => leave backslash if no defn
	bool			itsPureEscapeEngineFlag;	// true => $ not special

private:

	void	AllocateInternals();
	void	CopyInternals(const JSubstitute& source);

	bool	FindNextOperator(JStringIterator& iter, JUtf8Character* opChar) const;

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

	Returns true if an escape exists for the given character, false otherwise.

 *****************************************************************************/

inline bool
JSubstitute::EscapeExists
	(
	const unsigned char c
	)
	const
{
	return itsEscapeTable[c] != nullptr;
}

/******************************************************************************
 GetEscape

	Returns true if there is a definition for the given character.

 *****************************************************************************/

inline bool
JSubstitute::GetEscape
	(
	const unsigned char	c,
	const JString**		value
	)
	const
{
	*value = itsEscapeTable[c];
	return *value != nullptr;
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

inline bool
JSubstitute::IsUsingControlEscapes()
	const
{
	return itsControlEscapesFlag;
}

inline void
JSubstitute::UseControlEscapes
	(
	const bool use
	)
{
	itsControlEscapesFlag = use;
}

/******************************************************************************
 Ignore unrecognized

	This flag controls the behavior when an escape is not recognized.
	false (the default) indicates that unknown escapes should be ignored,
	leaving the backslash in place.  true means that the backslash should
	be removed, so that unrecognized escapes just represent the escaped
	character itself.

 *****************************************************************************/

inline bool
JSubstitute::WillIgnoreUnrecognized()
	const
{
	return itsIgnoreUnrecognizedFlag;
}

inline void
JSubstitute::IgnoreUnrecognized
	(
	const bool ignore
	)
{
	itsIgnoreUnrecognizedFlag = ignore;
}

/******************************************************************************
 Pure escape engine

	Set this flag to true to avoid treating $ as an operator.  The
	default value is false.

 *****************************************************************************/

inline bool
JSubstitute::IsPureEscapeEngine()
	const
{
	return itsPureEscapeEngineFlag;
}

inline void
JSubstitute::SetPureEscapeEngine
	(
	const bool is
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

	UseControlEscapes(false);
	IgnoreUnrecognized(false);
	SetPureEscapeEngine(false);
}

#endif
