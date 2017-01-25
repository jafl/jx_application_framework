/******************************************************************************
 SMTPMessage.cc

	<Description>

	BASE CLASS = <NONE>

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <SMTPMessage.h>
#include <GNetDebugDir.h>
#include "gMailUtils.h"
#include "GPrefsMgr.h"
#include "GAddressBookMgr.h"
#include "GMAttachmentTable.h"
#include "GMAccountManager.h"
#include "GMAccount.h"

#include <JXTimerTask.h>

#include <JMessageProtocol.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>
#include <JTextEditor.h>

#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jTime.h>
#include <jErrno.h>

#include <GMGlobals.h>

#include <j_prep_ace.h>
#include <ace/Connector.h>
#include <ace/INET_Addr.h>
#include <ace/UNIX_Addr.h>

#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <ace/LSOCK_Connector.h>
#include <ace/LSOCK_Stream.h>

#include <jErrno.h>

#include <sys/timeb.h>
#include <sys/utsname.h>
#include <jFStreamUtil.h>
#include <unistd.h>
#include <time.h>
#include <strstream>
#include <jAssert.h>

#undef new
#undef delete

typedef JMessageProtocol<ACE_SOCK_STREAM>	InetLink;
typedef ACE_Connector<InetLink, ACE_SOCK_CONNECTOR>	INETConnector;

enum
{
	kStartUp = 1,
	kHello,
	kFrom,
	kTo,
	kCc,
	kBcc,
	kDataHeader,
	kData
};

const JCharacter* SMTPMessage::kMessageSent = "SMTPMessage::kMessageSent";
const JCharacter* SMTPMessage::kSendFailure = "SMTPMessage::kSendFailure";

static const JInteger	kOKValue		= 250;
static const JSize		kMaxWaitSeconds	= 25;

static const JRegex smtpPeriodStuff("^(\\.)+");

static const char* gcl_months[] =
	{"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"};
	
/******************************************************************************
 Constructor

 *****************************************************************************/

SMTPMessage::SMTPMessage()
	:
	itsDeleteTask(NULL),
	itsTimeoutTask(NULL)
{
	itsTo = new JString();
	assert(itsTo != NULL);

	itsFrom = new JString();
	assert(itsFrom != NULL);

	itsSubject = new JString();
	assert(itsSubject != NULL);

	itsReplyTo = new JString();
	assert(itsReplyTo != NULL);

	itsCC = new JString();
	assert(itsCC != NULL);

	itsBCC = new JString();
	assert(itsBCC != NULL);

	itsData = new JString();
	assert(itsData != NULL);

	itsAddedHeaders = new JString();
	assert(itsAddedHeaders != NULL);

	itsToNames = new JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsToNames != NULL);
	itsToNames->SetCompareFunction(JCompareStringsCaseInsensitive);
	itsCcNames = new JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsCcNames != NULL);
	itsCcNames->SetCompareFunction(JCompareStringsCaseInsensitive);
	itsBccNames = new JPtrArray<JString>(JPtrArrayT::kForgetAll);
	assert(itsBccNames != NULL);
	itsBccNames->SetCompareFunction(JCompareStringsCaseInsensitive);

	itsLink		= NULL;
	itsConnector	= NULL;

	itsCurrentMode = kStartUp;
	itsCurrentIndex = 1;

	itsIsFinished		= kJFalse;
	itsSomethingRead	= kJFalse;
	itsIsTryingToQuit	= kJFalse;

	SetAccount(GGetAccountMgr()->GetDefaultAccount());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

SMTPMessage::~SMTPMessage()
{
	delete itsTo;
	delete itsFrom;
	delete itsSubject;
	delete itsReplyTo;
	delete itsCC;
	delete itsBCC;
	delete itsData;
	delete itsAddedHeaders;
	delete itsLink;
	delete itsConnector;
	itsToNames->DeleteAll();
	delete itsToNames;
	itsCcNames->DeleteAll();
	delete itsCcNames;
	itsBccNames->DeleteAll();
	delete itsBccNames;
}

/******************************************************************************
 SetTo

 *****************************************************************************/

void
SMTPMessage::SetTo
	(
	const JString& to
	)
{
	*itsTo = to;
}

/******************************************************************************
 SetFrom

 *****************************************************************************/

void
SMTPMessage::SetFrom
	(
	const JString& from
	)
{
	*itsFrom = from;
	if (!itsFrom->BeginsWith("<"))
		{
		itsFrom->Prepend("<");
		itsFrom->Append(">");
		}
}

/******************************************************************************
 SetSubject

 *****************************************************************************/

void
SMTPMessage::SetSubject
	(
	const JString& subject
	)
{
	*itsSubject = subject;
}

/******************************************************************************
 SetReplyTo

 *****************************************************************************/

void
SMTPMessage::SetReplyTo
	(
	const JString& replyto
	)
{
	*itsReplyTo = replyto;
}

/******************************************************************************
 SetCC

 *****************************************************************************/

void
SMTPMessage::SetCC
	(
	const JString& cc
	)
{
	*itsCC = cc;
}

/******************************************************************************
 SetBCC

 *****************************************************************************/

void
SMTPMessage::SetBCC
	(
	const JString& bcc
	)
{
	*itsBCC = bcc;
}

/******************************************************************************
 SetData

 *****************************************************************************/

void
SMTPMessage::SetData
	(
	const JString& data
	)
{
	*itsData = data;
}

/******************************************************************************
 SetData (public)

 *****************************************************************************/

void
SMTPMessage::SetData
	(
	GMAttachmentTable*	table,
	JTextEditor*		editor
	)
{
	JString header;
	GMGetMIMESource(table, editor, itsData, &header);
	*itsAddedHeaders += header;
}


/******************************************************************************
 AddHeaders

 *****************************************************************************/

void
SMTPMessage::AddHeaders
	(
	const JString& headers
	)
{
	*itsAddedHeaders = headers;
}

/******************************************************************************
 SetAccount (public)

 ******************************************************************************/

void
SMTPMessage::SetAccount
	(
	GMAccount* account
	)
{
	itsSMTPServer	= account->GetSMTPServer();
	itsFullName		= account->GetFullName();
	itsSMTPUser		= account->GetSMTPUser();
	JString dir		= account->GetMailDir();
	dir				= JCombinePathAndName(dir, "Sent");
	time_t now;
	time(&now);

	struct tm* local_time	= localtime(&now);
	JString outbox			= gcl_months[local_time->tm_mon];
	outbox					+= "_" + JString(local_time->tm_year + 1900, JString::kBase10);
	itsOutbox				= JCombinePathAndName(dir, outbox);
}

/******************************************************************************
 HandleAddresses

 *****************************************************************************/

void
SMTPMessage::HandleAddresses()
{
	JPtrArray<JString> names(JPtrArrayT::kForgetAll);
	GParseNameList(*itsTo, names);
	ReplaceAliases(names);
	while (!names.IsEmpty())
		{
		JString* str = names.GetFirstElement();
		if (!str->BeginsWith("<"))
			{
			str->Prepend("<");
			str->Append(">");
			}
		if (itsToNames->InsertSorted(str,kJFalse))
			{
			names.RemoveElement(1);
			}
		else
			{
			names.DeleteElement(1);
			}
		}

	GParseNameList(*itsCC, names);
	ReplaceAliases(names);
	while (!names.IsEmpty())
		{
		JString* str = names.GetFirstElement();
		if (!str->BeginsWith("<"))
			{
			str->Prepend("<");
			str->Append(">");
			}
		JIndex findex;
		if (itsToNames->SearchSorted(str, JOrderedSetT::kFirstMatch, &findex))
			{
			names.DeleteElement(1);
			}
		else
			{
			if (itsCcNames->InsertSorted(str,kJFalse))
				{
				names.RemoveElement(1);
				}
			else
				{
				names.DeleteElement(1);
				}
			}
		}

	GParseNameList(*itsBCC, names);
	ReplaceAliases(names);
	while (!names.IsEmpty())
		{
		JString* str = names.GetFirstElement();
		if (!str->BeginsWith("<"))
			{
			str->Prepend("<");
			str->Append(">");
			}
		JIndex findex;
		if (itsToNames->SearchSorted(str, JOrderedSetT::kFirstMatch, &findex))
			{
			names.DeleteElement(1);
			}
		else
			{
			if (itsCcNames->SearchSorted(str, JOrderedSetT::kFirstMatch, &findex))
				{
				names.DeleteElement(1);
				}
			else
				{
				if (itsBccNames->InsertSorted(str,kJFalse))
					{
					names.RemoveElement(1);
					}
				else
					{
					names.DeleteElement(1);
					}
				}
			}
		}

}

/******************************************************************************
 ReplaceAliases

 *****************************************************************************/

void
SMTPMessage::ReplaceAliases
	(
	JPtrArray<JString>& names
	)
{
	JPtrArray<JString> aliases(JPtrArrayT::kForgetAll);
	aliases.SetCompareFunction(JCompareStringsCaseSensitive);
	JSize i = 1;
	while (i <= names.GetElementCount())
		{
		JString& name = *(names.GetElement(i));
		JString alias;
		JString fcc;
		if (GGetAddressBookMgr()->NameIsAlias(name, alias, fcc))
			{
			JIndex findex;
			if (!aliases.SearchSorted(&name, JOrderedSetT::kAnyMatch, &findex))
				{
				GParseNameList(alias, names);
				aliases.InsertSorted(names.GetElement(i));
				names.RemoveElement(i);
				}
			else
				{
				names.DeleteElement(i);
				}
//			GParseNameList(fcc, names);
			}
		else
			{
			i++;
			}
		}
	aliases.DeleteAll();
}


/******************************************************************************
 Send

 *****************************************************************************/

void
SMTPMessage::Send()
{
	HandleAddresses();

	AppendToOutbox("From ");
	AppendToOutbox(*itsFrom);
	AppendToOutbox(" ");

	struct timeb tp;
	ftime(&tp);
	JString date(ctime(&(tp.time)));
	AppendToOutbox(date);

	std::istrstream is(date.GetCString(), date.GetLength());
	JString dow;
	JString mon;
	JString day;
	JString rest;
	dow = JReadUntilws(is);
	mon = JReadUntilws(is);
	day = JReadUntilws(is);
	JReadAll(is, &rest);

	AppendToOutbox("Date: ");
	AppendToOutbox(dow);
	AppendToOutbox(", ");
	AppendToOutbox(day);
	AppendToOutbox(" ");
	AppendToOutbox(mon);
	AppendToOutbox(" ");
	AppendToOutbox(rest);

	JString addrStr = itsSMTPServer;
	if (!addrStr.Contains(":"))
		{
		addrStr += ":25";
		}
	ACE_INET_Addr addr(addrStr);

	itsLink = new InetLink;
	assert(itsLink != NULL);

	itsConnector = new INETConnector;
	assert( itsConnector != NULL );

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, ACE_Time_Value(kMaxWaitSeconds));

	if (itsConnector->connect(itsLink, addr, options) == -1 &&
		jerrno() != EAGAIN)
		{
		JString notice	= "No response from sendmail : Error number ";
		int errNumber	= jerrno();
		notice += JString(errNumber);
		JGetUserNotification()->ReportError(notice);
		itsDeleteTask = new JXTimerTask(1000,kJTrue);
		assert( itsDeleteTask != NULL );
		itsDeleteTask->Start();
		ListenTo(itsDeleteTask);
		Broadcast(SendFailure());
//		itsLink = NULL;
		return;
		}
	itsTimeoutTask = new JXTimerTask(kMaxWaitSeconds * 1000,kJTrue);
	assert( itsTimeoutTask != NULL );
	itsTimeoutTask->Start();
	ListenTo(itsTimeoutTask);
	ClearWhenGoingAway(itsLink, &itsLink);
}

/******************************************************************************
 Receive

 *****************************************************************************/

void
SMTPMessage::Receive
	(
	JBroadcaster*					sender,
	const JBroadcaster::Message&	message
	)
{
	if (sender == itsLink && !itsIsFinished)
		{
		if (message.Is(JMessageProtocolT::kMessageReady))
			{
			itsSomethingRead = kJTrue;
			if (itsTimeoutTask != NULL)
				{
				delete itsTimeoutTask;
				itsTimeoutTask	= NULL;
				}
			ReadReturnValue();
			}
		else if (message.Is(JMessageProtocolT::kReceivedDisconnect))
			{
			JGetUserNotification()->ReportError("The connection to sendmail closed early.");
			Broadcast(SendFailure());
			itsDeleteTask = new JXTimerTask(1000,kJTrue);
			assert( itsDeleteTask != NULL );
			itsDeleteTask->Start();
			ListenTo(itsDeleteTask);
//			itsLink = NULL;
			}
		}
	else if (sender == itsTimeoutTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		JGetUserNotification()->ReportError("The connection to the SMTP server timed out.");
		Broadcast(SendFailure());
		itsLink		= NULL;
		itsTimeoutTask	= NULL;
		delete this;
		}
	else if (sender == itsDeleteTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		itsDeleteTask	= NULL;
		delete this;
		}
}

/******************************************************************************
 ReadReturnValue

 *****************************************************************************/

void
SMTPMessage::ReadReturnValue()
{
	JString line;
	JBoolean ok = itsLink->GetNextMessage(&line);
	assert(ok);

	if (itsCurrentMode != kDataHeader)
		{
		GMGetSMTPDebugDir()->AddText(line);
		}

	if ((itsCurrentMode != kStartUp) && (itsCurrentMode != kDataHeader))
		{
		JInteger value;
		JIndex findex;
		ok = line.LocateSubstring(" ", &findex);
		if (ok && (findex > 1))
			{
			JString number = line.GetSubstring(1, findex - 1);
			if (number.IsInteger())
				{
				number.ConvertToInteger(&value);
				}
			else
				{
				ok	= kJFalse;
				}
			}
		else
			{
			ok	= kJFalse;
			}

		if (!ok)
			{
			const JIndex kDashIndex	= 4;
			if (line.GetCharacter(kDashIndex) == '-')
				{
				// this is a multiline response.
				return;
				}
			}

		if (!ok || (value != kOKValue))
			{
			if (!itsIsTryingToQuit)
				{
				const JCharacter* map[] =
					{
					"err", line
					};
				const JString msg = JGetString("SMTPERROR", map, sizeof(map));

				JGetUserNotification()->ReportError(msg);
				}
			itsIsFinished = kJTrue;
			Broadcast(SendFailure());
			if (!itsIsTryingToQuit)
				{
				itsDeleteTask = new JXTimerTask(1000,kJTrue);
				assert( itsDeleteTask != NULL );
				itsDeleteTask->Start();
				ListenTo(itsDeleteTask);
				}
			return;
			}
		}

	if (itsCurrentMode < kTo)
		{
		itsCurrentMode++;
		}
	else if (itsCurrentMode == kTo)
		{
		if (itsCurrentIndex < itsToNames->GetElementCount())
			{
			itsCurrentIndex++;
			}
		else if (itsCcNames->GetElementCount() != 0)
			{
			itsCurrentMode = kCc;
			itsCurrentIndex = 1;
			}
		else if (itsBccNames->GetElementCount() != 0)
			{
			itsCurrentMode = kBcc;
			itsCurrentIndex = 1;
			}
		else
			{
			itsCurrentMode = kDataHeader;
			}
		}
	else if (itsCurrentMode == kCc)
		{
		if (itsCurrentIndex < itsCcNames->GetElementCount())
			{
			itsCurrentIndex++;
			}
		else if (itsBccNames->GetElementCount() != 0)
			{
			itsCurrentMode = kBcc;
			itsCurrentIndex = 1;
			}
		else
			{
			itsCurrentMode = kDataHeader;
			}
		}
	else if (itsCurrentMode == kBcc)
		{
		if (itsCurrentIndex < itsBccNames->GetElementCount())
			{
			itsCurrentIndex++;
			}
		else
			{
			itsCurrentMode = kDataHeader;
			}
		}
	else if (itsCurrentMode == kData)
		{
		itsIsFinished = kJTrue;
		Broadcast(MessageSent());
		GMGetSMTPDebugDir()->AddText("\n-------------------------------------\n");
		if (!itsIsTryingToQuit)
			{
			itsDeleteTask = new JXTimerTask(1000,kJTrue);
			assert( itsDeleteTask != NULL );
			itsDeleteTask->Start();
			ListenTo(itsDeleteTask);
			}
		return;
		}
	else
		{
		itsCurrentMode++;
		}

	SendNextData();
}

/******************************************************************************
 SendNextData

 *****************************************************************************/

void
SMTPMessage::SendNextData()
{
	if (itsCurrentMode == kHello)
		{
		JCharacter hostname[255];
		gethostname(hostname, 255);
		JString senddata = "HELO ";
		senddata += hostname;
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kFrom)
		{
		JString senddata = "MAIL FROM: " + *itsFrom;
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kTo)
		{
		assert(itsCurrentIndex <= itsToNames->GetElementCount());
		JString senddata;
		JString name = *(itsToNames->GetElement(itsCurrentIndex));
		senddata = "RCPT TO: " + name;
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kCc)
		{
		assert(itsCurrentIndex <= itsCcNames->GetElementCount());
		JString senddata;
		JString name = *(itsCcNames->GetElement(itsCurrentIndex));
		senddata = "RCPT TO: " + name;
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kBcc)
		{
		assert(itsCurrentIndex <= itsBccNames->GetElementCount());
		JString senddata;
		JString name = *(itsBccNames->GetElement(itsCurrentIndex));
		senddata = "RCPT TO: " + name;
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kDataHeader)
		{
		JString senddata = "DATA";
		SendLine(senddata);
		senddata += "\n";
		GMGetSMTPDebugDir()->AddText(senddata);
		}
	else if (itsCurrentMode == kData)
		{
		JString from	 = itsFullName + " <" + itsSMTPUser + ">";
		if (!from.IsEmpty())
			{
			JString senddata = "From: " + from;
			SendLine(senddata);
			senddata += "\n";
			GMGetSMTPDebugDir()->AddText(senddata);
			AppendToOutbox("From: ");
			AppendToOutbox(from);
			AppendToOutbox("\n");
			}

		struct utsname buf;
		uname(&buf);

		JString senddata = "X-Mailer: Arrow ";
		senddata += GMGetVersionNumberStr();
		senddata += JString(" (X11; ") + buf.sysname + JString(" ") +  + JString(buf.release);
		senddata += JString("; ") + buf.machine + JString(")");
		SendLine(senddata);
		AppendToOutbox(senddata);
		AppendToOutbox("\n");
		GMGetSMTPDebugDir()->AddText(senddata);

		senddata = "To: ";
		assert(itsToNames->GetElementCount() > 0);
		senddata += *(itsToNames->GetElement(1));
		for (JSize i = 2; i <= itsToNames->GetElementCount(); i++)
			{
			senddata += ",\n\t" + *(itsToNames->GetElement(i));
			}
		SendLine(senddata);
		AppendToOutbox(senddata);
		AppendToOutbox("\n");
		GMGetSMTPDebugDir()->AddText(senddata);

		if (!itsReplyTo->IsEmpty())
			{
			senddata = "Reply-To: " + *itsReplyTo;
			SendLine(senddata);
			GMGetSMTPDebugDir()->AddText(senddata);
			}

		if (itsCcNames->GetElementCount() != 0)
			{
			JSize count = itsCcNames->GetElementCount();
			senddata = "Cc: ";
			senddata += *(itsCcNames->GetElement(1));
			for (JSize i = 2; i <= count; i++)
				{
				senddata += ",\n\t" + *(itsCcNames->GetElement(i));
				}
			SendLine(senddata);
			AppendToOutbox(senddata);
			AppendToOutbox("\n");
			GMGetSMTPDebugDir()->AddText(senddata);
			}

		if (!itsSubject->IsEmpty())
			{
			senddata = "Subject: " + *itsSubject;
			SendLine(senddata);
			AppendToOutbox("Subject: ");
			AppendToOutbox(*itsSubject);
			AppendToOutbox("\n");
			GMGetSMTPDebugDir()->AddText(senddata);
			}

		if (!itsAddedHeaders->IsEmpty())
			{
			SendMultiLine(itsAddedHeaders);
			}

		SendLine("");
		AppendToOutbox("\n");

		GMGetSMTPDebugDir()->AddText("\nData not shown ...\n");
		AppendToOutbox(*itsData);
		SendData();
		AppendToOutbox("\n");
		AppendToOutbox("\n");

		SendLine("");
		SendLine(".");
		SendLine("QUIT");
		WriteToOutbox();
		}
}

/******************************************************************************
 SendLine (private)

 ******************************************************************************/

void
SMTPMessage::SendLine
	(
	const JCharacter* line
	)
{
	JString sLine(line);
	SendLine(sLine);
}

void
SMTPMessage::SendLine
	(
	const JString& line
	)
{
	JString sLine(line);
	sLine += "\r";
	itsLink->SendMessage(sLine);
}

/******************************************************************************
 SendData (private)

 ******************************************************************************/

void
SMTPMessage::SendData()
{
	// The text needs to be period stuffed and had all \n converted to \r\n
	// before this point

	itsLink->SendData(*itsData);

/*	// Need to '.' stuff
	JIndexRange range(1,0);
	while (smtpPeriodStuff.MatchAfter(*itsData, range, &range))
		{
		itsData->InsertSubstring(".", range.last + 1);
		range += 1;
		}

	SendMultiLine(itsData);
*/
}

/******************************************************************************
 SendMultiLine (private)

 ******************************************************************************/

void
SMTPMessage::SendMultiLine
	(
	JString* data
	)
{
	JIndex findex = 1;
	while (data->IndexValid(findex) && data->LocateNextSubstring("\n", &findex))
		{
		data->ReplaceSubstring(findex, findex, "\r\n");
		findex += 2;
		}
	itsLink->SendData(*data);
}

/******************************************************************************
 AppendToOutbox (private)

 ******************************************************************************/

void
SMTPMessage::AppendToOutbox
	(
	const JCharacter* text
	)
{
	itsOutboxString += text;
}

/******************************************************************************
 WriteToOutbox (private)

 ******************************************************************************/

void
SMTPMessage::WriteToOutbox()
{
	//std::ofstream os(GGetPrefsMgr()->GetCurrentOutbox(), std::ios::out|std::ios::app);
	std::ofstream os(itsOutbox, std::ios::out|std::ios::app);
	if (os.good())
		{
		itsOutboxString.Print(os);
		}
	os.close();
}

/******************************************************************************
 BlockUntilOkOrFail

 *****************************************************************************/

void
SMTPMessage::BlockUntilOkOrFail()
{
	if (itsIsFinished)
		{
		return;
		}

	if (itsTimeoutTask != NULL)
		{
		JGetUserNotification()->ReportError("Connection to SMTP server timed out.");
		Broadcast(SendFailure());
//		itsLink = NULL;
		delete this;
		return;
		}

	JBoolean success	= kJFalse;
	itsIsFinished		= kJFalse;
	itsIsTryingToQuit	= kJTrue;

	JSize i = 1;
	while((i <= 30) && !itsIsFinished)
		{
		do
			{
			itsSomethingRead = kJFalse;
			ACE_Time_Value timeout(0);
			(ACE_Reactor::instance())->handle_events(timeout);
			ACE_Reactor::check_reconfiguration(NULL);

//			itsLink->handle_input(0);
			}
		while (itsSomethingRead);// && !itsIsFinished);

		JWait(2);
		i++;
		}
	if (itsIsFinished)
		{
		delete this;
		}
}
