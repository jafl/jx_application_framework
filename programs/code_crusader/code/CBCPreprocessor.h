/******************************************************************************
 CBCPreprocessor.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCPreprocessor
#define _H_CBCPreprocessor

#include <JUtf8Character.h>
#include <JArray.h>

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
			name(nullptr), value(nullptr)
		{ };

		MacroInfo(JString* n, JString* v)
			:
			name(n), value(v)
		{ };
	};

public:

	CBCPreprocessor();

	~CBCPreprocessor();

	bool	Preprocess(JString* text) const;
	void		PrintMacrosForCTags(std::ostream& output) const;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// called by CBMacroTable

	void	DefineMacro(const JString& name, const JString& value);
	void	UndefineAllMacros();

	const CBPPMacroList&	GetMacroList() const;

private:

	CBPPMacroList*	itsMacroList;

private:

	void	ReadMacro(std::istream& input, const JFileVersion vers);

	static JListT::CompareResult
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

#endif
