/******************************************************************************
 JXDockManager.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockManager
#define _H_JXDockManager

#include "JXDirector.h"
#include <JPrefObject.h>
#include <JStringMap.h>
#include <JString.h>
#include <JMinMax.h>
#include "JXDockDirector.h"		// for Windows template compile
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

	JXDockManager(JXDisplay* display, const JString& title,
				  JPrefsManager* prefsMgr = nullptr,
				  const JPrefID& id = JFAID::kInvalidID);

	virtual ~JXDockManager();

	JXDockDirector*	CreateDock(const bool splitHoriz);
	void			CloseAll();

	bool					HasDocks() const;
	JPtrArray<JXDockDirector>*	GetDockList() const;
	bool					FindDock(const JIndex id, JXDockWidget** dock);
	bool					IsLastDock(JXDockDirector* dock) const;

	virtual	bool	CanDockAll() const;
	virtual void		DockAll();

	bool	GetDefaultDock(const JUtf8Byte* windowType, JXDockWidget** dock);
	void		SetDefaultDock(const JUtf8Byte* windowType, const JXDockWidget* dock);

	bool	IsReadingSetup() const;
	void		ReadSetup(std::istream& input);
	void		WriteSetup(std::ostream& output) const;

	void		SetIcon(JXImage* icon);
	bool	CreateIcon(JXImage** icon) const;

	Atom	GetDNDMinSizeAtom() const;
	Atom	GetDNDWindowAtom() const;

	// for use by JXDockWidget

	JIndex	GetUniqueDockID();
	void	IDUsed(const JIndex id);

	CloseDockMode	GetCloseDockMode() const;
	void			SetCloseDockMode(const CloseDockMode mode);

protected:

	virtual void	DirectorClosed(JXDirector* theDirector);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

public:		// kAtomCount required at global scope

	enum
	{
		kDNDMinSizeAtomIndex,
		kDNDWindowAtomIndex,

		kAtomCount
	};

private:

	JXDisplay*					itsDisplay;
	JPtrArray<JXDockDirector>*	itsDockList;
	JStringMap<JIndex>*			itsWindowTypeMap;
	JString						itsTitle;
	JXImage*					itsWindowIcon;
	JIndex						itsNextDockIndex;
	JIndex						itsNextDockID;
	bool					itsIsReadingSetupFlag;
	CloseDockMode				itsCloseDockMode;
	Atom						itsAtoms[ kAtomCount ];

private:

	JString	GetNewDockTitle();

	// not allowed

	JXDockManager(const JXDockManager& source);
	const JXDockManager& operator=(const JXDockManager& source);
};


/******************************************************************************
 HasDocks

 ******************************************************************************/

inline bool
JXDockManager::HasDocks()
	const
{
	return !itsDockList->IsEmpty();
}

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

inline bool
JXDockManager::IsLastDock
	(
	JXDockDirector* dock
	)
	const
{
	return itsDockList->GetElementCount() == 1 &&
				 itsDockList->GetFirstElement() == dock;
}

/******************************************************************************
 Get DND atoms

 ******************************************************************************/

inline Atom
JXDockManager::GetDNDMinSizeAtom()
	const
{
	return itsAtoms[ kDNDMinSizeAtomIndex ];
}

inline Atom
JXDockManager::GetDNDWindowAtom()
	const
{
	return itsAtoms[ kDNDWindowAtomIndex ];
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

inline bool
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
