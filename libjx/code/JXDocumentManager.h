/******************************************************************************
 JXDocumentManager.h

	Copyright © 1997-2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDocumentManager
#define _H_JXDocumentManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>
#include <JString.h>

class JXImage;
class JXDocument;
class JXFileDocument;
class JXTimerTask;
class JXDocumentMenu;
class JXUpdateDocMenuTask;

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

	JXDocumentManager(const JBoolean wantShortcuts = kJTrue);

	virtual ~JXDocumentManager();

	JBoolean	WillSafetySave() const;
	void		ShouldSafetySave(const JBoolean doIt);

	JSize		GetSafetySaveInterval() const;
	void		SetSafetySaveInterval(const JSize deltaSeconds);

	void		SafetySave(const SafetySaveReason reason);

	void		CloseDocuments();

	// for use by classes derived from JXDocument

	const JString&		GetNewFileName();
	void				DocumentMustStayOpen(JXDocument* doc, const JBoolean stayOpen);
	JBoolean			FileDocumentIsOpen(const JCharacter* fileName,
										   JXFileDocument** doc) const;

	virtual JBoolean	FindFile(const JCharacter* fileName,
								 const JCharacter* currPath,
								 JString* newFileName,
								 const JBoolean askUser = kJTrue) const;

	// for use by JXDocument

	virtual void	DocumentCreated(JXDocument* doc);	// must call inherited
	virtual void	DocumentDeleted(JXDocument* doc);	// must call inherited
	JBoolean		OKToCloseDocument(JXDocument* doc) const;

	const JXImage*	GetDefaultMenuIcon() const;

	// for use by JXDocumentMenu

	void		UpdateDocumentMenu(JXDocumentMenu* menu);
	void		ActivateDocument(const JIndex index);
	JBoolean	GetDocument(const JIndex index, JXDocument** doc) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct DocInfo
	{
		JXDocument*	doc;
		JBoolean	keepOpen;
		JInteger	shortcut;

		DocInfo()
			:
			doc(NULL), keepOpen(kJFalse), shortcut(kNoShortcutForDoc)
		{ };

		DocInfo(JXDocument*	d)
			:
			doc(d), keepOpen(kJFalse), shortcut(kNoShortcutForDoc)
		{ };
	};

	struct FileMap
	{
		JString*	oldName;
		JString*	newName;

		FileMap()
			:
			oldName(NULL), newName(NULL)
		{ };

		FileMap(JString* s1, JString* s2)
			:
			oldName(s1), newName(s2)
		{ };
	};

private:

	JArray<DocInfo>*		itsDocList;

	JString					itsLastNewFileName;		// so we can return const JString&
	JIndex					itsNewDocCount;

	JArray<FileMap>*		itsFileMap;

	const JBoolean			itsWantShortcutFlag;

	JBoolean				itsPerformSafetySaveFlag;
	JXTimerTask*			itsSafetySaveTask;

	JXUpdateDocMenuTask*	itsUpdateDocMenuTask;	// NULL unless pending
	JXImage*				itsDefaultMenuIcon;

private:

	JBoolean	SearchFileMap(const JCharacter* fileName,
							  JString* newFileName) const;

	void	DocumentMenusNeedUpdate();
	void	UpdateAllDocumentMenus();

	static JOrderedSetT::CompareResult
		CompareDocNames(const DocInfo& d1, const DocInfo& d2);

	// not allowed

	JXDocumentManager(const JXDocumentManager& source);
	const JXDocumentManager& operator=(const JXDocumentManager& source);

public:

	// JBroadcaster messages

	static const JCharacter* kDocMenuNeedsUpdate;

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
 WillSafetySave

 ******************************************************************************/

inline JBoolean
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
