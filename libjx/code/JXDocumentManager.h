/******************************************************************************
 JXDocumentManager.h

	Copyright (C) 1997-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDocumentManager
#define _H_JXDocumentManager

#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class JXImage;
class JXDocument;
class JXFileDocument;
class JXDocumentMenu;
class JXIdleTask;
class JXUrgentTask;

class JXDocumentManager : virtual public JBroadcaster
{
	friend class JXUpdateDocMenuTask;

public:

	enum
	{
		kNoShortcutForDoc = -1
	};

	enum SafetySaveReason
	{
		kTimer,			// periodic safety save

		// following reasons all imply process termination

		kServerDead,	// X server died
		kKillSignal,	// SIGTERM: either from user or system shutdown
		kAssertFired	// assert() macro fired
	};

public:

	JXDocumentManager(const bool wantShortcuts = true);

	~JXDocumentManager() override;

	bool	WillSafetySave() const;
	void	ShouldSafetySave(const bool doIt);

	JSize	GetSafetySaveInterval() const;
	void	SetSafetySaveInterval(const JSize deltaSeconds);

	void	SafetySave(const SafetySaveReason reason);

	bool	HasDocuments() const;
	JSize	GetDocumentCount() const;
	bool	DocumentsNeedSave() const;
	bool	SaveAllFileDocuments(const bool saveUntitled);
	void	CloseDocuments();

	// for use by classes derived from JXDocument

	JString	GetNewFileName();
	void	DocumentMustStayOpen(JXDocument* doc, const bool stayOpen);
	bool	FileDocumentIsOpen(const JString& fileName,
							   JXFileDocument** doc) const;

	virtual bool	FindFile(const JString& fileName,
							 const JString& currPath,
							 JString* newFileName,
							 const bool askUser = true) const;

	// for use by JXDocument

	virtual void	DocumentCreated(JXDocument* doc);	// must call inherited
	virtual void	DocumentDeleted(JXDocument* doc);	// must call inherited
	bool			OKToCloseDocument(JXDocument* doc) const;

	const JXImage*	GetDefaultMenuIcon() const;

	// for use by JXDocumentMenu

	void	UpdateDocumentMenu(JXDocumentMenu* menu);
	void	ActivateDocument(const JIndex index);
	bool	GetDocument(const JIndex index, JXDocument** doc) const;

private:

	struct DocInfo
	{
		JXDocument*	doc;
		bool		keepOpen;
		JInteger	shortcut;

		DocInfo()
			:
			doc(nullptr), keepOpen(false), shortcut(kNoShortcutForDoc)
		{ };

		DocInfo(JXDocument*	d)
			:
			doc(d), keepOpen(false), shortcut(kNoShortcutForDoc)
		{ };
	};

	struct FileMap
	{
		JString*	oldName;
		JString*	newName;

		FileMap()
			:
			oldName(nullptr), newName(nullptr)
		{ };

		FileMap(JString* s1, JString* s2)
			:
			oldName(s1), newName(s2)
		{ };
	};

private:

	JArray<DocInfo>*	itsDocList;
	JIndex				itsNewDocCount;

	JArray<FileMap>*	itsFileMap;

	const bool			itsWantShortcutFlag;

	bool				itsPerformSafetySaveFlag;
	JXIdleTask*			itsSafetySaveTask;

	JXUrgentTask*		itsUpdateDocMenuTask;	// nullptr unless pending
	JXImage*			itsDefaultMenuIcon;		// not owned

private:

	bool	SearchFileMap(const JString& fileName,
						  JString* newFileName) const;

	void	DocumentMenusNeedUpdate();
	void	UpdateAllDocumentMenus();

	static std::weak_ordering
		CompareDocNames(const DocInfo& d1, const DocInfo& d2);

	// not allowed

	JXDocumentManager(const JXDocumentManager&) = delete;
	JXDocumentManager& operator=(const JXDocumentManager&) = delete;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDocMenuNeedsUpdate;

	class DocMenuNeedsUpdate : public JBroadcaster::Message
		{
		public:

			DocMenuNeedsUpdate()
				:
				JBroadcaster::Message(kDocMenuNeedsUpdate)
				{ };
		};
};


/******************************************************************************
 HasDocuments

 ******************************************************************************/

inline bool
JXDocumentManager::HasDocuments()
	const
{
	return !itsDocList->IsEmpty();
}

/******************************************************************************
 HasDocuments

 ******************************************************************************/

inline JSize
JXDocumentManager::GetDocumentCount()
	const
{
	return itsDocList->GetItemCount();
}

/******************************************************************************
 WillSafetySave

 ******************************************************************************/

inline bool
JXDocumentManager::WillSafetySave()
	const
{
	return itsPerformSafetySaveFlag;
}

/******************************************************************************
 GetDefaultMenuIcon

 ******************************************************************************/

inline const JXImage*
JXDocumentManager::GetDefaultMenuIcon()
	const
{
	return itsDefaultMenuIcon;
}

#endif
