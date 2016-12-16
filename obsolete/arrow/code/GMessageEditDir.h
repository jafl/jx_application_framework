/******************************************************************************
 GMessageEditDir.h

	Interface for the GMessageEditDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GMessageEditDir
#define _H_GMessageEditDir

#include <GMManagedDirector.h>

#include <GMAccount.h>
#include <GMessageHeader.h>
#include <JPtrArray-JString.h>

class JXTextMenu;
class JXStaticText;
class JXInputField;
class GMessageEditor;
class JXScrollbarSet;
class JXTextButton;
class JXGetStringDialog;
class SMTPMessage;
class JXToolBar;
class JXImage;
class GMAttachmentTable;
class JXVertPartition;
class GMAccountMenu;

class GMessageEditDir : public GMManagedDirector
{
public:

	GMessageEditDir(JXDirector* supervisor);
	virtual ~GMessageEditDir();

	void				SetTo(const JString& to);
	void				SetCC(const JString& cc);
	void				SetSubject(const JString& subject);
	void				SetText(const JString& filename);
	void				SetSentFrom(const JString& sentfrom);
	void				SetSentDate(const JString& date);
	void				Send();
	virtual JBoolean	Close();
	void				InsertText(const JBoolean marked);
	void				SaveState(std::ostream& os);
	void				ReadState(std::istream& is, const JFileVersion& version);
	void				SetReplyType(GMessageHeader::ReplyStatus status);
	void				SetHeader(GMessageHeader* header, GMessageHeader::ReplyStatus status);
	void				SetAccount(GMAccount* account);

	void				CheckForPendingMessage();

	virtual JXImage*	GetMenuIcon() const;

	void				FileDropped(const JString& filename, const JBoolean local);

	void			WriteWindowPrefs(std::ostream& os);
	void			ReadWindowPrefs(std::istream& is);

protected:

	virtual void	Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	JXStaticText*	itsToLabel;
	JXStaticText*	itsReplyToLabel;
	JXStaticText*	itsCCLabel;
	JXStaticText*	itsBCCLabel;
	JXStaticText*	itsSubjectLabel;

	JXInputField*	itsToInput;
	JXInputField*	itsReplyToInput;
	JXInputField*	itsCCInput;
	JXInputField*	itsBCCInput;
	JXInputField*	itsSubjectInput;

	JXScrollbarSet*	itsEditorSet;
	GMessageEditor*	itsEditor;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsMessageMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;
	JBoolean		itsShowReplyTo;
	JBoolean		itsShowCC;
	JBoolean		itsShowBCC;

	JString*		itsReplyString;
	JString*		itsReplyMark;
	JString*		itsSentFrom;
	JString*		itsSentDate;
	JString*		itsSentTextFileName;

	JXTextButton*	itsSendButton;

	JXGetStringDialog*	itsPasswdDialog;
	SMTPMessage*		itsMessage;
	JString				itsFileName;

	JBoolean		itsShowReplyString;

	JXToolBar*		itsToolBar;

	JXImage*		itsMenuIcon;

	GMAttachmentTable*	itsAttachTable;
	JXVertPartition*	itsPart;

	GMessageHeader*		itsHeader;
	GMessageHeader::ReplyStatus	itsStatus;

	GMAccount*			itsAccount;
	GMAccountMenu*		itsAccountMenu;

	JCoordinate			itsAttachTableHeight;

	JString				itsSig;
	GMAccount::SigType	itsSigType;

private:


	void		UpdateFileMenu();
	void		HandleFileMenu(const JIndex index);

	void		UpdateMessageMenu();
	void		HandleMessageMenu(const JIndex index);

	void		HandlePrefsMenu(const JIndex index);
	void		HandleHelpMenu(const JIndex index);

	void		BuildWindow();

	void		AdjustInputFields();
	void		AdjustWindowTitle();
	void		QuoteLineRange(const JIndex linestart,
								const JIndex lineend);

	// not allowed

	GMessageEditDir(const GMessageEditDir& source);
	const GMessageEditDir& operator=(const GMessageEditDir& source);

};

/******************************************************************************
 GetMenuIcon (public)

 ******************************************************************************/

inline JXImage*
GMessageEditDir::GetMenuIcon()
	const
{
	return itsMenuIcon;
}


#endif
