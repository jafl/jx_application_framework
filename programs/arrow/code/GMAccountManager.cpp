/******************************************************************************
 GMAccountManager.cc

	<Description>

	BASE CLASS = public JPrefObject, virtual public JBroadcaster

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMAccountManager.h>
#include <GMAccount.h>
#include <GMAccountDialog.h>
#include <GPrefsMgr.h>
#include <GMGlobals.h>
#include <GPOPRetriever.h>

#include <JXApplication.h>

#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jSysUtil.h>

#include <unistd.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 1;

const JCharacter* GMAccountManager::kAccountsChanged = "kAccountsChanged::GMAccountManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMAccountManager::GMAccountManager
	(
	)
	:
	JPrefObject(GGetPrefsMgr(), kGAccountMgrID),
	itsFirstTime(kJTrue),
	itsDialog(NULL),
	itsDefaultIndex(1)
{
	itsAccounts = new JPtrArray<GMAccount>(JPtrArrayT::kForgetAll);
	assert(itsAccounts != NULL);

	itsRetrievers = new JPtrArray<GPOPRetriever>(JPtrArrayT::kForgetAll);
	assert(itsRetrievers != NULL);

	itsPendingRetrievers = new JPtrArray<GPOPRetriever>(JPtrArrayT::kForgetAll);
	assert(itsPendingRetrievers != NULL);

	JPrefObject::ReadPrefs();

	if (itsFirstTime)
		{
		GenerateDefaultAccount();
		}

	GeneratePOPRetreivers();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMAccountManager::~GMAccountManager()
{
	JPrefObject::WritePrefs();

	itsAccounts->DeleteAll();
	delete itsAccounts;

	itsRetrievers->DeleteAll();
	delete itsRetrievers;

	itsPendingRetrievers->DeleteAll();
	delete itsPendingRetrievers;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GMAccountManager::ReadPrefs
	(
	istream& input
	)
{
	itsFirstTime	= kJFalse;

	JFileVersion version;
	input >> version;

	if (version > kCurrentPrefsVersion)
		{
		return;
		}
	JSize count;
	input >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= new GMAccount(input);
		assert(account != NULL);
		itsAccounts->Append(account);
		}
	if (version > 0)
		{
		input >> itsDefaultIndex;
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GMAccountManager::WritePrefs
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';

	const JSize count	= itsAccounts->GetElementCount();
	output << ' ' << count << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccounts->NthElement(i);
		account->WritePrefs(output);
		}

	output << ' ' << itsDefaultIndex << ' ';
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMAccountManager::Receive
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
			GetEditedAccounts();
			}
		else
			{
			JPtrArray<GMAccount>& list	= itsDialog->GetAccountInfoArray();
			const JSize count	= list.GetElementCount();
			for (JIndex i = 1; i <= count; i++)
				{
				list.NthElement(i)->SetReadList(NULL);
				}
			}
		itsDialog = NULL;
		}
	else if (message.Is(GPOPRetriever::kCheckFinished))
		{
		GPOPRetriever* r	= itsPendingRetrievers->NthElement(1);
		StopListening(r);
		itsPendingRetrievers->RemoveElement(1);
		CheckNextAccount();
		}

	else if (message.Is(GPOPRetriever::kCheckFailed))
		{
		itsPendingRetrievers->RemoveAll();
		}
}

/******************************************************************************
 EditAccounts (public)

 ******************************************************************************/

void
GMAccountManager::EditAccounts()
{
	if (!FinishedChecking())
		{
		JGetUserNotification()->ReportError("I haven't finished checking the mail yet.");
		return;
		}
	assert(itsDialog == NULL);
	itsDialog = new GMAccountDialog(JXGetApplication(), *itsAccounts, itsDefaultIndex);
	assert(itsDialog != NULL);
	ListenTo(itsDialog);
	itsDialog->BeginDialog();
}

/******************************************************************************
 GenerateDefaultAccount (private)

 ******************************************************************************/

void
GMAccountManager::GenerateDefaultAccount()
{
	GMAccount* account	= new GMAccount();
	assert(account != NULL);

	account->SetDefaultInbox(GGetPrefsMgr()->GetDefaultInbox());
	account->SetSMTPServer(GGetPrefsMgr()->GetSMTPHost());
	account->SetSMTPUser(GGetPrefsMgr()->GetSMTPUser());
	account->SetReplyTo(GGetPrefsMgr()->GetReplyTo());
	JString from	= GGetPrefsMgr()->GetCustomFromString();
	JIndex findex;
	if (from.LocateLastSubstring("<", &findex) && findex > 1)
		{
		JString name	= from.GetSubstring(1, findex - 1);
		name.TrimWhitespace();
		if (!name.IsEmpty())
			{
			account->SetFullName(name);
			}
		}

	account->SetNickname(JGetUserName());

	account->ShouldUsePOP(GGetPrefsMgr()->UsingPopServer());
	if (account->IsUsingPOP())
		{
		account->ShouldLeaveMailOnServer(GGetPrefsMgr()->LeaveOnServer());
		account->ShouldSavePass(GGetPrefsMgr()->RememberPasswd());
		account->SetPassword(GGetPrefsMgr()->GetPopPasswd());
		account->ShouldUseAPOP(GGetPrefsMgr()->UsingAPop());
		account->ShouldAutoCheck(GGetPrefsMgr()->AutoCheckingMail());
		if (account->IsAutoChecking())
			{
			account->SetCheckInterval(GGetPrefsMgr()->CheckMailDelay());
			}
		account->SetPOPServer(GGetPrefsMgr()->GetPopServer());
		account->SetPOPAccount(GGetPrefsMgr()->GetPopUserName());
		}

	if (GGetPrefsMgr()->HasUIDList())
		{
		JPtrArray<JString>* list = new JPtrArray<JString>(JPtrArrayT::kForgetAll);
		assert(list != NULL);
		GGetPrefsMgr()->GetUIDList(*list);
		account->SetReadList(list);
		}

	JString home;
	if (JGetHomeDirectory(&home))
		{
		JAppendDirSeparator(&home);
		JString sig = home + ".signature";
		if (JFileExists(sig))
			{
			JString data;
			JReadFile(sig, &data);
			account->SetSig(data);
			}
		}

	itsAccounts->Append(account);
}

/******************************************************************************
 GeneratePOPRetreivers (private)

 ******************************************************************************/

void
GMAccountManager::GeneratePOPRetreivers()
{
	assert(FinishedChecking());
	itsRetrievers->DeleteAll();

	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccounts->NthElement(i);
		if (account->IsUsingPOP())
			{
			GPOPRetriever* r	= new GPOPRetriever(account);
			assert(r != NULL);
			itsRetrievers->Append(r);
			}
		}
}

/******************************************************************************
 GetEditedAccounts (private)

 ******************************************************************************/

void
GMAccountManager::GetEditedAccounts()
{
	assert(itsDialog != NULL);
	JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsAccounts->NthElement(i)->SetReadList(NULL);
		}
	itsAccounts->DeleteAll();

	JPtrArray<GMAccount>& list	= itsDialog->GetAccountInfoArray();
	count	= list.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsAccounts->Append(list.NthElement(i));
		}
	list.RemoveAll();

	GeneratePOPRetreivers();

	itsDefaultIndex	= itsDialog->GetDefaultIndex();

	Broadcast(AccountsChanged());
}

/******************************************************************************
 GetPOPAccountCount (public)

 ******************************************************************************/

JSize
GMAccountManager::GetPOPAccountCount()
	const
{
	return itsRetrievers->GetElementCount();
}

/******************************************************************************
 GetPOPAccounts (public)

 ******************************************************************************/

void
GMAccountManager::GetPOPAccounts
	(
	JPtrArray<JString>* accounts
	)
{
	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccounts->NthElement(i);
		if (account->IsUsingPOP())
			{
			JString* str	= new JString(account->GetNickname());
			assert(str != NULL);
			accounts->Append(str);
			}
		}
}

/******************************************************************************
 CheckAccount (public)

 ******************************************************************************/

void
GMAccountManager::CheckAccount
	(
	const JIndex index
	)
{
	assert(itsRetrievers->IndexValid(index));
	GPOPRetriever* r	= itsRetrievers->NthElement(index);
	itsPendingRetrievers->Append(r);
	CheckNextAccount();
}

/******************************************************************************
 CheckAccount (public)

 ******************************************************************************/

void
GMAccountManager::CheckAccount
	(
	GPOPRetriever* r
	)
{
	itsPendingRetrievers->Append(r);
	CheckNextAccount();
}

/******************************************************************************
 CheckAllAccounts (public)

 ******************************************************************************/

void
GMAccountManager::CheckAllAccounts()
{
	assert(FinishedChecking());
	const JSize count	= itsRetrievers->GetElementCount();
	for (JIndex index = 1; index <= count; index++)
		{
		GPOPRetriever* r	= itsRetrievers->NthElement(index);
		itsPendingRetrievers->Append(r);
		}
	CheckNextAccount();
}

/******************************************************************************
 FinishedChecking (public)

 ******************************************************************************/

JBoolean
GMAccountManager::FinishedChecking()
	const
{
	return JI2B(itsPendingRetrievers->GetElementCount() == 0);
}

/******************************************************************************
 CheckNextAccount (private)

 ******************************************************************************/

void
GMAccountManager::CheckNextAccount()
{
	if (FinishedChecking())
		{
		return;
		}
	GPOPRetriever* r	= itsPendingRetrievers->NthElement(1);
	ListenTo(r);
	r->CheckMail();
}

/******************************************************************************
 GetPOPAccount (public)

 ******************************************************************************/

JBoolean
GMAccountManager::GetPOPAccount
	(
	const JCharacter*	accountName,
	GMAccount**			account
	)
{
	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* a	= itsAccounts->NthElement(i);
		if (a->IsUsingPOP() && a->GetFullPOPAccountName() == accountName)
			{
			*account	= a;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 GetAccountWithNickname (public)

 ******************************************************************************/

JBoolean
GMAccountManager::GetAccountWithNickname
	(
	const JCharacter*	nickname,
	GMAccount**			account
	)
{
	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* a	= itsAccounts->NthElement(i);
		if (a->GetNickname() == nickname)
			{
			*account	= a;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 GetReplyAccount (public)

 ******************************************************************************/

JBoolean
GMAccountManager::GetReplyAccount
	(
	const JCharacter*	to,
	GMAccount**			account
	)
{
	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* a	= itsAccounts->NthElement(i);
//		cout << "\tCompare: " << a->GetSMTPUser() << endl;
		if (a->GetSMTPUser() == to)
			{
			*account	= a;
			return kJTrue;
			}
		}

	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* a	= itsAccounts->NthElement(i);
		if (a->GetReplyTo() == to)
			{
//			cout << "\tCompare: " << a->GetReplyTo() << endl;
			*account	= a;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetFirstAccount (public)

 ******************************************************************************/

GMAccount*
GMAccountManager::GetFirstAccount()
{
	assert(itsAccounts->GetElementCount() > 0);
	return itsAccounts->NthElement(1);
}

/******************************************************************************
 GetAccountCount (public)

 ******************************************************************************/

JSize
GMAccountManager::GetAccountCount()
	const
{
	return itsAccounts->GetElementCount();
}

/******************************************************************************
 GetAccount (public)

 ******************************************************************************/

GMAccount*
GMAccountManager::GetAccount
	(
	const JIndex index
	)
{
	assert(itsAccounts->IndexValid(index));
	GMAccount* account	= itsAccounts->NthElement(index);
	return account;
}

/******************************************************************************
 GetAccountNames (public)

 ******************************************************************************/

void
GMAccountManager::GetAccountNames
	(
	JPtrArray<JString>* accounts
	)
{
	const JSize count	= itsAccounts->GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMAccount* account	= itsAccounts->NthElement(i);
		JString* str	= new JString(account->GetNickname());
		assert(str != NULL);
		accounts->Append(str);
		}
}

/******************************************************************************
 GetDefaultAccount (public)

 ******************************************************************************/

GMAccount*
GMAccountManager::GetDefaultAccount()
{
	assert(itsAccounts->IndexValid(itsDefaultIndex));
	return itsAccounts->NthElement(itsDefaultIndex);
}
