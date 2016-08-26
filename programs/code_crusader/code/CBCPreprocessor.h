/******************************************************************************
 CBCPreprocessor.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCPreprocessor
#define _H_CBCPreprocessor

#include <JArray.h>
#include <ctype.h>		// need isalnum()

class JString;
class CBPPMacroList;

class CBCPreprocessor
{
public:

	struct MacroInfo
	{
		JString*	name;
		JString*	value;

		MacroInfo()
			:
			name(NULL), value(NULL)
		{ };

		MacroInfo(JString* n, JString* v)
			:
			name(n), value(v)
		{ };
	};

public:

	CBCPreprocessor();

	~CBCPreprocessor();

	JBoolean	Preprocess(JString* text) const;
	void		PrintMacrosForCTags(ostream& output) const;

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

	// called by CBMacroTable

	void	DefineMacro(const JCharacter* name, const JCharacter* value);
	void	UndefineAllMacros();

	const CBPPMacroList&	GetMacroList() const;

private:

	CBPPMacroList*	itsMacroList;

private:

	JBoolean	IsEntireWord(const JString& text, const JIndex wordIndex,
					 const JSize wordLength) const;
	int			IsIDCharacter(const JCharacter c) const;

	void	ReadMacro(istream& input, const JFileVersion vers);

	static JOrderedSetT::CompareResult
		CompareMacros(const MacroInfo& m1, const MacroInfo& m2);

	// not allowed

	CBCPreprocessor(const CBCPreprocessor& source);
	const CBCPreprocessor& operator=(const CBCPreprocessor& source);
};


class CBPPMacroList : public JArray<CBCPreprocessor::MacroInfo>
{
public:

	void	DeleteAll();
};


/******************************************************************************
 UndefineAllMacros

 ******************************************************************************/

inline void
CBCPreprocessor::UndefineAllMacros()
{
	itsMacroList->DeleteAll();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

inline const CBPPMacroList&
CBCPreprocessor::GetMacroList()
	const
{
	return *itsMacroList;
}

/******************************************************************************
 IsIDCharacter (private)

 ******************************************************************************/

inline int
CBCPreprocessor::IsIDCharacter
	(
	const JCharacter c
	)
	const
{
	return (isalnum(c) || c == '_');
}

#endif
