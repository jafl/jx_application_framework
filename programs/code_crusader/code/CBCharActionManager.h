/******************************************************************************
 CBCharActionManager.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCharActionManager
#define _H_CBCharActionManager

#include <JPtrArray-JString.h>

class CBTextDocument;

class CBCharActionManager
{
public:

	CBCharActionManager();
	CBCharActionManager(const CBCharActionManager& source);

	~CBCharActionManager();

	void	Perform(const unsigned char c, CBTextDocument* doc);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CBCharActionTable

	JBoolean	GetAction(const unsigned char c, JString* script) const;
	void		SetAction(const unsigned char c, const JCharacter* script);
	void		ClearAction(const unsigned char c);
	void		ClearAllActions();

	const JPtrArray<JString>&	GetActionList() const;

private:

	JPtrArray<JString>*	itsActionList;

private:

	// not allowed

	const CBCharActionManager& operator=(const CBCharActionManager& source);
};


/******************************************************************************
 GetActionList

 ******************************************************************************/

inline const JPtrArray<JString>&
CBCharActionManager::GetActionList()
	const
{
	return *itsActionList;
}

#endif
