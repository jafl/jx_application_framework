/******************************************************************************
 JPrefObject.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPrefObject
#define _H_JPrefObject

#include "JPrefsManager.h"	// for JPrefID

class JPrefObject
{
public:

	JPrefObject(JPrefsManager* prefsMgr, const JPrefID& id);

	virtual ~JPrefObject();

	bool	PrefInfoValid() const;
	bool	GetPrefInfo(JPrefsManager** mgr, JIndex* id) const;
	void	SetPrefInfo(JPrefsManager* prefsMgr, const JPrefID& id);

	void	ReadPrefs();
	void	WritePrefs() const;

protected:

	// override only in leaf classes

	virtual void	ReadPrefs(std::istream& input) = 0;
	virtual void	WritePrefs(std::ostream& output) const = 0;

private:

	JPrefsManager*	itsPrefsMgr;	// not owned; can be nullptr
	JPrefID			itsID;

private:

	// not allowed

	JPrefObject(const JPrefObject&) = delete;
	JPrefObject& operator=(const JPrefObject&) = delete;
};


/******************************************************************************
 Prefs info

 ******************************************************************************/

inline bool
JPrefObject::PrefInfoValid()
	const
{
	return itsPrefsMgr != nullptr && itsID.IsValid();
}

inline bool
JPrefObject::GetPrefInfo
	(
	JPrefsManager**	mgr,
	JIndex*			id
	)
	const
{
	*mgr = itsPrefsMgr;
	*id  = itsID.GetID();
	return PrefInfoValid();
}

inline void
JPrefObject::SetPrefInfo
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
{
	itsPrefsMgr = prefsMgr;
	itsID       = id;
}

#endif
