/******************************************************************************
 JXSharedPrefsManager.h

	Copyright (C) 2000 John Lindal.

 ******************************************************************************/

#ifndef _H_JXSharedPrefsManager
#define _H_JXSharedPrefsManager

#include "jx-af/jx/JXTEBase.h"	// need defn of PartialWordModifier
#include "jx-af/jx/JXMenu.h"		// need defn of Style
#include <jx-af/jcore/jTime.h>

class JPrefsFile;
class JXTimerTask;
class JXSharedPrefObject;

class JXSharedPrefsManager : virtual public JBroadcaster
{
public:

	JXSharedPrefsManager();

	virtual ~JXSharedPrefsManager();

	bool	WasNew() const;

	void	ReadPrefs(JXSharedPrefObject* obj);
	void	WritePrefs(const JXSharedPrefObject* obj);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	// do not change or deallocate ID's once they are assigned

	enum
	{
		kWriteLockID = 0,
		kWMPlacementStrategyID,		// unused
		kCopyWhenSelectID,
		kPWModifierID,
		kCaretFollowsScrollID,
		kWindowsHomeEndID,
		kLatestHelpMgrVersionID,
		kHelpMgrV3,					// unused
		kHelpMgrV4,					// unused
		kLatestWebBrowserVersionID,
		kWebBrowserV0,
		kAllowSpaceID,
		kWebBrowserV1,
		kMenuDisplayStyleID,
		kLatestSpellCheckerVersionID,
		kSpellCheckerV0,
		kFocusFollowsCursorInDockID,
		kMiddleClickWillPasteID
	};

private:

	JPrefsFile*		itsFile;			// nullptr unless working
	bool			itsChangedFlag;		// used when re-entrant
	JXTimerTask*	itsUpdateTask;
	JString			itsSignalFileName;	// can be empty
	time_t			itsSignalModTime;	// last time signal file was modified
	bool			itsWasNewFlag;		// true => we created the file

	// this section exists because JCore values cannot update themselves

	bool							itsOrigFocusInDockFlag;
	bool							itsOrigCopyWhenSelectFlag;
	bool							itsOrigMiddleClickWillPasteFlag;
	JXTEBase::PartialWordModifier	itsOrigPWMod;
	bool							itsOrigCaretScrollFlag;
	bool							itsOrigWindowsHomeEndFlag;
	bool							itsOrigAllowSpaceFlag;
	JXMenu::Style					itsOrigMenuDisplayStyle;

private:

	bool	Update();
	bool	GetAll(bool* isNew);

	void	PrivateSetFocusFollowsCursorInDock();
	void	PrivateSetCopyWhenSelectFlag();
	void	PrivateSetMiddleClickWillPasteFlag();
	void	PrivateSetPartialWordModifier();
	void	PrivateSetCaretFollowsScroll();
	void	PrivateSetWindowsHomeEnd();
	void	PrivateSetAllowSpaceFlag();
	void	PrivateSetMenuDisplayStyle();

	bool	PrivateReadPrefs(JXSharedPrefObject* obj);
	bool	PrivateWritePrefs(const JXSharedPrefObject* obj);

	bool	Open();
	void	Close(const bool changed);
	void	NotifyChanged();
	void	SaveSignalModTime();

	// not allowed

	JXSharedPrefsManager(const JXSharedPrefsManager&) = delete;
	JXSharedPrefsManager& operator=(const JXSharedPrefsManager&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kRead;

	class Read : public JBroadcaster::Message
		{
		public:

			Read()
				:
				JBroadcaster::Message(kRead)
				{ };
		};
};


/******************************************************************************
 WasNew

	Returns true if prefs stored by the application should be used.

 ******************************************************************************/

inline bool
JXSharedPrefsManager::WasNew()
	const
{
	return itsWasNewFlag;
}

#endif
