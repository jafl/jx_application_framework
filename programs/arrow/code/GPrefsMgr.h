/******************************************************************************
 GPrefsMgr.h

	Interface for the GPrefsMgr class

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GPrefsMgr
#define _H_GPrefsMgr

#include <JXPrefsManager.h>
#include <JPtrArray-JString.h>
#include <JFontStyle.h>
#include <JXPTPrinter.h>

class GMailPrefsDialog;
class JXWindow;

enum
{
	kGDefaultInboxID = 1,
	kGInboxesID,
	kGSMTPHostID,
	kGPOPServerID,
	kGUsePopID,
	kGPopLeaveOnServerID,
	kGPopUserNameID,
	kGPopPasswdID,
	kGPopRememberPasswdID,
	kGCurrentOutboxID,
	kGDefaultMonoFontID,
	kGShowStateInTitleID,
	kGProgramVersionID,
	kGUseAPopID,
	kGUIDListID,
	kAutoInsertQuoteID,
	kAutoCheckMailID,
	kCheckMailDelayID,
	kSMTPUserID,
	kFindDialogPrefsID,
	kChooseSaveDialogPrefsID,
	kMailTreePrefsID,
	kAddressTreePrefsID,
	kGReplyToID,
	kGShowSenderStringID,
	kGSenderStringID,
	kPTPrinterSetupID,
	kGHelpSetupID,
	kGTableToolBarID,
	kGViewToolBarID,
	kGEditToolBarID,
	kGAddressToolBarID,
	kGMailboxToolBarID,
	kGTableWindowSizeID,
	kGViewWindowSizeID,
	kGEditorWindowSizeID,
	kGUseCustomFromLineID,
	kGCustomFromLineID,
	kGDefaultFontSizeID,
	kGTabCharWidthID,
	kGMaxQuoteID,
	kUnused1ID,
	kGAltChooseSaveSetupID,
	kGSpecialPTPrintSetupID,
	kUnused2ID,
	kGBeepOnNewMailID,
	kGEncryptionTypeID,
	kGAccountMgrID,
	kGEditFilterDialogID,
	kGFilterMgrID,
	kGFilterFileID,
	kGTableWindowPrefsID,
	kGViewWindowPrefsID,
	kGEditorWindowPrefsID,
	kGFindDirToolBarID,
	kGFindDirWindowPrefsID,
	kGOpenMailboxWindowPrefsID,
	kGVersionCheckID
};


class GMessageTableDir;
class GMessageViewDir;
class GMessageEditDir;
class GMFindTableDir;

class GPrefsMgr : public JXPrefsManager
{
public:
	enum EncryptionType
	{
		kPGP2_6 = 1,
		kGPG1_0
	};

public:

	GPrefsMgr(JBoolean*	isNew);
	virtual ~GPrefsMgr();

	void		GetInboxes(JPtrArray<JString>& inboxes);
	void		AddInbox(const JString& inbox);
	void		DeleteInbox(const JString& inbox);
	void		DeleteAllInboxes();

	JString		GetSMTPHost();
	void		SetSMTPHost(const JString& host);

	JString		GetSMTPUser();
	void		SetSMTPUser(const JString& user);

	JBoolean	ShowingStateInTitle();
	void		ShowStateInTitle(const JBoolean show);

	JString		GetPopServer();
	void		SetPopServer(const JString& pop);

	JBoolean	UsingPopServer();
	void		UsePopServer(const JBoolean pop);

	JBoolean	LeaveOnServer();
	void		SetLeaveOnServer(const JBoolean leave);

	JString		GetPopUserName();
	void		SetPopUserName(const JString& name);

	JString		GetPopPasswd();
	void		SetPopPasswd(const JString& passwd);

	JBoolean	RememberPasswd();
	void		SetRememberPasswd(const JBoolean leave);

	JBoolean	UsingAPop();
	void		UseAPop(const JBoolean pop);

	JBoolean	AutoCheckingMail();
	void		AutoCheckMail(const JBoolean check);

	JInteger	CheckMailDelay();
	void		SetCheckMailDelay(const JInteger delay);

	JString		GetReplyTo();
	void		SetReplyTo(const JString& replyto);

	JBoolean	HasUIDList() const;
	void		GetUIDList(JPtrArray<JString>& list);
	void		SetUIDList(const JPtrArray<JString>& list);
	void		ClearUIDList();

	JBoolean	AutoInsertingQuote();
	void		AutoInsertQuote(const JBoolean insert);

	JString		GetCurrentOutbox();
	void		SetCurrentOutbox(const JString& mbox);

	JString		GetDefaultInbox();
	void		SetDefaultInbox(const JString& mbox);

	JFontID		GetDefaultMonoFontID();
	JString		GetDefaultMonoFont();
	void		SetDefaultMonoFont(const JCharacter* fontName);

	JSize		GetDefaultFontSize();
	void		SetDefaultFontSize(const JSize size);

	JSize		GetTabCharWidth();
	void		SetTabCharWidth(const JSize size);

	JBoolean	ShowingSenderString();
	void		ShowSenderString(const JBoolean show);

	JString		GetSenderString();
	void		SetSenderString(const JString& senderString);

	JBoolean	UsingCustomFromLine();
	void		UseCustomFromLine(const JBoolean useFrom);

	JBoolean	IsBeepingOnNewMail();
	void		BeepOnNewMail(const JBoolean beep);

	JBoolean	IsOpeningMailboxWindow() const;
	void		ShouldBeOpeningMailboxWindow(const JBoolean open);

	JString		GetCustomFromString();
	void		SetCustomFromString(const JString& fromLine);

	JSize		GetMaxQuote();
	void		SetMaxQuote(const JSize max);

	JString		GetFilterFile();
	void		SetFilterFile(const JString& filterFile);

	EncryptionType	GetEncryptionType() const;
	void			SetEncryptionType(const EncryptionType type);

	void		EditPrefs();
	JString		GetArrowVersionStr() const;

	void		ReadChooseSaveDialogPrefs();
	void		WriteChooseSaveDialogPrefs();

	void		WritePrinterSetup(JXPTPrinter* printer);
	void		ReadPrinterSetup(JXPTPrinter* printer);

	void		GetTableWindowSize(JXWindow* window) const;
	void		SaveTableWindowSize(JXWindow* window);

	void		GetViewWindowSize(JXWindow* window) const;
	void		SaveViewWindowSize(JXWindow* window);

	void		GetEditorWindowSize(JXWindow* window) const;
	void		SaveEditorWindowSize(JXWindow* window);

	void		GetTableWindowPrefs(GMessageTableDir* dir);
	void		SetTableWindowPrefs(GMessageTableDir* dir);

	void		GetFindDirWindowPrefs(GMFindTableDir* dir);
	void		SetFindDirWindowPrefs(GMFindTableDir* dir);

	void		GetViewWindowPrefs(GMessageViewDir* dir);
	void		SetViewWindowPrefs(GMessageViewDir* dir);

	void		GetEditorWindowPrefs(GMessageEditDir* dir);
	void		SetEditorWindowPrefs(GMessageEditDir* dir);

	static JOrderedSetT::CompareResult
		CompareFileNames(JString* const & n1, JString* const & n2);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	GMailPrefsDialog*	itsDialog;

private:

	void	GetWindowSize(const JPrefID id, JXWindow* window) const;
	void	SaveWindowSize(const JPrefID id, JXWindow* window);
	void	SetInboxes(const JPtrArray<JString>& inboxes);

	// not allowed

	GPrefsMgr(const GPrefsMgr& source);
	const GPrefsMgr& operator=(const GPrefsMgr& source);

public:

	static const JCharacter* kInboxesChanged;
	static const JCharacter* kInboxAdded;
	static const JCharacter* kInboxRemoved;
	static const JCharacter* kShowStateChanged;
	static const JCharacter* kPopChanged;
	static const JCharacter* kFontChanged;

	class InboxesChanged : public JBroadcaster::Message
		{
		public:

			InboxesChanged()
				:
				JBroadcaster::Message(kInboxesChanged)
				{ };
		};

	class InboxAdded : public JBroadcaster::Message
		{
		public:

			InboxAdded(const JIndex index)
				:
				JBroadcaster::Message(kInboxAdded),
				itsIndex(index)
				{ };

			JIndex
			GetIndex() const
				{
				return itsIndex;
				}

		private:

			JIndex itsIndex;
		};

	class InboxRemoved : public JBroadcaster::Message
		{
		public:

			InboxRemoved(const JIndex index)
				:
				JBroadcaster::Message(kInboxRemoved),
				itsIndex(index)
				{ };

			JIndex
			GetIndex() const
				{
				return itsIndex;
				}

		private:

			JIndex itsIndex;
		};

	class ShowStateChanged : public JBroadcaster::Message
		{
		public:

			ShowStateChanged()
				:
				JBroadcaster::Message(kShowStateChanged)
				{ };
		};

	class PopChanged : public JBroadcaster::Message
		{
		public:

			PopChanged()
				:
				JBroadcaster::Message(kPopChanged)
				{ };
		};

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };
		};

};

#endif
