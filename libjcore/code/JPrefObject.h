/******************************************************************************
 JPrefObject.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPrefObject
#define _H_JPrefObject

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPrefsManager.h>	// need definition of JPrefID

class JPrefObject
{
public:

	JPrefObject(JPrefsManager* prefsMgr, const JPrefID& id);

	virtual ~JPrefObject();

	JBoolean	PrefInfoValid() const;
	JBoolean	GetPrefInfo(JPrefsManager** mgr, JIndex* id) const;
	void		SetPrefInfo(JPrefsManager* prefsMgr, const JPrefID& id);

	void	ReadPrefs();
	void	WritePrefs() const;

protected:

	// override only in leaf classes

	virtual void	ReadPrefs(istream& input) = 0;
	virtual void	WritePrefs(ostream& output) const = 0;

private:

	JPrefsManager*	itsPrefsMgr;	// not owned; can be NULL
	JPrefID			itsID;

private:

	// not allowed

	JPrefObject(const JPrefObject& source);
	const JPrefObject& operator=(const JPrefObject& source);
};


/******************************************************************************
 Prefs info

 ******************************************************************************/

inline JBoolean
JPrefObject::PrefInfoValid()
	const
{
	return JI2B( itsPrefsMgr != NULL && itsID.IsValid() );
}

inline JBoolean
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
