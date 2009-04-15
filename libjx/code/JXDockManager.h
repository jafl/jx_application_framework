/******************************************************************************
 JXDockManager.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDockManager
#define _H_JXDockManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirector.h>
#include <JPrefObject.h>
#include <JStringMap.h>
#include <JString.h>
#include <JXDockDirector.h>		// for Windows template compile
#include <X11/Xlib.h>

class JXDisplay;
class JXDockWidget;

class JXDockManager : public JXDirector, public JPrefObject
{
public:

	enum CloseDockMode
	{
		kCloseWindows,
		kUndockWindows
	};

	enum
	{
		kInvalidDockID = 0
	};

public:

	JXDockManager(JXDisplay* display, const JCharacter* title,
				  JPrefsManager* prefsMgr = NULL,
				  const JPrefID& id = JFAID::kInvalidID);

	virtual ~JXDockManager();

	JXDockDirector*	CreateDock(const JBoolean splitHoriz);
	void			CloseAll();

	JPtrArray<JXDockDirector>*	GetDockList() const;
	JBoolean					FindDock(const JIndex id, JXDockWidget** dock);
	JBoolean					IsLastDock(JXDockDirector* dock) const;

	virtual	JBoolean	CanDockAll() const;
	virtual void		DockAll();

	JBoolean	GetDefaultDock(const JCharacter* windowType, JXDockWidget** dock);
	void		SetDefaultDock(const JCharacter* windowType, const JXDockWidget* dock);

	JBoolean	IsReadingSetup() const;
	void		ReadSetup(istream& input);
	void		WriteSetup(ostream& output) const;

	Atom	GetDNDMinSizeAtom() const;
	Atom	GetDNDWindowAtom() const;

	// for use by JXDockWidget

	JIndex	GetUniqueDockID();
	void	IDUsed(const JIndex id);

	CloseDockMode	GetCloseDockMode() const;
	void			SetCloseDockMode(const CloseDockMode mode);

protected:

	virtual void	DirectorClosed(JXDirector* theDirector);

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	JXDisplay*					itsDisplay;
	JPtrArray<JXDockDirector>*	itsDockList;
	JStringMap<JIndex>*			itsWindowTypeMap;
	JString						itsTitle;
	JIndex						itsNextDockIndex;
	JIndex						itsNextDockID;
	JBoolean					itsIsReadingSetupFlag;
	CloseDockMode				itsCloseDockMode;
	Atom						itsDNDMinSizeAtom;
	Atom						itsDNDWindowAtom;

private:

	JString	GetNewDockTitle();

	// not allowed

	JXDockManager(const JXDockManager& source);
	const JXDockManager& operator=(const JXDockManager& source);
};


/******************************************************************************
 GetDockList

 ******************************************************************************/

inline JPtrArray<JXDockDirector>*
JXDockManager::GetDockList()
	const
{
	return itsDockList;
}

/******************************************************************************
 IsLastDock

 ******************************************************************************/

inline JBoolean
JXDockManager::IsLastDock
	(
	JXDockDirector* dock
	)
	const
{
	return JI2B( itsDockList->GetElementCount() == 1 &&
				 itsDockList->FirstElement()    == dock );
}

/******************************************************************************
 Get DND atoms

 ******************************************************************************/

inline Atom
JXDockManager::GetDNDMinSizeAtom()
	const
{
	return itsDNDMinSizeAtom;
}

inline Atom
JXDockManager::GetDNDWindowAtom()
	const
{
	return itsDNDWindowAtom;
}

/******************************************************************************
 GetUniqueDockID

 ******************************************************************************/

inline JIndex
JXDockManager::GetUniqueDockID()
{
	return itsNextDockID;
}

/******************************************************************************
 IDUsed

 ******************************************************************************/

inline void
JXDockManager::IDUsed
	(
	const JIndex id
	)
{
	itsNextDockID = JMax(itsNextDockID, id+1);
}

/******************************************************************************
 IsReadingSetup

 ******************************************************************************/

inline JBoolean
JXDockManager::IsReadingSetup()
	const
{
	return itsIsReadingSetupFlag;
}

/******************************************************************************
 Close dock mode

 ******************************************************************************/

inline JXDockManager::CloseDockMode
JXDockManager::GetCloseDockMode()
	const
{
	return itsCloseDockMode;
}

inline void
JXDockManager::SetCloseDockMode
	(
	const CloseDockMode mode
	)
{
	itsCloseDockMode = mode;
}

#endif
