/******************************************************************************
 GPrefsMgr.cc

	BASE CLASS = public JXPrefsManager

	Copyright (C) 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GPrefsMgr.h>
#include <GMailPrefsDialog.h>
#include <GMessageTableDir.h>
#include <GMessageViewDir.h>
#include <GMessageEditDir.h>
#include <GMFindTableDir.h>
#include <GMGlobals.h>

#include <JXFontManager.h>
#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXChooseSaveFile.h>
#include <JXHelpManager.h>
#include <JXWebBrowser.h>
#include <JXSpellChecker.h>
#include <JXWindow.h>

#include <jSysUtil.h>
#include <jDirUtil.h>
#include <unistd.h>
#include <stdlib.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsFileVersion	= 10;

const JCharacter* GPrefsMgr::kInboxesChanged	= "GPrefsMgr::kInboxesChanged";
const JCharacter* GPrefsMgr::kInboxAdded		= "GPrefsMgr::kInboxAdded";
const JCharacter* GPrefsMgr::kInboxRemoved		= "GPrefsMgr::kInboxRemoved";
const JCharacter* GPrefsMgr::kShowStateChanged	= "GPrefsMgr::kShowStateChanged";
const JCharacter* GPrefsMgr::kPopChanged		= "GPrefsMgr::kPopChanged";
const JCharacter* GPrefsMgr::kFontChanged		= "GPrefsMgr::kFontChanged";

const JCharacter* kGMDefaultMailtoString		= "arrow --mailto \"$a\"";

const JSize kGDefaultFontSize					= kJDefaultFontSize;
const JSize	kGDefaultTabCharWidth				= 8;
const JSize kGDefaultMaxQuote					= 10;

/******************************************************************************
 Constructor

 *****************************************************************************/

GPrefsMgr::GPrefsMgr
	(
	JBoolean* isNew
	)
	:
   JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	itsDialog	= NULL;

	*isNew = JPrefsManager::UpgradeData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPrefsMgr::~GPrefsMgr()
{
	SetData(kGProgramVersionID, GMGetVersionNumberStr());
	SaveToDisk();
}

/******************************************************************************
 GetJCCVersionStr

 ******************************************************************************/

JString
GPrefsMgr::GetArrowVersionStr()
	const
{
	std::string data;
	if (GetData(kGProgramVersionID, &data))
		{
		return JString(data);
		}
	else
		{
		return JString("< 0.5.0");		// didn't exist before this version
		}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
GPrefsMgr::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		JString homeDir;
		JBoolean fileok = JGetHomeDirectory(&homeDir);
		JAppendDirSeparator(&homeDir);
		JString outbox = homeDir + ".outbox";
		SetCurrentOutbox(outbox);

		JString username = JGetUserName();

		JString inbox = "/var/spool/mail/" + username;
		SetDefaultInbox(inbox);

		ShowStateInTitle(kJTrue);
		SetPopServer(JString("pop-server"));
		UsePopServer(kJFalse);
		SetLeaveOnServer(kJFalse);
		SetPopUserName(JString("name"));
		SetPopPasswd(JString(""));
		SetRememberPasswd(kJFalse);
		UseAPop(kJFalse);
		AutoInsertQuote(kJTrue);
		AutoCheckMail(kJFalse);
		SetCheckMailDelay(30);
		SetDefaultMonoFont(JGetMonospaceFontName());
		JCharacter* var = getenv("REPLYTO");
		if (var != NULL)
			{
			JString varStr(var);
			SetReplyTo(varStr);
			}
		ShowSenderString(kJTrue);
		SetSenderString("On $d, $f wrote:");
		JXGetWebBrowser()->SetComposeMailCmd(kGMDefaultMailtoString);
		SetFilterFile(".procmailrc");
		}
	else
		{
		if (currentVersion < 2)
			{
			ShowStateInTitle(kJTrue);
			}
		if (currentVersion < 3)
			{
			UseAPop(kJFalse);
			AutoInsertQuote(kJTrue);
			AutoCheckMail(kJFalse);
			SetCheckMailDelay(30);
			}
		if (currentVersion < 4)
			{
			JCharacter* var = getenv("REPLYTO");
			if (var != NULL)
				{
				JString varStr(var);
				SetReplyTo(varStr);
				}
			ShowSenderString(kJTrue);
			SetSenderString("On $d, $f wrote:");
			}
		if (currentVersion < 5)
			{
			RemoveData(kGHelpSetupID);
			JXGetWebBrowser()->SetComposeMailCmd(kGMDefaultMailtoString);
			}
		if (currentVersion < 7)
			{
			SetFilterFile(".procmailrc");
			}
		if (currentVersion < 10)
			{
			RemoveData(42);
			RemoveData(45);
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPrefsMgr::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			JBoolean showstate;
			JBoolean autoQuote;
			JBoolean showSender;
			JString	 senderString;
			JString  fontname;
			JSize    fontsize;
			JSize	 maxQuote;
			JBoolean report;
			JSize	 spacesPerTab;
			JBoolean beeping;
			JIndex	 encType;
			JString	 filterFile;
			JString  defInbox;
			JBoolean openMailboxWindow;
			itsDialog->GetValues(&showstate, &autoQuote,
									&showSender, &senderString,
									&fontname, &fontsize,
									&maxQuote, &report, &spacesPerTab, &beeping,
									&encType, &filterFile, &defInbox,
									&openMailboxWindow);
			ShowStateInTitle(showstate);
			AutoInsertQuote(autoQuote);
			ShowSenderString(showSender);
			SetSenderString(senderString);
			SetDefaultMonoFont(fontname);
			SetDefaultFontSize(fontsize);
			SetMaxQuote(maxQuote);
			(JXGetSpellChecker())->ShouldReportNoErrors(report);
			SetTabCharWidth(spacesPerTab);
			BeepOnNewMail(beeping);
			SetEncryptionType((EncryptionType)encType);
			SetFilterFile(filterFile);
			SetDefaultInbox(defInbox);
			ShouldBeOpeningMailboxWindow(openMailboxWindow);
			}
		itsDialog = NULL;
		}
}

/******************************************************************************
 GetInboxes

 ******************************************************************************/

void
GPrefsMgr::GetInboxes
	(
	JPtrArray<JString>& inboxes
	)
{
	if (IDValid(kGInboxesID))
		{
		std::string data;
		const JBoolean ok = GetData(kGInboxesID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JSize count;
		dataStream >> count;

		inboxes.SetCompareFunction(CompareFileNames);

		for (JIndex i=1; i<=count; i++)
			{
			JString* str = new JString;
			assert( str != NULL );
			dataStream >> *str;
			if (!inboxes.InsertSorted(str, kJFalse))
				{
				delete str;
				}
			}
		}
}

/******************************************************************************
 SetInboxes

 ******************************************************************************/

void
GPrefsMgr::SetInboxes
	(
	const JPtrArray<JString>& inboxes
	)
{
	const JSize count = inboxes.GetElementCount();

	std::ostringstream data;
	data << count;

	for (JIndex i=1; i<=count; i++)
		{
		data << ' ' << *(inboxes.GetElement(i));
		}

	SetData(kGInboxesID, data);
	Broadcast(InboxesChanged());
}

/******************************************************************************
 AddInbox

 ******************************************************************************/

void
GPrefsMgr::AddInbox
	(
	const JString& inbox
	)
{
	JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
	GetInboxes(inboxes);
	JString* str = new JString(inbox);
	assert(str != NULL);
	JIndex index;
	inboxes.SetCompareFunction(CompareFileNames);
	JBoolean found = inboxes.SearchSorted(str, JOrderedSetT::kAnyMatch, &index);
	if (!found)
		{
		if (inboxes.InsertSorted(str, kJFalse, &index))
			{
			SetInboxes(inboxes);
			Broadcast(InboxAdded(index));
			}
		else
			{
			delete str;
			}
		}
	else
		{
		delete str;
		}
}

/******************************************************************************
 DeleteInbox

 ******************************************************************************/

void
GPrefsMgr::DeleteInbox
	(
	const JString& inbox
	)
{
	JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
	GetInboxes(inboxes);
	JIndex index;
	JString str(inbox);
	inboxes.SetCompareFunction(CompareFileNames);
	if (inboxes.SearchSorted(&str, JOrderedSetT::kAnyMatch, &index))
		{
		Broadcast(InboxRemoved(index));
		}
	JBoolean found = kJFalse;
	for (JSize i = 1; i <= inboxes.GetElementCount(); i++)
		{
		if (*(inboxes.GetElement(i)) == inbox)
			{
			inboxes.DeleteElement(i);
			found = kJTrue;
			}
		}
	if (found)
		{
		SetInboxes(inboxes);
		}
}

/******************************************************************************
 DeleteAllInboxes

 ******************************************************************************/

void
GPrefsMgr::DeleteAllInboxes()
{
	JPtrArray<JString> inboxes(JPtrArrayT::kForgetAll);
	GetInboxes(inboxes);
	if (inboxes.GetElementCount() == 0)
		{
		return;
		}
	inboxes.DeleteAll();
	SetInboxes(inboxes);
}

/******************************************************************************
 GetSMTPHost

 ******************************************************************************/

JString
GPrefsMgr::GetSMTPHost()
{
	if (IDValid(kGSMTPHostID))
		{
		std::string data;
		const JBoolean ok = GetData(kGSMTPHostID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JString host;
		dataStream >> host;
		if (!host.IsEmpty())
			{
			return host;
			}
		}
	JCharacter hostname[255];
	gethostname(hostname, 255);
	JString host(hostname);
	return host;
}

/******************************************************************************
 SetSMTPHost

 ******************************************************************************/

void
GPrefsMgr::SetSMTPHost
	(
	const JString& host
	)
{
	std::ostringstream data;
	data << host;

	SetData(kGSMTPHostID, data);
}

/******************************************************************************
 GetSMTPUser

 ******************************************************************************/

JString
GPrefsMgr::GetSMTPUser()
{
	if (IDValid(kSMTPUserID))
		{
		std::string data;
		const JBoolean ok = GetData(kSMTPUserID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JString user;
		dataStream >> user;
		if (!user.IsEmpty())
			{
			return user;
			}
		}
	JCharacter* name = getenv("LOGNAME");
	if (name == NULL)
		{
		return JString();
		}
	return JString(name);
}

/******************************************************************************
 SetSMTPUser

 ******************************************************************************/

void
GPrefsMgr::SetSMTPUser
	(
	const JString& user
	)
{
	std::ostringstream data;
	data << user;

	SetData(kSMTPUserID, data);
}

/******************************************************************************
 ShowingStateInTitle

 ******************************************************************************/

JBoolean
GPrefsMgr::ShowingStateInTitle()
{
	std::string data;
	const JBoolean ok = GetData(kGShowStateInTitleID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean show;
	dataStream >> show;
	return show;
}

/******************************************************************************
 ShowStateInTitle

 ******************************************************************************/

void
GPrefsMgr::ShowStateInTitle
	(
	const JBoolean show
	)
{
	std::ostringstream data;
	data << show;

	SetData(kGShowStateInTitleID, data);
	Broadcast(ShowStateChanged());
}

/******************************************************************************
 GetPopServer

 ******************************************************************************/

JString
GPrefsMgr::GetPopServer()
{
	std::string data;
	const JBoolean ok = GetData(kGPOPServerID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString pop;
	dataStream >> pop;
	return pop;
}

/******************************************************************************
 SetPopServer

 ******************************************************************************/

void
GPrefsMgr::SetPopServer
	(
	const JString& pop
	)
{
	std::ostringstream data;
	data << pop;

	SetData(kGPOPServerID, data);
}

/******************************************************************************
 UsingPopServer

 ******************************************************************************/

JBoolean
GPrefsMgr::UsingPopServer()
{
	std::string data;
	const JBoolean ok = GetData(kGUsePopID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean use;
	dataStream >> use;
	return use;
}

/******************************************************************************
 UsePopServer

 ******************************************************************************/

void
GPrefsMgr::UsePopServer
	(
	const JBoolean use
	)
{
	std::ostringstream data;
	data << use;

	SetData(kGUsePopID, data);
	Broadcast(PopChanged());
}

/******************************************************************************
 LeaveOnServer

 ******************************************************************************/

JBoolean
GPrefsMgr::LeaveOnServer()
{
	std::string data;
	const JBoolean ok = GetData(kGPopLeaveOnServerID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean leave;
	dataStream >> leave;
	return leave;
}

/******************************************************************************
 SetLeaveOnServer

 ******************************************************************************/

void
GPrefsMgr::SetLeaveOnServer
	(
	const JBoolean leave
	)
{
	std::ostringstream data;
	data << leave;

	SetData(kGPopLeaveOnServerID, data);
}

/******************************************************************************
 GetPopUserName

 ******************************************************************************/

JString
GPrefsMgr::GetPopUserName()
{
	std::string data;
	const JBoolean ok = GetData(kGPopUserNameID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString name;
	dataStream >> name;
	return name;
}

/******************************************************************************
 SetPopUserName

 ******************************************************************************/

void
GPrefsMgr::SetPopUserName
	(
	const JString& name
	)
{
	std::ostringstream data;
	data << name;

	SetData(kGPopUserNameID, data);
}

/******************************************************************************
 GetPopPasswd

 ******************************************************************************/

JString
GPrefsMgr::GetPopPasswd()
{
	std::string data;
	const JBoolean ok = GetData(kGPopPasswdID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString passwd;
	dataStream >> passwd;
	return passwd;
}

/******************************************************************************
 SetPopPasswd

 ******************************************************************************/

void
GPrefsMgr::SetPopPasswd
	(
	const JString& passwd
	)
{
	std::ostringstream data;
	data << passwd;

	SetData(kGPopPasswdID, data);
}

/******************************************************************************
 RememberPasswd

 ******************************************************************************/

JBoolean
GPrefsMgr::RememberPasswd()
{
	std::string data;
	const JBoolean ok = GetData(kGPopRememberPasswdID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean remember;
	dataStream >> remember;
	return remember;
}

/******************************************************************************
 SetRememberPasswd

 ******************************************************************************/

void
GPrefsMgr::SetRememberPasswd
	(
	const JBoolean remember
	)
{
	std::ostringstream data;
	data << remember;

	SetData(kGPopRememberPasswdID, data);
}

/******************************************************************************
 UsingAPop

 ******************************************************************************/

JBoolean
GPrefsMgr::UsingAPop()
{
	std::string data;
	const JBoolean ok = GetData(kGUseAPopID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean use;
	dataStream >> use;
	return use;
}

/******************************************************************************
 UseAPop

 ******************************************************************************/

void
GPrefsMgr::UseAPop
	(
	const JBoolean use
	)
{
	std::ostringstream data;
	data << use;

	SetData(kGUseAPopID, data);
	Broadcast(PopChanged());
}

/******************************************************************************
 AutoCheckingMail

 ******************************************************************************/

JBoolean
GPrefsMgr::AutoCheckingMail()
{
	std::string data;
	const JBoolean ok = GetData(kAutoCheckMailID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean check;
	dataStream >> check;
	return check;
}

/******************************************************************************
 AutoCheckMail

 ******************************************************************************/

void
GPrefsMgr::AutoCheckMail
	(
	const JBoolean check
	)
{
	std::ostringstream data;
	data << check;

	SetData(kAutoCheckMailID, data);
	Broadcast(PopChanged());
}

/******************************************************************************
 CheckMailDelay

 ******************************************************************************/

JInteger
GPrefsMgr::CheckMailDelay()
{
	std::string data;
	const JBoolean ok = GetData(kCheckMailDelayID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JInteger delay;
	dataStream >> delay;
	return delay;
}

/******************************************************************************
 SetCheckMailDelay

 ******************************************************************************/

void
GPrefsMgr::SetCheckMailDelay
	(
	const JInteger delay
	)
{
	std::ostringstream data;
	data << delay;

	SetData(kCheckMailDelayID, data);
	Broadcast(PopChanged());
}

/******************************************************************************
 GetReplyTo

 ******************************************************************************/

JString
GPrefsMgr::GetReplyTo()
{
	JString replyto;
	if (IDValid(kGReplyToID))
		{
		std::string data;
		const JBoolean ok = GetData(kGReplyToID, &data);
		assert( ok );

		std::istringstream dataStream(data);
		dataStream >> replyto;
		}
	return replyto;
}

/******************************************************************************
 SetReplyTo

 ******************************************************************************/

void
GPrefsMgr::SetReplyTo
	(
	const JString& replyto
	)
{
	std::ostringstream data;
	data << replyto;

	SetData(kGReplyToID, data);
}

/******************************************************************************
 HasUIDList

 ******************************************************************************/

JBoolean
GPrefsMgr::HasUIDList()
	const
{
	return IDValid(kGUIDListID);
}


/******************************************************************************
 GetUIDList

 ******************************************************************************/

void
GPrefsMgr::GetUIDList
	(
	JPtrArray<JString>& list
	)
{
	if (IDValid(kGUIDListID))
		{
		std::string data;
		const JBoolean ok = GetData(kGUIDListID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JSize count;
		dataStream >> count;

		list.SetCompareFunction(JCompareStringsCaseSensitive);

		for (JIndex i=1; i<=count; i++)
			{
			JString* str = new JString;
			assert( str != NULL );
			dataStream >> *str;
			if (!list.InsertSorted(str, kJFalse))
				{
				delete str;
				}
			}
		}
}

/******************************************************************************
 SetUIDList

 ******************************************************************************/

void
GPrefsMgr::SetUIDList
	(
	const JPtrArray<JString>& list
	)
{
	const JSize count = list.GetElementCount();

	std::ostringstream data;
	data << count;

	for (JIndex i=1; i<=count; i++)
		{
		data << ' ' << *(list.GetElement(i));
		}

	SetData(kGUIDListID, data);
}

void
GPrefsMgr::ClearUIDList()
{
	const JSize count = 0;

	std::ostringstream data;
	data << count;

	SetData(kGUIDListID, data);
}

/******************************************************************************
 AutoInsertingQuote

 ******************************************************************************/

JBoolean
GPrefsMgr::AutoInsertingQuote()
{
	std::string data;
	const JBoolean ok = GetData(kAutoInsertQuoteID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean insert;
	dataStream >> insert;
	return insert;
}

/******************************************************************************
 AutoInsertQuote

 ******************************************************************************/

void
GPrefsMgr::AutoInsertQuote
	(
	const JBoolean insert
	)
{
	std::ostringstream data;
	data << insert;

	SetData(kAutoInsertQuoteID, data);
}

/******************************************************************************
 GetCurrentOutbox

 ******************************************************************************/

JString
GPrefsMgr::GetCurrentOutbox()
{
	JString homeDir;
	JBoolean fileok = JGetHomeDirectory(&homeDir);
	JAppendDirSeparator(&homeDir);
	JString outbox = homeDir + ".outbox";
	return outbox;

	std::string data;
	const JBoolean ok = GetData(kGCurrentOutboxID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString mbox;
	dataStream >> mbox;
	return mbox;
}

/******************************************************************************
 SetCurrentOutbox

 ******************************************************************************/

void
GPrefsMgr::SetCurrentOutbox
	(
	const JString& mbox
	)
{
	std::ostringstream data;
	data << mbox;

	SetData(kGCurrentOutboxID, data);
}

/******************************************************************************
 GetDefaultInbox

 ******************************************************************************/

JString
GPrefsMgr::GetDefaultInbox()
{
	std::string data;
	const JBoolean ok = GetData(kGDefaultInboxID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString mbox;
	dataStream >> mbox;
	return mbox;
}

/******************************************************************************
 SetDefaultInbox

 ******************************************************************************/

void
GPrefsMgr::SetDefaultInbox
	(
	const JString& mbox
	)
{
	std::ostringstream data;
	data << mbox;

	SetData(kGDefaultInboxID, data);
}

/******************************************************************************
 GetDefaultMonoFont

 ******************************************************************************/

JFontID
GPrefsMgr::GetDefaultMonoFontID()
{
	if (IDValid(kGDefaultMonoFontID))
		{
		std::string data;
		JBoolean ok = GetData(kGDefaultMonoFontID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JString name;
		dataStream >> name;
		JFontID id;
		if (JXGetApplication()->GetCurrentDisplay()->GetXFontManager()->GetFontID(name, &id))
			{
			return id;
			}
		}

	SetDefaultMonoFont(JGetMonospaceFontName());
	return GetDefaultMonoFontID();
}

JString
GPrefsMgr::GetDefaultMonoFont()
{
	if (IDValid(kGDefaultMonoFontID))
		{
		std::string data;
		JBoolean ok = GetData(kGDefaultMonoFontID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JString name;
		dataStream >> name;
		return name;
		}

	SetDefaultMonoFont(JGetMonospaceFontName());
	return GetDefaultMonoFont();
}

/******************************************************************************
 SetDefaultMonoFont

 ******************************************************************************/

void
GPrefsMgr::SetDefaultMonoFont
	(
	const JCharacter* fontName
	)
{
	std::ostringstream data;
	data << JString(fontName);

	SetData(kGDefaultMonoFontID, data);
	Broadcast(FontChanged());
}

/******************************************************************************
 GetDefaultFontSize

 ******************************************************************************/

JSize
GPrefsMgr::GetDefaultFontSize()
{
	if (IDValid(kGDefaultFontSizeID))
		{
		std::string data;
		JBoolean ok = GetData(kGDefaultFontSizeID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JSize size;
		dataStream >> size;
		return size;
		}
	return kGDefaultFontSize;
}

/******************************************************************************
 SetDefaultFontSize

 ******************************************************************************/

void
GPrefsMgr::SetDefaultFontSize
	(
	const JSize size
	)
{
	std::ostringstream data;
	data << size;

	SetData(kGDefaultFontSizeID, data);
}

/******************************************************************************
 GetTabCharWidth

 ******************************************************************************/

JSize
GPrefsMgr::GetTabCharWidth()
{
	if (IDValid(kGTabCharWidthID))
		{
		std::string data;
		JBoolean ok = GetData(kGTabCharWidthID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JSize size;
		dataStream >> size;
		return size;
		}
	return kGDefaultTabCharWidth;
}

/******************************************************************************
 SetTabCharWidth

 ******************************************************************************/

void
GPrefsMgr::SetTabCharWidth
	(
	const JSize size
	)
{
	std::ostringstream data;
	data << size;

	SetData(kGTabCharWidthID, data);
	Broadcast(FontChanged());
}

/******************************************************************************
 ShowingSenderString

 ******************************************************************************/

JBoolean
GPrefsMgr::ShowingSenderString()
{
	std::string data;
	const JBoolean ok = GetData(kGShowSenderStringID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JBoolean show;
	dataStream >> show;
	return show;
}

/******************************************************************************
 ShowSenderString

 ******************************************************************************/

void
GPrefsMgr::ShowSenderString
	(
	const JBoolean show
	)
{
	std::ostringstream data;
	data << show;

	SetData(kGShowSenderStringID, data);
}

/******************************************************************************
 GetSenderString

 ******************************************************************************/

JString
GPrefsMgr::GetSenderString()
{
	std::string data;
	const JBoolean ok = GetData(kGSenderStringID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString sender;
	dataStream >> sender;
	return sender;
}

/******************************************************************************
 SetSenderString

 ******************************************************************************/

void
GPrefsMgr::SetSenderString
	(
	const JString& senderString
	)
{
	std::ostringstream data;
	data << senderString;

	SetData(kGSenderStringID, data);
}

/******************************************************************************
 UsingCustomFromLine

 ******************************************************************************/

JBoolean
GPrefsMgr::UsingCustomFromLine()
{
	if (IDValid(kGUseCustomFromLineID))
		{
		std::string data;
		const JBoolean ok = GetData(kGUseCustomFromLineID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JBoolean use;
		dataStream >> use;
		return use;
		}
	return kJFalse;
}

/******************************************************************************
 UseCustomFromLine

 ******************************************************************************/

void
GPrefsMgr::UseCustomFromLine
	(
	const JBoolean useFrom
	)
{
	std::ostringstream data;
	data << useFrom;

	SetData(kGUseCustomFromLineID, data);
}

/******************************************************************************
 GetCustomFromString

 ******************************************************************************/

JString
GPrefsMgr::GetCustomFromString()
{
	if (IDValid(kGCustomFromLineID))
		{
		std::string data;
		const JBoolean ok = GetData(kGCustomFromLineID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JString from;
		dataStream >> from;
		return from;
		}
	return JString();
}

/******************************************************************************
 SetCustomFromString

 ******************************************************************************/

void
GPrefsMgr::SetCustomFromString
	(
	const JString& fromLine
	)
{
	std::ostringstream data;
	data << fromLine;

	SetData(kGCustomFromLineID, data);
}

/******************************************************************************
 MaxQuote

 ******************************************************************************/

JSize
GPrefsMgr::GetMaxQuote()
{
	if (IDValid(kGMaxQuoteID))
		{
		std::string data;
		const JBoolean ok = GetData(kGMaxQuoteID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JSize max;
		dataStream >> max;
		return max * 1024;
		}
	return kGDefaultMaxQuote * 1024;
}

void
GPrefsMgr::SetMaxQuote
	(
	const JSize max
	)
{
	std::ostringstream data;
	data << max;

	SetData(kGMaxQuoteID, data);
}

/******************************************************************************
 FilterFile

 ******************************************************************************/

JString
GPrefsMgr::GetFilterFile()
{
	std::string data;
	const JBoolean ok = GetData(kGFilterFileID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString name;
	dataStream >> name;
	return name;
}

void
GPrefsMgr::SetFilterFile
	(
	const JString& name
	)
{
	std::ostringstream data;
	data << name;

	SetData(kGFilterFileID, data);
}

/******************************************************************************
 EncryptionType

 ******************************************************************************/

GPrefsMgr::EncryptionType
GPrefsMgr::GetEncryptionType()
	const
{
	if (IDValid(kGEncryptionTypeID))
		{
		std::string data;
		JBoolean ok = GetData(kGEncryptionTypeID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		int type;
		dataStream >> type;
		return (EncryptionType)type;
		}
	return kPGP2_6;
}

void
GPrefsMgr::SetEncryptionType
	(
	const EncryptionType type
	)
{
	std::ostringstream data;
	data << (int)type;

	SetData(kGEncryptionTypeID, data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
GPrefsMgr::EditPrefs()
{
	assert(itsDialog == NULL);
	itsDialog =
		new GMailPrefsDialog(JXGetApplication(), ShowingStateInTitle(),
								AutoInsertingQuote(), ShowingSenderString(),
								GetSenderString(), GetDefaultMonoFont(),
								GetDefaultFontSize(), GetMaxQuote()/1024,
								(JXGetSpellChecker())->WillReportNoErrors(),
								GetTabCharWidth(),
								IsBeepingOnNewMail(), (JIndex)GetEncryptionType(),
								GetFilterFile(), GetDefaultInbox(), 
								IsOpeningMailboxWindow());
	assert(itsDialog != NULL);
	ListenTo(itsDialog);
	itsDialog->BeginDialog();
}

/******************************************************************************
 ReadChooseSaveDialogPrefs

 ******************************************************************************/

void
GPrefsMgr::ReadChooseSaveDialogPrefs()
{
	if (IDValid(kChooseSaveDialogPrefsID))
		{
		std::string data;
		JBoolean ok = GetData(kChooseSaveDialogPrefsID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JXGetChooseSaveFile()->ReadSetup(dataStream);
		}
}

/******************************************************************************
 WriteChooseSaveDialogPrefs

 ******************************************************************************/

void
GPrefsMgr::WriteChooseSaveDialogPrefs()
{
	std::ostringstream data;
	JXGetChooseSaveFile()->WriteSetup(data);
	SetData(kChooseSaveDialogPrefsID, data);
}

/******************************************************************************
 CompareFileNames (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
GPrefsMgr::CompareFileNames
	(
	JString* const & n1,
	JString* const & n2
	)
{
	JString s1(*n1);
	JString s2(*n2);
	JIndex index;
	if (s1.LocateLastSubstring("/",&index))
		{
		s1.RemoveSubstring(1, index);
		}
	if (s2.LocateLastSubstring("/",&index))
		{
		s2.RemoveSubstring(1, index);
		}
	const int r = JStringCompare(s1, s2, kJFalse);

	if (r > 0)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (r < 0)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 PrinterSetup

 ******************************************************************************/

void
GPrefsMgr::WritePrinterSetup
	(
	JXPTPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteXPTSetup(data);
	SetData(kPTPrinterSetupID, data);
}

void
GPrefsMgr::ReadPrinterSetup
	(
	JXPTPrinter* printer
	)
{
	if (IDValid(kPTPrinterSetupID))
		{
		std::string data;
		GetData(kPTPrinterSetupID, &data);
		std::istringstream dataStream(data);
		printer->ReadXPTSetup(dataStream);
		}
}

/******************************************************************************
 TableWindowSize

 ******************************************************************************/

void
GPrefsMgr::GetTableWindowSize
	(
	JXWindow* window
	)
	const
{
	GetWindowSize(kGTableWindowSizeID, window);
}

void
GPrefsMgr::SaveTableWindowSize
	(
	JXWindow* window
	)
{
	SaveWindowSize(kGTableWindowSizeID, window);
}

/******************************************************************************
 ViewWindowSize

 ******************************************************************************/

void
GPrefsMgr::GetViewWindowSize
	(
	JXWindow* window
	)
	const
{
	GetWindowSize(kGViewWindowSizeID, window);
}

void
GPrefsMgr::SaveViewWindowSize
	(
	JXWindow* window
	)
{
	SaveWindowSize(kGViewWindowSizeID, window);
}

/******************************************************************************
 EditorWindowSize

 ******************************************************************************/

void
GPrefsMgr::GetEditorWindowSize
	(
	JXWindow* window
	)
	const
{
	GetWindowSize(kGEditorWindowSizeID, window);
}

void
GPrefsMgr::SaveEditorWindowSize
	(
	JXWindow* window
	)
{
	SaveWindowSize(kGEditorWindowSizeID, window);
}

/******************************************************************************
 WindowSize (private)

 ******************************************************************************/

void
GPrefsMgr::GetWindowSize
	(
	const JPrefID	id,
	JXWindow*		window
	)
	const
{
	if (IDValid(id))
		{
		std::string data;
		GetData(id, &data);
		std::istringstream dataStream(data);
		window->ReadGeometry(dataStream);
		}
}

void
GPrefsMgr::SaveWindowSize
	(
	const JPrefID	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	window->WriteGeometry(data);
//	JPoint dtl = window->GetDesktopLocation();
//	window->Place(dtl.x, dtl.y);

	SetData(id, data);
}

/******************************************************************************
 TableWindowPrefs (public)

 ******************************************************************************/

void
GPrefsMgr::SetTableWindowPrefs
	(
	GMessageTableDir* dir
	)
{
	std::ostringstream data;
	dir->WriteWindowPrefs(data);
	SetData(kGTableWindowPrefsID, data);
}

void
GPrefsMgr::GetTableWindowPrefs
	(
	GMessageTableDir* dir
	)
{
	if (IDValid(kGTableWindowPrefsID))
		{
		std::string data;
		GetData(kGTableWindowPrefsID, &data);
		std::istringstream dataStream(data);
		dir->ReadWindowPrefs(dataStream);
		}
}

/******************************************************************************
 FindDirWindowPrefs (public)

 ******************************************************************************/

void
GPrefsMgr::SetFindDirWindowPrefs
	(
	GMFindTableDir* dir
	)
{
	std::ostringstream data;
	dir->WriteWindowPrefs(data);
	SetData(kGFindDirWindowPrefsID, data);
}

void
GPrefsMgr::GetFindDirWindowPrefs
	(
	GMFindTableDir* dir
	)
{
	if (IDValid(kGFindDirWindowPrefsID))
		{
		std::string data;
		GetData(kGFindDirWindowPrefsID, &data);
		std::istringstream dataStream(data);
		dir->ReadWindowPrefs(dataStream);
		}
}

/******************************************************************************
 ViewWindowPrefs (public)

 ******************************************************************************/

void
GPrefsMgr::SetViewWindowPrefs
	(
	GMessageViewDir* dir
	)
{
	std::ostringstream data;
	dir->WriteWindowPrefs(data);
	SetData(kGViewWindowPrefsID, data);
}

void
GPrefsMgr::GetViewWindowPrefs
	(
	GMessageViewDir* dir
	)
{
	if (IDValid(kGViewWindowPrefsID))
		{
		std::string data;
		GetData(kGViewWindowPrefsID, &data);
		std::istringstream dataStream(data);
		dir->ReadWindowPrefs(dataStream);
		}
}

/******************************************************************************
 EditorWindowPrefs (public)

 ******************************************************************************/

void
GPrefsMgr::SetEditorWindowPrefs
	(
	GMessageEditDir* dir
	)
{
	std::ostringstream data;
	dir->WriteWindowPrefs(data);
	SetData(kGEditorWindowPrefsID, data);
}

void
GPrefsMgr::GetEditorWindowPrefs
	(
	GMessageEditDir* dir
	)
{
	if (IDValid(kGEditorWindowPrefsID))
		{
		std::string data;
		GetData(kGEditorWindowPrefsID, &data);
		std::istringstream dataStream(data);
		dir->ReadWindowPrefs(dataStream);
		}
}

/******************************************************************************
 IsBeepingOnNewMail

 ******************************************************************************/

JBoolean
GPrefsMgr::IsBeepingOnNewMail()
{
	if (IDValid(kGBeepOnNewMailID))
		{
		std::string data;
		const JBoolean ok = GetData(kGBeepOnNewMailID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JBoolean beep;
		dataStream >> beep;
		return beep;
		}
	return kJTrue;
}

/******************************************************************************
 BeepOnNewMail

 ******************************************************************************/

void
GPrefsMgr::BeepOnNewMail
	(
	const JBoolean beep
	)
{
	std::ostringstream data;
	data << beep;

	SetData(kGBeepOnNewMailID, data);
}

/******************************************************************************
 IsOpeningMailboxWindow

 ******************************************************************************/

JBoolean
GPrefsMgr::IsOpeningMailboxWindow()
	const
{
	if (IDValid(kGOpenMailboxWindowPrefsID))
		{
		std::string data;
		const JBoolean ok = GetData(kGOpenMailboxWindowPrefsID, &data);
		assert( ok );

		std::istringstream dataStream(data);

		JBoolean open;
		dataStream >> open;
		return open;
		}
	return kJTrue;
}

/******************************************************************************
 ShouldBeOpeningMailboxWindow

 ******************************************************************************/

void
GPrefsMgr::ShouldBeOpeningMailboxWindow
	(
	const JBoolean open
	)
{
	std::ostringstream data;
	data << open;

	SetData(kGOpenMailboxWindowPrefsID, data);
}
