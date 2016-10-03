/******************************************************************************
 GMFindTableDir.h

	Interface for the GMFindTableDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GMFindTableDir
#define _H_GMFindTableDir

#include <GMManagedDirector.h>
#include <JPtrArray-JString.h>

class GMessageFrom;
class GMessageHeaderList;
class GMessageHeader;
class GMessageViewDir;
class JDirEntry;
class JXTextMenu;
class GMessageDirUpdateTask;
class JXWindow;
class JXImage;
class JXStaticText;
class GXBlockingPG;
class JXProgressIndicator;
class GMessageTable;
class JXScrollbarSet;
class JFileArray;
class GMDummyText;
class JXToolBar;
class JXFSDirMenu;
class GMMessageFindDialog;
class GMessageTableDir;
class GMMailboxData;

class GMFindTableDir : public GMManagedDirector
{
public:

	GMFindTableDir(GMessageTableDir* supervisor, GMMailboxData* data, GMessageHeaderList* list);
	virtual ~GMFindTableDir();

	GMessageTable&		GetTable();

	virtual JXImage*	GetMenuIcon() const;

	void				ReadWindowPrefs(istream& is);
	void				WriteWindowPrefs(ostream& os);

	// The following need to be accessed by GMessageTable

	void		HandleMessageTransfer(const JCharacter* dest, const JBoolean move);
	void		SaveSelectedMessages(const JBoolean headers = kJFalse);
	void		AppendSelectedMessages(const JBoolean headers = kJFalse);
	void		PrintSelectedMessages(const JBoolean headers = kJFalse);

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
									
private:

	GMessageTable*				itsTable;
	JString						itsMailFile;
	JPtrArray<GMessageFrom>*	itsFromList;
	GMMailboxData*				itsData;
	JDirEntry*					itsEntry;
	JXTextMenu*					itsFileMenu;
	JXTextMenu*					itsInboxMenu;
	JXTextMenu*					itsPrefsMenu;
	JXTextMenu*					itsHelpMenu;
	GMessageDirUpdateTask*		itsIdleTask;
	JPtrArray<GMessageViewDir>*	itsViewDirs;

	JBoolean					itsNeedsUpdate;
	JBoolean					itsNeedsSave;
	JXWindow*					itsWindow;

	JXImage*					itsNoMailIcon;
	JXImage*					itsNewMailIcon;
	JXImage*					itsSmallNewMailIcon;
	JXImage*					itsHaveMailIcon;

	JXStaticText*				itsPath;
	JXStaticText*				itsMessageCount;

	GXBlockingPG*				itsPG;
	JXProgressIndicator*		itsIndicator;
	JXScrollbarSet*				itsSBSet;

	JBoolean					itsHasNewMail;
	GMDummyText*				itsDummyPrintText;
	JXToolBar*					itsToolBar;
	JXImage*					itsMenuIcon;
	JString						itsInternalMessage;

	JXFSDirMenu*				itsTransferMenu;
	JXFSDirMenu*				itsCopyMenu;

	GMessageTableDir*			itsDir;

private:

	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex index);

	void		HandlePrefsMenu(const JIndex index);
	void		HandleHelpMenu(const JIndex index);

	void		BuildWindow();

	void		SaveSelectedMessages(ostream& os, const JBoolean headers);

	// not allowed

	GMFindTableDir(const GMFindTableDir& source);
	const GMFindTableDir& operator=(const GMFindTableDir& source);

};

/******************************************************************************
 GetTable

 ******************************************************************************/

inline GMessageTable&
GMFindTableDir::GetTable()
{
	return *itsTable;
}

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GMFindTableDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}



#endif
