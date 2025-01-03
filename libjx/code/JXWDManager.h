/******************************************************************************
 JXWDManager.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWDManager
#define _H_JXWDManager

#include <jx-af/jcore/JPtrArray-JString.h>

class JXDisplay;
class JXImage;
class JXWindowDirector;
class JXWDMenu;
class JXUrgentTask;

class JXWDManager : virtual public JBroadcaster
{
	friend class JXUpdateWDMenuTask;

public:

	enum
	{
		kNoShortcutForDir = -1
	};

public:

	JXWDManager(JXDisplay* display, const bool wantShortcuts = true);

	~JXWDManager() override;

	void	GetPermanentDirectors(JPtrArray<JXWindowDirector>* directorList) const;
	void	GetDirectors(JPtrArray<JXWindowDirector>* directorList) const;

	// for use by JXWindowDirector

	void	PermanentDirectorCreated(JXWindowDirector* dir,
									 const JString& shortcut = JString::empty,
									 const JUtf8Byte* id = nullptr);
	void	DirectorCreated(JXWindowDirector* dir);
	void	DirectorDeleted(JXWindowDirector* dir);

	// for use by JXWDMenu

	void	UpdateWDMenu(JXWDMenu* menu);
	void	HandleWDMenu(JXWDMenu* menu, const JIndex index);

private:

	struct WindowInfo
	{
		JXWindowDirector*	dir;
		JString*			shortcutStr;	// nullptr unless specified in DirectorCreated()
		JInteger			shortcutIndex;	// use if shortcutStr == nullptr
		JString*			itemID;			// nullptr unless specified in DirectorCreated()

		WindowInfo()
			:
			dir(nullptr), shortcutStr(nullptr),
			shortcutIndex(kNoShortcutForDir), itemID(nullptr)
		{ };

		WindowInfo(JXWindowDirector* d)
			:
			dir(d), shortcutStr(nullptr),
			shortcutIndex(kNoShortcutForDir), itemID(nullptr)
		{ };
	};

private:

	JArray<WindowInfo>*	itsPermWindowList;
	JArray<WindowInfo>*	itsWindowList;
	const bool			itsWantShortcutFlag;
	JXUrgentTask*		itsUpdateWDMenuTask;	// nullptr unless pending

private:

	void	WDMenusNeedUpdate();
	void	UpdateAllWDMenus();
	void	ActivateDirector(const JIndex index);

	void	DirectorCreated(JArray<WindowInfo>* windowList, JXWindowDirector* dir,
							const JString& shortcut, const JUtf8Byte* id);
	bool	CalledByDirectorDeleted(JArray<WindowInfo>* windowList,
									JXWindowDirector* dir);
	void	CalledByUpdateWDMenu(const JArray<WindowInfo>& windowList, JXWDMenu* menu);

	void	GetDirectors(JArray<WindowInfo>* windowList,
						 JPtrArray<JXWindowDirector>* directorList) const;

	static bool	ShortcutUsed(const JArray<WindowInfo>& windowList,
							 const JInteger shortcutIndex);

	static std::weak_ordering
		CompareWindowNames(const WindowInfo& w1, const WindowInfo& w2);

	// not allowed

	JXWDManager(const JXWDManager&) = delete;
	JXWDManager& operator=(const JXWDManager&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kWDMenuNeedsUpdate;

	class WDMenuNeedsUpdate : public JBroadcaster::Message
		{
		public:

			WDMenuNeedsUpdate()
				:
				JBroadcaster::Message(kWDMenuNeedsUpdate)
				{ };
		};
};


/******************************************************************************
 GetPermanentDirectors

 ******************************************************************************/

inline void
JXWDManager::GetPermanentDirectors
	(
	JPtrArray<JXWindowDirector>* directorList
	)
	const
{
	return GetDirectors(itsPermWindowList, directorList);
}

/******************************************************************************
 GetDirectors

 ******************************************************************************/

inline void
JXWDManager::GetDirectors
	(
	JPtrArray<JXWindowDirector>* directorList
	)
	const
{
	return GetDirectors(itsWindowList, directorList);
}

#endif
