/******************************************************************************
 GMessageViewDir.h

	Interface for the GMessageViewDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GMessageViewDir
#define _H_GMessageViewDir

#include <GMManagedDirector.h>
#include <JString.h>

class GMessageHeader;
class GMessageView;
class JXTextButton;
class JXTextMenu;
class JXScrollbarSet;
class GMessageTableDir;
class JXGetStringDialog;
class JXVertPartition;
class JXHorizPartition;
class JXToolBar;
class JXImage;
class GMAttachmentTable;
class JXFSDirMenu;

class GMessageViewDir : public GMManagedDirector
{
public:

	GMessageViewDir(GMessageTableDir* supervisor, const JString& mailfile,
							GMessageHeader* header, const JBoolean source = kJFalse);
	virtual ~GMessageViewDir();

	GMessageHeader*	GetMessageHeader();
	void			SaveState(std::ostream& os);
	void			ReadState(std::istream& is, const JFileVersion& version);
	void			WriteWindowPrefs(std::ostream& os);
	void			ReadWindowPrefs(std::istream& is);

	virtual JXImage*	GetMenuIcon() const;
	GMessageTableDir*	GetDir();

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	GMessageTableDir*	itsDir;
	GMessageView*		itsView;
	GMessageView*		itsHeader;
	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsMessageMenu;
	JXTextMenu*			itsPrefsMenu;
	JXTextMenu*			itsHelpMenu;

	JString				itsFrom;
	JString				itsTo;
	JString				itsReplyTo;
	JString				itsDate;
	JString				itsCC;
	JString				itsSubject;
	JString				itsFullHeader;

	JXScrollbarSet*		itsSBSet;
	JBoolean			itsShowFullHeaders;
	GMessageHeader*		itsMessageHeader;

	JXGetStringDialog*	itsPasswdDialog;
	JXVertPartition*	itsPart;
	GMAttachmentTable*	itsAttachTable;

	JXToolBar*			itsToolBar;
	JXImage*			itsMenuIcon;
	JBoolean			itsSourceOnly;
	JBoolean			itsPrefsIncludeAttachTable;
	JCoordinate			itsAttachTableHeight;

	JXFSDirMenu*		itsTransferMenu;
	JXFSDirMenu*		itsCopyMenu;

private:

	void		HandleFileMenu(const JIndex index);
	void		UpdateFileMenu();

	void		HandleMessageMenu(const JIndex index);
	void		UpdateMessageMenu();

	void		HandlePrefsMenu(const JIndex index);
	void		HandleHelpMenu(const JIndex index);

	void		ShowNext();
	void		ShowPrev();
	void		ShowHeader(GMessageHeader* header);

	void		FixHeaderForReply(JString* sub);

	void		AppendMessagesToFile(const JBoolean headers);

	void		HandleMessageTransfer(const JCharacter* dest, const JBoolean move);

	void		AdjustView(const JBoolean showFull, const JBoolean firstTime = kJFalse);

	void		PasteClean(const JString& buffer);

	void		BuildWindow(const JString& mailfile);


	// not allowed

	GMessageViewDir(const GMessageViewDir& source);
	const GMessageViewDir& operator=(const GMessageViewDir& source);

};

/******************************************************************************
 GetMessageHeader

 ******************************************************************************/

inline GMessageHeader*
GMessageViewDir::GetMessageHeader()
{
	return itsMessageHeader;
}

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GMessageViewDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}

/******************************************************************************
 GetDir (public)

 ******************************************************************************/

inline GMessageTableDir*
GMessageViewDir::GetDir()
{
	return itsDir;
}

#endif
