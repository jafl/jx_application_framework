/******************************************************************************
 CBCharActionManager.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCharActionManager
#define _H_CBCharActionManager

#include <JStringPtrMap.h>

class CBTextDocument;

class CBCharActionManager
{
public:

	CBCharActionManager();
	CBCharActionManager(const CBCharActionManager& source);

	~CBCharActionManager();

	void	Perform(const JUtf8Character& c, CBTextDocument* doc);

	void	ReadSetup(std::istream& input);
	void	WriteSetup(std::ostream& output) const;

	// called by CBCharActionTable

	void	SetAction(const JUtf8Character& c, const JString& script);
	void	ClearAction(const JUtf8Character& c);
	void	ClearAllActions();

	const JStringPtrMap<JString>&	GetActionMap() const;

private:

	JStringPtrMap<JString>*	itsActionMap;

private:

	// not allowed

	const CBCharActionManager& operator=(const CBCharActionManager& source);
};


/******************************************************************************
 GetActionMap

 ******************************************************************************/

inline const JStringPtrMap<JString>&
CBCharActionManager::GetActionMap()
	const
{
	return *itsActionMap;
}

#endif
