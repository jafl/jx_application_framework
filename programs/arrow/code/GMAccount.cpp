/******************************************************************************
 GMAccount.cc

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMAccount.h>
#include <jDirUtil.h>
#include <jSysUtil.h>
#include <jAssert.h>

const JFileVersion	kCurrentPrefsVersion	= 4;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMAccount::GMAccount()
	:
	itsReadUIDList(NULL)
{
	itsNickname	= JGetUserName();
	itsDefaultInbox	= "/var/spool/mail/" + itsNickname;
	JString home;
	if (JGetHomeDirectory(&home))
		{
		itsMailDir	= JCombinePathAndName(home, "mail");
		}
		
	itsSMTPServer	= "localhost";
	itsSMTPUser		= itsNickname + "@localhost";
	itsFullName		= JGetUserRealWorldName();
	itsSigType		= kManualSig;
	InitPOP();
}

GMAccount::GMAccount
	(
	istream& is
	)
	:
	itsReadUIDList(NULL)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}
	is >> itsNickname;
	is >> itsDefaultInbox;
	is >> itsSMTPServer;
	is >> itsSMTPUser;
	is >> itsReplyTo;
	if (version > 1)
		{
		is >> itsCC;
		}
	if (version > 2)
		{
		is >> itsBCC;
		is >> itsSig;
		int type;
		is >> type;
		itsSigType	= (SigType)type;
		}
	else
		{
		itsSigType	= kManualSig;
		}
	if (version == 0)
		{
		JString from;
		is >> from;
		JString email;
		SplitNameAndAddress(from, &itsFullName, &email);
		}
	else
		{
		is >> itsFullName;
		}
	if (version > 3)
		{
		is >> itsMailDir;
		is >> itsPOPInbox;
		}
	is >> itsUsePOP;
	if (itsUsePOP)
		{
		is >> itsLeaveMailOnServer;
		is >> itsSavePass;
		if (itsSavePass)
			{
			is >> itsPassword;
			}
		is >> itsUseAPOP;
		is >> itsAutoCheck;
		if (itsAutoCheck)
			{
			is >> itsAutoCheckInterval;
			}
		is >> itsPOPServer;
		is >> itsPOPAccount;
		if (itsLeaveMailOnServer)
			{
			itsReadUIDList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert(itsReadUIDList != NULL);
			itsReadUIDList->SetCompareFunction(JCompareStringsCaseSensitive);
			JSize count;
			is >> count;
			if (count > 0)
				{
				for (JIndex i = 1; i <= count; i++)
					{
					JString* str = new JString;
					is >> *str;
					itsReadUIDList->Append(str);
					}
				}
			}
		}
	else
		{
		InitPOP();
		}

	if (itsMailDir.IsEmpty())
		{
		JString home;
		if (JGetHomeDirectory(&home))
			{
			itsMailDir	= JCombinePathAndName(home, "mail");
			}
		}

	if (itsPOPInbox.IsEmpty())
		{
		JString home;
		if (JGetHomeDirectory(&home))
			{
			itsPOPInbox	= JCombinePathAndName(home, "mail/Inbox");
			}
		}
}

GMAccount::GMAccount
	(
	const GMAccount& source
	)
	:
	itsReadUIDList(NULL)
{
	CopyData(source);
}

/******************************************************************************
 operator=

 ******************************************************************************/

const GMAccount&
GMAccount::operator=
	(
	const GMAccount& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	CopyData(source);

	return *this;
}

/******************************************************************************
 CopyData (private)

 ******************************************************************************/

void
GMAccount::CopyData
	(
	const GMAccount& source
	)
{
	itsNickname				= source.itsNickname;
	itsDefaultInbox			= source.itsDefaultInbox;
	itsSMTPServer			= source.itsSMTPServer;
	itsSMTPUser				= source.itsSMTPUser;
	itsReplyTo				= source.itsReplyTo;
	itsCC					= source.itsCC;
	itsBCC					= source.itsBCC;
	itsSig					= source.itsSig;
	itsSigType				= source.itsSigType;
	itsFullName				= source.itsFullName;
	itsUsePOP				= source.itsUsePOP;
	itsLeaveMailOnServer	= source.itsLeaveMailOnServer;
	itsSavePass				= source.itsSavePass;
	itsPassword				= source.itsPassword;
	itsUseAPOP				= source.itsUseAPOP;
	itsAutoCheck			= source.itsAutoCheck;
	itsAutoCheckInterval	= source.itsAutoCheckInterval;
	itsPOPServer			= source.itsPOPServer;
	itsPOPAccount			= source.itsPOPAccount;
	itsMailDir              = source.itsMailDir;
	itsPOPInbox             = source.itsPOPInbox;
}

/******************************************************************************
 InitPOP (private)

 ******************************************************************************/

void
GMAccount::InitPOP()
{
	itsUsePOP		= kJFalse;
	itsSavePass		= kJFalse;
	itsUseAPOP		= kJFalse;
	itsAutoCheck	= kJFalse;
	itsAutoCheckInterval	= 30;
	itsLeaveMailOnServer	= kJFalse;

	JString home;
	if (JGetHomeDirectory(&home))
		{
		itsPOPInbox	= JCombinePathAndName(home, "mail/Inbox");
		}
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
GMAccount::WritePrefs
	(
	ostream& os
	)
{
	os << ' ' << kCurrentPrefsVersion << ' ';
	os << ' ' << itsNickname << ' ';
	os << ' ' << itsDefaultInbox << ' ';
	os << ' ' << itsSMTPServer << ' ';
	os << ' ' << itsSMTPUser << ' ';
	os << ' ' << itsReplyTo << ' ';
	os << ' ' << itsCC << ' ';
	os << ' ' << itsBCC << ' ';
	os << ' ' << itsSig << ' ';
	os << ' ' << (int)itsSigType << ' ';
	os << ' ' << itsFullName << ' ';
	os << ' ' << itsMailDir << ' ';
	os << ' ' << itsPOPInbox << ' ';
	os << ' ' << itsUsePOP << ' ';
	if (itsUsePOP)
		{
		os << ' ' << itsLeaveMailOnServer << ' ';
		os << ' ' << itsSavePass << ' ';
		if (itsSavePass)
			{
			os << ' ' << itsPassword << ' ';
			}
		os << ' ' << itsUseAPOP << ' ';
		os << ' ' << itsAutoCheck << ' ';
		if (itsAutoCheck)
			{
			os << ' ' << itsAutoCheckInterval << ' ';
			}
		os << ' ' << itsPOPServer << ' ';
		os << ' ' << itsPOPAccount << ' ';

		if (itsLeaveMailOnServer)
			{
			if (itsReadUIDList == NULL)
				{
				os << ' ' << 0 << ' ';
				}
			else
				{
				const JSize count = itsReadUIDList->GetElementCount();
				os << ' ' << count << ' ';
				for (JIndex i = 1; i <= count; i++)
					{
					os << ' ' << *(itsReadUIDList->NthElement(i)) << ' ';
					}
				}
			}
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMAccount::~GMAccount()
{
	delete itsReadUIDList;
}

/******************************************************************************
 GetFullPOPAccountName

 ******************************************************************************/

JString
GMAccount::GetFullPOPAccountName()
{
	JString account = itsPOPAccount + "@" + itsPOPServer;
	return account;
}

/******************************************************************************
 LeavingMailOnServer

 ******************************************************************************/

JBoolean
GMAccount::IsLeavingMailOnServer()
	const
{
	return itsLeaveMailOnServer;
}

void
GMAccount::ShouldLeaveMailOnServer
	(
	const JBoolean leave
	)
{
	itsLeaveMailOnServer	= leave;
	if (leave && itsReadUIDList == NULL)
		{
		itsReadUIDList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsReadUIDList != NULL);
		itsReadUIDList->SetCompareFunction(JCompareStringsCaseSensitive);
		}
}

/******************************************************************************
 SplitNameAndAddress (private)

 ******************************************************************************/

void
GMAccount::SplitNameAndAddress
	(
	const JString&	fromline,
	JString*		name,
	JString*		email
	)
{
	JIndex findex;
	if (fromline.LocateLastSubstring("<", &findex) && findex > 1)
		{
		*name	= fromline.GetSubstring(1, findex - 1);
		name->TrimWhitespace();
		JIndex lindex;
		if (fromline.LocateLastSubstring(">", &lindex) &&
			lindex > findex + 1)
			{
			*email	= fromline.GetSubstring(findex + 1, lindex - 1);
			}
		}
}

/******************************************************************************
 CompareNicknames

 ******************************************************************************/

JOrderedSetT::CompareResult
GMAccount::CompareNicknames
	(
	GMAccount* const & a1,
	GMAccount* const & a2
	)
{
	JString n1	= a1->GetNickname();
	JString n2	= a2->GetNickname();
	return JCompareStringsCaseInsensitive(&n1, &n2);
}
