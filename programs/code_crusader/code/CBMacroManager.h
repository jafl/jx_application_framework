/******************************************************************************
 CBMacroManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBMacroManager
#define _H_CBMacroManager

#include <JStyledText.h>

class JString;
class CBMacroList;
class CBTextDocument;

class CBMacroManager
{
public:

	struct MacroInfo
	{
		JString*	macro;
		JString*	script;

		MacroInfo()
			:
			macro(nullptr), script(nullptr)
		{ };

		MacroInfo(JString* m, JString* s)
			:
			macro(m), script(s)
		{ };
	};

public:

	CBMacroManager();
	CBMacroManager(const CBMacroManager& source);

	~CBMacroManager();

	bool	Perform(const JStyledText::TextIndex& caretIndex, CBTextDocument* doc);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CBCharActionManager and tables

	static void	Perform(const JString& script, CBTextDocument* doc);
	static void	HighlightErrors(const JString& script, JRunArray<JFont>* styles);

	// called by CBMacroTable

	void	AddMacro(const JUtf8Byte* macro, const JUtf8Byte* script);
	void	RemoveAllMacros();

	const CBMacroList&	GetMacroList() const;

private:

	CBMacroList*	itsMacroList;

private:

	void	CBMacroManagerX();

	static JListT::CompareResult
		CompareMacros(const MacroInfo& m1, const MacroInfo& m2);

	// not allowed

	const CBMacroManager& operator=(const CBMacroManager& source);
};


class CBMacroList : public JArray<CBMacroManager::MacroInfo>
{
public:

	void	DeleteAll();
};


/******************************************************************************
 RemoveAllMacros

 ******************************************************************************/

inline void
CBMacroManager::RemoveAllMacros()
{
	itsMacroList->DeleteAll();
}

/******************************************************************************
 GetMacroList

 ******************************************************************************/

inline const CBMacroList&
CBMacroManager::GetMacroList()
	const
{
	return *itsMacroList;
}

#endif
