/******************************************************************************
 GPOPRetriever.cc

	<Description>

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 1998 by Glenn W. Bach.  All rights reserved.

	Need to use JAsynchDataReceiver, to get chunks of data rather than
	lines.

 *****************************************************************************/

// includes

//Class Header
#include "GPOPRetriever.h"
#include "GMAccountManager.h"
#include "GPrefsMgr.h"
#include "GNetDebugDir.h"
#include "GMessageDataUpdateTask.h"
#include "GMAccount.h"
#include "GInboxMgr.h"
#include "md5.h"

#include "GMGlobals.h"

#include <JXGetStringDialog.h>
#include <JXTimerTask.h>

#include <JLatentPG.h>
#include <JOutPipeStream.h>
#include <JProcess.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>

#include <jDirUtil.h>
#include <jErrno.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jTime.h>

#include <j_prep_ace.h>
#include <ace/Connector.h>
#include <ace/INET_Addr.h>
#include <ace/UNIX_Addr.h>

#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <jAssert.h>

#undef new
#undef delete

typedef JMessageProtocol<ACE_SOCK_STREAM>			InetLink;
typedef ACE_Connector<InetLink, ACE_SOCK_CONNECTOR>	INETConnector;

enum
{
	kStartup = 1,
	kAPop,
	kUser,
	kPasswd,
	kList,
	kRetr,
	kDelete,
	kQuit
};

static const JSize	kMaxWaitSeconds		= 25;
static const JSize	kPGWaitWaitSeconds	= 2;

static const JCharacter* kDownloadMsg	= "Downloading new messages...";
static const JCharacter* kInternalData	= "\nFrom: Mail System Internal Data";

const JCharacter* GPOPRetriever::kCheckFinished	= "kCheckFinished::GPOPRetriever";
const JCharacter* GPOPRetriever::kCheckFailed	= "kCheckFailed::GPOPRetriever";

/******************************************************************************
 Constructor

 *****************************************************************************/

GPOPRetriever::GPOPRetriever
	(
	GMAccount* account
	)
	:
	itsAccount(account)
{
	itsLink			= NULL;
	itsDialog			= NULL;
	itsReadUIDList		= NULL;
	itsUIDList			= NULL;
	itsNewUIDIndexes	= NULL;
	itsTimerTask		= NULL;
	itsDeleteTask		= NULL;
	itsTimeoutTask		= NULL;
	itsPGCheckTask		= NULL;
	itsDeadLinkTask		= NULL;
	itsProcmailProcess	= NULL;
	itsMessageBuffer	= NULL;
	itsCurrentMode		= kStartup;
	itsCurrentIndex	= 1;
	itsMessageCount	= 0;
	itsListLinesCount	= 0;
	itsPG				= NULL;
	itsSomethingRead	= kJFalse;
	itsOutStream		= NULL;
	itsBroadcastOnFinish	= kJTrue;

// These relate to the retrieval of individual messages.
// When a "retr" command is set, "itsMessageStarting" is set to kJTrue
// because we need to pull out the response (ie. +OK) string before
// saving the message.

	itsMessageStarting	= kJTrue;
	itsMessageFinished	= kJFalse;

	itsListStarting		= kJTrue;

	if (itsAccount->IsSavingPass())
		{
		itsPasswd = itsAccount->GetPassword();
		}

	if (itsAccount->IsLeavingMailOnServer())
		{
		itsReadUIDList = itsAccount->GetReadList();
		assert(itsReadUIDList != NULL);

		itsUIDList = new JPtrArray<JString>(JPtrArrayT::kForgetAll);
		assert(itsUIDList != NULL);

		itsNewUIDIndexes = new JArray<JIndex>;
		assert(itsNewUIDIndexes != NULL);
		}

	if (itsAccount->IsAutoChecking())
		{
		JInteger delay = itsAccount->GetCheckInterval();
		itsTimerTask = new JXTimerTask(delay * 60 * 1000);
		assert(itsTimerTask != NULL);
		itsTimerTask->Start();
		ListenTo(itsTimerTask);
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPOPRetriever::~GPOPRetriever()
{
	delete itsLink;
	if (itsUIDList != NULL)
		{
		itsUIDList->DeleteAll();
		delete itsUIDList;
		}
	if (itsNewUIDIndexes != NULL)
		{
		delete itsNewUIDIndexes;
		}
	if (itsMessageBuffer != NULL)
		{
		delete itsMessageBuffer;
		itsMessageBuffer = NULL;
		}

	if (itsTimeoutTask != NULL)
		{
		delete itsTimeoutTask;
		}
	if (itsPGCheckTask != NULL)
		{
		delete itsPGCheckTask;
		}
}

/******************************************************************************
 Receive

 *****************************************************************************/

void
GPOPRetriever::Receive
	(
	JBroadcaster*					sender,
	const JBroadcaster::Message&	message
	)
{
	if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		if (itsTimeoutTask != NULL)
			{
			delete itsTimeoutTask;
			itsTimeoutTask	= NULL;
			}
		ReadReturnValue();
		}
	else if (sender == itsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert(info != NULL);
		if (info->Successful())
			{
			itsPasswd = itsDialog->GetString();
			if (!itsPasswd.IsEmpty())
				{
				if (itsAccount->IsSavingPass())
					{
					itsAccount->SetPassword(itsPasswd);
					}
				ConnectToServer();
				}
			}
		else
			{
			Broadcast(CheckFailed());
			}
		itsDialog = NULL;
		}
	else if (message.Is(JXTimerTask::kTimerWentOff))
		{
		if (sender == itsTimerTask)
			{
			GGetAccountMgr()->CheckAccount(this);
			}
		else if (sender == itsDeleteTask)
			{
			delete itsLink;
			itsLink = NULL;
			itsDeleteTask = NULL;
			if (itsBroadcastOnFinish)
				{
				Broadcast(CheckFinished());
				}
			else
				{
				itsBroadcastOnFinish	= kJTrue;
				}
			}
		else if (sender == itsTimeoutTask)
			{
			JGetUserNotification()->ReportError("The connection to the POP server timed out.");
			Broadcast(CheckFailed());
			itsLink		= NULL;
			itsTimeoutTask	= NULL;
			}
		else if (sender == itsPGCheckTask)
			{
			if (itsPG != NULL)
				{
				JBoolean stop	= kJFalse;;
				if (itsLink != NULL)
					{
					if (itsLink->ReceivedDisconnect())
						{
						JGetUserNotification()->ReportError("The connection has been broked.");
						stop	= kJTrue;
						}
					}
				if (stop || !itsPG->ProcessContinuing())
					{
					Broadcast(CheckFailed());
					itsPG->ProcessFinished();
					if (itsLink != NULL)
						{
						delete itsLink;
						itsLink	= NULL;
						}
					ReadFinished();
					}
				}
			}
		}
	else if (sender == itsProcmailProcess && message.Is(JProcess::kFinished))
		{
		GMessageDataUpdateTask::StopUpdate(kJFalse);
		GGetInboxMgr()->ShouldBePostponingUpdate(kJFalse);
		delete itsProcmailProcess;
		itsProcmailProcess = NULL;
		}
}

/******************************************************************************
 CheckMail

 *****************************************************************************/

void
GPOPRetriever::CheckMail()
{
	if ((itsDialog != NULL) || (itsLink != NULL))
		{
		return;
		}

	JString inbox = itsAccount->GetPOPInbox();
	JString lockfile = JString(inbox) + ".lock";
	if (JFileExists(lockfile))
		{
		JString report = "Your inbox \"" + inbox + "\" is locked."
						" No mail can be retrieved until this lock is removed."
						" If another program is currently using this file"
						" it will remove the lock when it is finished."
						" Do you want to forcibly remove the lock?";

		JBoolean ok = JGetUserNotification()->AskUserYes(report);
		if (ok)
			{
			GUnlockFile(inbox);
			}
		else
			{
			Broadcast(CheckFailed());
			return;
			}
		}
	if (itsPasswd.IsEmpty())
		{
		assert(itsDialog == NULL);

		const JCharacter* map[] =
			{
			"account", itsAccount->GetNickname()
			};
		const JString msg = JGetString("PasswordPrompt::GPOPRetreiver", map, sizeof(map));

		itsDialog =
			new JXGetStringDialog(JXGetApplication(), JGetString("GMGetPasswordTitle"),
								  msg, NULL, kJTrue, kJTrue);
		assert(itsDialog != NULL);
		ListenTo(itsDialog);
		itsDialog->BeginDialog();
		}
	else
		{
		ConnectToServer();
		}
}

/******************************************************************************
 ConnectToServer

	If the necessary settings are available in the preferences (ie. pop
	server and user name), a connection is made here to the pop socket
	(110) of the pop server.

 *****************************************************************************/

void
GPOPRetriever::ConnectToServer()
{
	JString addrStr = itsAccount->GetPOPServer();
	if (!addrStr.Contains(":"))
		{
		addrStr	+= ":110";
		}
	ACE_INET_Addr addr(addrStr);

	itsLink = new InetLink;
	assert(itsLink != NULL);
	itsLink->SetProtocol(
		JMessageProtocolT::kUNIXSeparatorStr, 
		JMessageProtocolT::kUNIXSeparatorLength,
		"\0\0", 2);

	INETConnector* connector = new INETConnector;
	assert( connector != NULL );

	ACE_Synch_Options options(ACE_Synch_Options::USE_REACTOR, ACE_Time_Value(kMaxWaitSeconds));

	if (connector->connect(itsLink, addr, options) == -1 &&
		jerrno() != EAGAIN)
		{
		JString notice = "No response from the POP server.";
		JGetUserNotification()->ReportError(notice);
		itsLink = NULL;
		Broadcast(CheckFailed());
		}
	else
		{
		itsTimeoutTask = new JXTimerTask(kMaxWaitSeconds * 1000,kJTrue);
		assert( itsTimeoutTask != NULL );
		itsTimeoutTask->Start();
		ListenTo(itsTimeoutTask);
		ListenTo(itsLink);
		}
}

/******************************************************************************
 ReadReturnValue

	If using APOP, the timestamp returned from the server must be retreived.
	That's what the regex: <[[:digit:]]+[-.][[:digit:]]+@[^>]+> is for. If
	the response does not match this regex, I assume that it does not support
	APOP.

 *****************************************************************************/

void
GPOPRetriever::ReadReturnValue()
{
	JString line;
	JBoolean ok = itsLink->GetNextMessage(&line);
	assert(ok);
	itsSomethingRead	= kJTrue;
	if (((itsCurrentMode != kRetr) && (itsCurrentMode != kList)) ||
		((itsCurrentMode == kRetr) && itsMessageStarting) ||
		((itsCurrentMode == kList) && itsListStarting))
		{
		JIndex findex;
		ok = line.LocateSubstring(" ", &findex);
		JString response;
		if (ok)
			{
			assert(findex > 1);
			response = line.GetSubstring(1, findex - 1);
			}
		else
			{
			response = line;
			}
		if (!response.BeginsWith("+OK"))
			{
			itsDeleteTask = new JXTimerTask(10,kJTrue);
			assert(itsDeleteTask != NULL);
			itsDeleteTask->Start();
			ListenTo(itsDeleteTask);
			StopListening(itsLink);
			JGetUserNotification()->ReportError(line);
			if ((itsCurrentMode == kPasswd) || (itsCurrentMode == kAPop))
				{
				itsPasswd = "";
				if (itsAccount->IsSavingPass())
					{
					itsAccount->SetPassword(itsPasswd);
					}
				assert(itsDialog == NULL);

				const JCharacter* map[] =
					{
					"account", itsAccount->GetNickname()
					};
				const JString msg = JGetString("PasswordPrompt::GPOPRetreiver", map, sizeof(map));

				itsDialog = new JXGetStringDialog(JXGetApplication(), JGetString("GMGetPasswordTitle"),
												  msg, NULL, kJTrue, kJTrue);
				assert(itsDialog != NULL);
				ListenTo(itsDialog);
				itsDialog->BeginDialog();
				itsBroadcastOnFinish	= kJFalse;
				}
			itsCurrentMode		= kStartup;
			itsCurrentIndex	= 1;
			itsMessageStarting	= kJTrue;
			itsListStarting		= kJTrue;
			if (itsMessageBuffer != NULL)
				{
				delete itsMessageBuffer;
				itsMessageBuffer = NULL;
				}
			return;
			}
		}

	if (itsCurrentMode == kStartup)
		{
		GMGetPOPDebugDir()->AddText(line);
		if (itsAccount->IsUsingAPOP())
			{
			itsCurrentMode = kAPop;

		// Get the time stamp
			JRegex regex;
			JBoolean matched;
			JArray<JIndexRange>* subList = new JArray<JIndexRange>;
			assert(subList != NULL);
			JError err = regex.SetPattern("<[[:digit:]]+[-.][[:digit:]]+@[^>]+>");
			assert(err.OK());
			matched = regex.Match(line, subList);
			if (matched)
				{
				JIndexRange sRange = subList->GetElement(1);
				itsTimeStamp = line.GetSubstring(sRange);
				}
			else
				{
				itsDeleteTask = new JXTimerTask(10,kJTrue);
				assert(itsDeleteTask != NULL);
				itsDeleteTask->Start();
				ListenTo(itsDeleteTask);
				JString report = "This server does not support APOP";
				JGetUserNotification()->ReportError(report);
				itsCurrentMode		= kStartup;
				itsCurrentIndex	= 1;
				itsMessageStarting	= kJTrue;
				itsListStarting		= kJTrue;
				if (itsMessageBuffer != NULL)
					{
					delete itsMessageBuffer;
					itsMessageBuffer = NULL;
					}
				return;
				}
			}
		else
			{
			itsCurrentMode = kUser;
			}
		}
	else if (itsCurrentMode == kAPop)
		{
		GMGetPOPDebugDir()->AddText(line);
		itsCurrentMode = kList;
		}
	else if (itsCurrentMode == kList)
		{
		GMGetPOPDebugDir()->AddText(line);
		if (!itsListStarting)
			{
			if (line == "." || line == ".\r")
				{
				if (itsAccount->IsLeavingMailOnServer())
					{
					HandleUIDL();
					}
				else
					{
					itsMessageCount = itsListLinesCount;
					}

				if (itsMessageCount == 0)
					{
					itsCurrentMode = kQuit;
					}
				else
					{
					itsCurrentMode++;
					}
				itsListStarting = kJTrue;
				}
			else
				{
				if (itsAccount->IsLeavingMailOnServer())
					{
					JString* str = new JString(line);
					assert(str != NULL);
					JIndex findex;
					if (str->LocateSubstring(" ", &findex) && (findex > 1))
						{
						str->RemoveSubstring(1, findex);
						str->TrimWhitespace();
						}
					itsUIDList->Append(str);
					}
				itsListLinesCount++;
				}
			}
		else
			{
			itsListStarting	= kJFalse;
			itsListLinesCount	= 0;
			}
		}
	else if (itsCurrentMode < kRetr)
		{
		GMGetPOPDebugDir()->AddText(line);
		itsCurrentMode++;
		}
	else if (itsCurrentMode == kRetr)
		{
		if (!itsMessageStarting)
			{
			if ((line == ".") || (line == ".\r"))
				{
				JBoolean ok = SaveMessage();
				if (!ok)
					{
					itsDeleteTask = new JXTimerTask(10,kJTrue);
					assert(itsDeleteTask != NULL);
					itsDeleteTask->Start();
					ListenTo(itsDeleteTask);
					StopListening(itsLink);
					JString report("Arrow needs the formail and procmail programs"
									" to download mail, but was unable"
									" to run them. Please make sure they"
									" are installed on your system.");
					JGetUserNotification()->ReportError(report);
					itsCurrentMode		= kStartup;
					itsCurrentIndex	= 1;
					itsMessageStarting	= kJTrue;
					itsListStarting		= kJTrue;
					return;
					}
				itsMessageBuffer->Clear();
				itsMessageFinished = kJTrue;
				itsMessageStarting = kJTrue;
				if (itsCurrentIndex <= itsMessageCount)
					{
					itsCurrentIndex++;
					}
				if (itsCurrentIndex <= itsMessageCount)
					{
					if (!itsPG->IncrementProgress())
						{
						ReadFinished();
						}
					}
				}
			else
				{
				if (!line.IsEmpty() && line.GetLastCharacter() == '\r')
					{
					line.RemoveSubstring(line.GetLength(),line.GetLength());
					}
				if (itsMessageBuffer == NULL)
					{
					itsMessageBuffer = new JString();
					assert(itsMessageBuffer != NULL);
					}
				*itsMessageBuffer += line;
				*itsMessageBuffer += "\n";
				}
			}
		else
			{
			itsMessageStarting = kJFalse;
			GMGetPOPDebugDir()->AddText("Data not shown...");
			}
		if ((itsCurrentIndex > itsMessageCount) && itsMessageFinished)
			{
			itsPG->ProcessFinished();
			ReadFinished();
			}
		}
	else if (itsCurrentMode == kDelete)
		{
		GMGetPOPDebugDir()->AddText(line);
		if (itsCurrentIndex < itsMessageCount)
			{
			itsCurrentIndex++;
			}
		else
			{
			itsCurrentMode++;
			}
		}
	else if (itsCurrentMode == kQuit)
		{
		GMGetPOPDebugDir()->AddText(line);
		itsCurrentMode	= kStartup;
		itsMessageStarting = kJTrue;
		itsCurrentIndex = 1;
		itsDeleteTask = new JXTimerTask(10,kJTrue);
		assert(itsDeleteTask != NULL);
		itsDeleteTask->Start();
		ListenTo(itsDeleteTask);
		if (itsMessageBuffer != NULL)
			{
			delete itsMessageBuffer;
			itsMessageBuffer = NULL;
			}
		StopListening(itsLink);
		}

	SendNextData();
}

/******************************************************************************
 SendNextData

 *****************************************************************************/

void
GPOPRetriever::SendNextData()
{
	if (itsCurrentMode == kAPop)
		{
		JString senddata = "apop " + itsAccount->GetPOPAccount();
		senddata += " " + EncryptedTimeAndPass();
		SendLine(senddata);
		}
	else if (itsCurrentMode == kUser)
		{
		JString senddata = "user " + itsAccount->GetPOPAccount();
		SendLine(senddata);
		}
	else if (itsCurrentMode == kPasswd)
		{
		JString senddata = "pass " + itsPasswd;
		SendLine(senddata);
		}
	else if (itsCurrentMode == kList)
		{
		if (itsListStarting)
			{
			if (itsAccount->IsLeavingMailOnServer())
				{
				itsUIDList->DeleteAll();
				itsNewUIDIndexes->RemoveAll();
				SendLine("uidl");
				}
			else
				{
				SendLine("list");
				}
			}
		}
	else if ((itsCurrentMode == kRetr) && itsMessageStarting)
		{
		assert(itsCurrentIndex <= itsMessageCount);
		JString senddata = "retr ";
		if (itsAccount->IsLeavingMailOnServer())
			{
			senddata += JString(itsNewUIDIndexes->GetElement(itsCurrentIndex));
			}
		else
			{
			senddata += JString(itsCurrentIndex);
			}
		SendLine(senddata);
		}
	else if (itsCurrentMode == kDelete)
		{
		assert(itsCurrentIndex <= itsMessageCount);
		JString senddata = "dele " + JString(itsCurrentIndex);
		SendLine(senddata);
		}
	else if (itsCurrentMode == kQuit)
		{
		JString senddata = "quit";
		SendLine(senddata);
		}

	// perhaps here is the place to create the pg and only create it if
	// current mode is kRetr and pg == NULL
	// then, while mode == kRetr, call handle_input
	if (itsPG == NULL && itsCurrentMode == kRetr)
		{
		itsPG	= new JLatentPG();
		assert(itsPG != NULL);
		itsPG->SetMaxSilentTime(0);

		const JCharacter* map[] =
			{
			"account", itsAccount->GetNickname()
			};
		const JString msg = JGetString("DownloadMessage::GPOPRetreiver", map, sizeof(map));

		itsPG->FixedLengthProcessBeginning(itsMessageCount, msg, kJTrue, kJTrue);
		GMessageDataUpdateTask::StopUpdate(kJTrue);
		GGetInboxMgr()->ShouldBePostponingUpdate(kJTrue);

		itsPGCheckTask = new JXTimerTask(kPGWaitWaitSeconds * 1000);
		assert( itsPGCheckTask != NULL );
		itsPGCheckTask->Start();
		ListenTo(itsPGCheckTask);
		
/*		while (itsCurrentMode == kRetr)
			{
			do
				{
				itsSomethingRead	= kJFalse;
				itsLink->handle_input(ACE_INVALID_HANDLE);
				}
			while (itsSomethingRead);
			JWait(2);
			}*/
		}
}

/******************************************************************************
 SaveMessage

 The individual messages are delivered here via procmail. Since pop strips
 the From line from each message, we need to use "procmail -f -" which adds
 the From line. The nice thing about having procmail do this, is that the
 user automatically gets filtering via the procmailrc file:

	$HOME/.procmailrc

 The environment variable "DEFAULT" tells procmail where to deliver mail by
 default.

 The Create functions forks, and then execs procmail. The stdin of procmail
 is redirected (via dup2) to "toFD", so that it will read from our pipe.
 procmail reads until it detects an end-of-file on the pipe, which happens
 when we close the pipe. JOutPipeStream closes the pipe when we delete it,
 since we passed kJTrue as the second argument to its constructor.

 *****************************************************************************/

JBoolean
GPOPRetriever::SaveMessage()
{
	if (itsProcmailProcess == NULL && itsMessageBuffer != NULL)
		{
		int toFD;
		
		JString inbox	= itsAccount->GetPOPInbox();
		if (JIsRelativePath(itsAccount->GetPOPInbox()))
			{
			JString home;
			if (JGetHomeDirectory(&home))
				{
				inbox	= JCombinePathAndName(home, itsAccount->GetPOPInbox());
				}			
			}

		JString envStr = "DEFAULT=" + inbox;
		JString execStr = "formail -s procmail " + envStr;

		JError err = JProcess::Create(&itsProcmailProcess, execStr,
										kJCreatePipe, &toFD,
										kJIgnoreConnection, NULL,
										kJTossOutput, NULL);

		if (err.OK())
			{
			ListenTo(itsProcmailProcess);

			itsOutStream = new JOutPipeStream(toFD, kJTrue);
			assert(itsOutStream != NULL);

			//generate from and X-Arrow-Account lines
			itsAccountName	= itsAccount->GetFullPOPAccountName();

			time_t now;
			time(&now);
			itsFromLine	= "From " + itsAccountName + JString(" ") + ctime(&now);
			itsAccountName.Prepend("X-Arrow-Account: ");
			itsAccountName.AppendCharacter('\n');
			WriteMessageToStream();
			return kJTrue;
			}
		return kJFalse;
		}
	else if (itsProcmailProcess != NULL && itsOutStream != NULL && itsMessageBuffer != NULL)
		{
		WriteMessageToStream();
		}
	return kJTrue;
}

/******************************************************************************
 WriteMessageToStream (private)

 ******************************************************************************/

void
GPOPRetriever::WriteMessageToStream()
{
	if (itsMessageBuffer->Contains(kInternalData))
		{
		return;
		}
	itsMessageBuffer->Prepend(itsAccountName);
	itsMessageBuffer->Prepend(itsFromLine);
	itsMessageBuffer->Print(*itsOutStream);
	if (!itsMessageBuffer->EndsWith("\n\n"))
		{
		*itsOutStream << endl << endl;
		}
}


/******************************************************************************
 HandleUIDL

 In this function, I have to merge two id lists. One list (itsReadUIDList),
 that is saved in the prefs file, contains the list of unique id's (uids
 from uidl) that have been read. The other list (itsUIDList) is the set of
 ids that are currently on the pop server.

 First, I loop through each of the ids in itsUIDList and check if they are
 already in itsReadUIDList. If the are, I set the corresponding entry in
 stillPresent, otherwise I add the index of this id into itsNewUIDIndexes.

 Next, I remove any ids in itsReadUIDList that are no longer on the server
 by looping through the stillPresent array.

 Finally, I loop through itsNewUIDIndexes, and add the corresponding id
 from itsUIDList to itsReadUIDList.

 Note: The ids in itsUIDList have an index number prepended to them. This
 needs to be removed before searching or inserting.

 *****************************************************************************/

void
GPOPRetriever::HandleUIDL()
{
	const JSize readCount = itsReadUIDList->GetElementCount();
//cout << "Read count: " << readCount << endl;
	JArray<JBoolean> stillPresent;
	for (JSize i = 1; i <= readCount; i++)
		{
		stillPresent.AppendElement(kJFalse);
		}

	const JSize count = itsUIDList->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString id = *(itsUIDList->NthElement(i));
		JIndex findex;
		if (itsReadUIDList->SearchSorted(&id, JOrderedSetT::kAnyMatch, &findex))
			{
			stillPresent.SetElement(findex, kJTrue);
			}
		else
			{
			itsNewUIDIndexes->AppendElement(i);
			}
		}

	for (JSize i = readCount; i >= 1; i--)
		{
		if (!stillPresent.GetElement(i))
			{
			itsReadUIDList->DeleteElement(i);
			}
		}

	itsMessageCount	= itsNewUIDIndexes->GetElementCount();
}

/******************************************************************************
 EncryptedTimeAndPass

 This function is needed for authenticated pop (APOP).

 When the pop3 session is started, a timestamp is sent from the server to the
 client. The timestamp has the form "<pid.time@hostname>", ie:

	<6977.900359467@dummy.caltech.edu>

 To use apop the user must have a pop password on the server. This password
 is added via popauth. Instead of using user/pass for authentication (which
 sends the password in plaintext), apop is used in the form:

	apop username authentication-string

 where the authentication-string is formed by appending the user's pop
 password to the timestamp shown above, and performing an md5 sum on the
 resulting string. This value is then converted to a hex text string.

 *****************************************************************************/

JString
GPOPRetriever::EncryptedTimeAndPass()
{
	MD5_CTX mdContext;
	unsigned char digest[16];

	JString buffer = itsTimeStamp + itsPasswd;
	char* charbuffer = strdup(buffer.GetCString());

	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char *)charbuffer, strlen(charbuffer));
	MD5Final(digest, &mdContext);

	free(charbuffer);

	JString encrypted;
	char charchunk[5];
	for (int i = 0 ; i < 16; i++)
		{
		sprintf(charchunk, "%02x", digest[i]);
		encrypted += charchunk;
		}

	return encrypted;
}

/******************************************************************************
 SendLine (private)

 ******************************************************************************/

void
GPOPRetriever::SendLine
	(
	const JCharacter* line
	)
{
	JString sLine(line);
	SendLine(sLine);
}

void
GPOPRetriever::SendLine
	(
	const JString& line
	)
{
	JString sLine(line);
	sLine += "\r";
	itsLink->SendMessage(sLine);
	if (itsCurrentMode == kPasswd)
		{
		GMGetPOPDebugDir()->AddText("pass xxxxxxxx");
		}
	else
		{
		GMGetPOPDebugDir()->AddText(line);
		}
}

/******************************************************************************
 ReadFinished (private)

 ******************************************************************************/

void
GPOPRetriever::ReadFinished()
{
	if (itsAccount->IsLeavingMailOnServer())
		{
		for (JSize i = 1; i <= itsCurrentIndex - 1; i++)
			{
			JIndex newIndex = itsNewUIDIndexes->GetElement(i);
			assert(itsUIDList->IndexValid(newIndex));
			JString* id = new JString(*(itsUIDList->NthElement(newIndex)));
			assert(id != NULL);
			if (!itsReadUIDList->InsertSorted(id, kJFalse))
				{
				delete id;
				}
			}
		itsNewUIDIndexes->RemoveAll();
		itsCurrentMode = kQuit;
		}
	else
		{
		itsCurrentMode = kDelete;
		}
	itsCurrentIndex = 1;
	itsPG	= NULL;

	delete itsPGCheckTask;
	itsPGCheckTask	= NULL;

	itsOutStream->close();
	delete itsOutStream;
}
