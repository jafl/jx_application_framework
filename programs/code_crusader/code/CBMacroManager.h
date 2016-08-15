/******************************************************************************
 CBMacroManager.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBMacroManager
#define _H_CBMacroManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>
#include <JFont.h>

class JString;
class JColormap;
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
			macro(NULL), script(NULL)
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

	JBoolean	Perform(const JIndex caretIndex, CBTextDocument* doc);

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

	// called by CBCharActionManager and tables

	static void	Perform(const JString& script, CBTextDocument* doc);
	static void	HighlightErrors(const JString& script, const JColormap* colormap,
								JRunArray<JFont>* styles);

	// called by CBMacroTable

	void	AddMacro(const JCharacter* macro, const JCharacter* script);
	void	RemoveAllMacros();

	const CBMacroList&	GetMacroList() const;

private:

	CBMacroList*	itsMacroList;

private:

	void	CBMacroManagerX();

	static JOrderedSetT::CompareResult
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
