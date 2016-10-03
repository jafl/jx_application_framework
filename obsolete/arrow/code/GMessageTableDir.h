/******************************************************************************
 GMessageTableDir.h

	Interface for the GMessageTableDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GMessageTableDir
#define _H_GMessageTableDir

#include <GMManagedDirector.h>
#include <JPtrArray-JString.h>

class GMessageHeaderList;
class GMessageHeader;
class GMessageViewDir;
class JXTextMenu;
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
class GMMailboxData;
class GMessageColHeader;

class GMessageTableDir : public GMManagedDirector
{
public:

	static JBoolean Create(JXDirector* supervisor, const JString& mailfile,
							GMessageTableDir** dir, const JBoolean iconify = kJFalse);
	static JBoolean Create(JXDirector* supervisor, JFileArray& fileArray,
							GMessageTableDir** dir, const JFileVersion& version);

	GMessageTableDir(JXDirector* supervisor, const JString& mailfile);

	virtual ~GMessageTableDir();

	GMMailboxData*		GetData();

	virtual JBoolean	Close();
	GMessageTable&		GetTable();

	void				ViewMessage(GMessageHeader* header);
	void				SaveState(JFileArray& fileArray);
	void				ReadState(JFileArray& fileArray, const JFileVersion& version);
	void				UpdateMessageCount();
	void				CloseMessage(const GMessageHeader* header);

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
	virtual void	DirectorClosed(JXDirector* theDirector);

private:

	GMMailboxData*				itsData;
	GMessageTable*				itsTable;
	GMessageColHeader*			itsColHeader;
	JXTextMenu*					itsFileMenu;
	JXTextMenu*					itsInboxMenu;
	JXTextMenu*					itsPrefsMenu;
	JXTextMenu*					itsHelpMenu;
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

	GMMessageFindDialog*		itsFindDialog;

private:

	static JBoolean CreateX(JXDirector* supervisor, const JString& mailfile,
							GMessageTableDir** dir);

	void		AdjustWindowTitle();

	void		SearchMessages();

	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex index);

	void		UpdateInboxMenu();
	void		HandleInboxMenu(const JIndex index);

	void		HandlePrefsMenu(const JIndex index);
	void		HandleHelpMenu(const JIndex index);

	void		BuildWindow(const JString& mailfile);

	void		SaveSelectedMessages(ostream& os, const JBoolean headers);

	// not allowed

	GMessageTableDir(const GMessageTableDir& source);
	const GMessageTableDir& operator=(const GMessageTableDir& source);

public:

	static const JCharacter* kMessageClosed;

	class MessageClosed : public JBroadcaster::Message
		{
		public:

			MessageClosed(const GMessageHeader* header)
				:
				JBroadcaster::Message(kMessageClosed),
				itsHeader(header)
				{ };

			const GMessageHeader*
			GetHeader()
				const
				{
				return itsHeader;
				}

		private:

			const GMessageHeader*	itsHeader;
		};
};

/******************************************************************************
 GetData

 ******************************************************************************/

inline GMMailboxData*
GMessageTableDir::GetData()
{
	return itsData;
}


/******************************************************************************
 GetTable

 ******************************************************************************/

inline GMessageTable&
GMessageTableDir::GetTable()
{
	return *itsTable;
}

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GMessageTableDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}



#endif
