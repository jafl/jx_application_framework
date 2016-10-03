/******************************************************************************
 GMAccount.h

	Interface for the GMAccount class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMAccount
#define _H_GMAccount

#include <JPtrArray-JString.h>

#include <jTypes.h>

class GMAccount
{
public:

	enum SigType
	{
		kPrependSig = 1,
		kAppendSig,
		kManualSig
	};

public:

	GMAccount();
	GMAccount(istream& is);

	GMAccount(const GMAccount& source);
	const GMAccount& operator=(const GMAccount& source);

	virtual ~GMAccount();

	void			WritePrefs(ostream& os);

	const JString&	GetNickname() const;
	void			SetNickname(const JCharacter* nickname);

	const JString&	GetDefaultInbox() const;
	void			SetDefaultInbox(const JCharacter* inbox);

	const JString&	GetSMTPServer() const;
	void			SetSMTPServer(const JCharacter* server);

	const JString&	GetSMTPUser() const;
	void			SetSMTPUser(const JCharacter* user);

	JString			GetFullPOPAccountName();

	const JString&	GetReplyTo() const;
	void			SetReplyTo(const JCharacter* replyto);

	const JString&	GetCC() const;
	void			SetCC(const JCharacter* cc);

	const JString&	GetBCC() const;
	void			SetBCC(const JCharacter* bcc);

	const JString&	GetSig() const;
	void			SetSig(const JCharacter* sig);

	SigType			GetSigType() const;
	void			SetSigType(const SigType type);

	const JString&	GetFullName() const;
	void			SetFullName(const JCharacter* from);

	JBoolean		IsUsingPOP() const;
	void			ShouldUsePOP(const JBoolean use);

	JBoolean		IsLeavingMailOnServer() const;
	void			ShouldLeaveMailOnServer(const JBoolean leave);

	JBoolean		IsSavingPass() const;
	void			ShouldSavePass(const JBoolean save);

	const JString&	GetPassword() const;
	void			SetPassword(const JCharacter* password);

	JBoolean		IsUsingAPOP() const;
	void			ShouldUseAPOP(const JBoolean use);

	JBoolean		IsAutoChecking() const;
	void			ShouldAutoCheck(const JBoolean check);

	JIndex			GetCheckInterval() const;
	void			SetCheckInterval(const JIndex interval);

	const JString&	GetPOPServer() const;
	void			SetPOPServer(const JCharacter* server);

	const JString&	GetPOPAccount() const;
	void			SetPOPAccount(const JCharacter* account);

	const JString&	GetMailDir() const;
	void			SetMailDir(const JCharacter* dir);

	const JString&	GetPOPInbox() const;
	void			SetPOPInbox(const JCharacter* inbox);

	JPtrArray<JString>* GetReadList();
	void				SetReadList(JPtrArray<JString>* list);

	static JOrderedSetT::CompareResult
		CompareNicknames(GMAccount* const & a1, GMAccount* const & a2);

protected:

private:

	JString		itsNickname;
	JString		itsDefaultInbox;
	JString		itsSMTPServer;
	JString		itsSMTPUser;
	JString		itsReplyTo;
	JString		itsCC;
	JString		itsBCC;
	JString		itsSig;
	SigType		itsSigType;
	JString		itsFullName;
	JBoolean	itsUsePOP;
	JBoolean	itsLeaveMailOnServer;
	JBoolean	itsSavePass;
	JString		itsPassword;
	JBoolean	itsUseAPOP;
	JBoolean	itsAutoCheck;
	JIndex		itsAutoCheckInterval;
	JString		itsPOPServer;
	JString		itsPOPAccount;
	JString		itsMailDir;
	JString		itsPOPInbox;

	JPtrArray<JString>*		itsReadUIDList;

private:

	void	CopyData(const GMAccount& source);
	void	InitPOP();
	void	SplitNameAndAddress(const JString& fromline, JString* name, JString* email);

};

/******************************************************************************
 Read list

	This is needed by the POP retreiver. Perhaps it would be best to let
	the retreiver be a friend, or do it with access functions, but for now
	I'll just do it this way.

 ******************************************************************************/

inline JPtrArray<JString>*
GMAccount::GetReadList()
{
	return itsReadUIDList;
}

inline void
GMAccount::SetReadList
	(
	JPtrArray<JString>* list
	)
{
	itsReadUIDList	= list;
}

/******************************************************************************
 Nickname

 ******************************************************************************/

inline const JString&
GMAccount::GetNickname()
	const
{
	return itsNickname;
}

inline void
GMAccount::SetNickname
	(
	const JCharacter* nickname
	)
{
	itsNickname	= nickname;
}

/******************************************************************************
 DefaultInbox

 ******************************************************************************/

inline const JString&
GMAccount::GetDefaultInbox()
	const
{
	return itsDefaultInbox;
}

inline void
GMAccount::SetDefaultInbox
	(
	const JCharacter* inbox
	)
{
	itsDefaultInbox	= inbox;
}

/******************************************************************************
 SMTPServer

 ******************************************************************************/

inline const JString&
GMAccount::GetSMTPServer()
	const
{
	return itsSMTPServer;
}

inline void
GMAccount::SetSMTPServer
	(
	const JCharacter* server
	)
{
	itsSMTPServer	= server;
}

/******************************************************************************
 SMTPUser

 ******************************************************************************/

inline const JString&
GMAccount::GetSMTPUser()
	const
{
	return itsSMTPUser;
}

inline void
GMAccount::SetSMTPUser
	(
	const JCharacter* user
	)
{
	itsSMTPUser	= user;
}

/******************************************************************************
 Mail dir

 *****************************************************************************/

inline const JString&
GMAccount::GetMailDir()
	const
{
	return itsMailDir;
}

inline void		
GMAccount::SetMailDir
	(
	const JCharacter* dir
	)
{
	itsMailDir = dir;
}

/******************************************************************************
 POP inbox

 *****************************************************************************/

inline const JString&
GMAccount::GetPOPInbox()
	const
{
	return itsPOPInbox;
}

inline void		
GMAccount::SetPOPInbox
	(
	const JCharacter* inbox
	)
{
	itsPOPInbox	= inbox;
}

/******************************************************************************
 ReplyTo

 ******************************************************************************/

inline const JString&
GMAccount::GetReplyTo()
	const
{
	return itsReplyTo;
}

inline void
GMAccount::SetReplyTo
	(
	const JCharacter* replyto
	)
{
	itsReplyTo	= replyto;
}

/******************************************************************************
 CC

 ******************************************************************************/

inline const JString&
GMAccount::GetCC()
	const
{
	return itsCC;
}

inline void
GMAccount::SetCC
	(
	const JCharacter* cc
	)
{
	itsCC	= cc;
}

/******************************************************************************
 BCC

 ******************************************************************************/

inline const JString&
GMAccount::GetBCC()
	const
{
	return itsBCC;
}

inline void
GMAccount::SetBCC
	(
	const JCharacter* bcc
	)
{
	itsBCC	= bcc;
}

/******************************************************************************
 Sig

 ******************************************************************************/

inline const JString&
GMAccount::GetSig()
	const
{
	return itsSig;
}

inline void
GMAccount::SetSig
	(
	const JCharacter* sig
	)
{
	itsSig	= sig;
}

/******************************************************************************
 SigType

 ******************************************************************************/

inline GMAccount::SigType
GMAccount::GetSigType()
	const
{
	return itsSigType;
}

inline void
GMAccount::SetSigType
	(
	const SigType type
	)
{
	itsSigType	= type;
}

/******************************************************************************
 FullName

 ******************************************************************************/

inline const JString&
GMAccount::GetFullName()
	const
{
	return itsFullName;
}

inline void
GMAccount::SetFullName
	(
	const JCharacter* name
	)
{
	itsFullName	= name;
}

/******************************************************************************
 UsingPOP

 ******************************************************************************/

inline JBoolean
GMAccount::IsUsingPOP()
	const
{
	return itsUsePOP;
}

inline void
GMAccount::ShouldUsePOP
	(
	const JBoolean use
	)
{
	itsUsePOP	= use;
}

/******************************************************************************
 SavingPass

 ******************************************************************************/

inline JBoolean
GMAccount::IsSavingPass()
	const
{
	return itsSavePass;
}

inline void
GMAccount::ShouldSavePass
	(
	const JBoolean save
	)
{
	itsSavePass	= save;
}

/******************************************************************************
 Password

 ******************************************************************************/

inline const JString&
GMAccount::GetPassword()
	const
{
	return itsPassword;
}

inline void
GMAccount::SetPassword
	(
	const JCharacter* password
	)
{
	itsPassword	= password;
}

/******************************************************************************
 UsingAPOP

 ******************************************************************************/

inline JBoolean
GMAccount::IsUsingAPOP()
	const
{
	return itsUseAPOP;
}

inline void
GMAccount::ShouldUseAPOP
	(
	const JBoolean use
	)
{
	itsUseAPOP	= use;
}

/******************************************************************************
 AutoChecking

 ******************************************************************************/

inline JBoolean
GMAccount::IsAutoChecking()
	const
{
	return itsAutoCheck;
}

inline void
GMAccount::ShouldAutoCheck
	(
	const JBoolean check
	)
{
	itsAutoCheck	= check;
}

/******************************************************************************
 CheckInterval

 ******************************************************************************/

inline JIndex
GMAccount::GetCheckInterval()
	const
{
	return itsAutoCheckInterval;
}

inline void
GMAccount::SetCheckInterval
	(
	const JIndex interval
	)
{
	itsAutoCheckInterval	= interval;
}

/******************************************************************************
 POPServer

 ******************************************************************************/

inline const JString&
GMAccount::GetPOPServer()
	const
{
	return itsPOPServer;
}

inline void
GMAccount::SetPOPServer
	(
	const JCharacter* server
	)
{
	itsPOPServer	= server;
}

/******************************************************************************
 POPAccount

 ******************************************************************************/

inline const JString&
GMAccount::GetPOPAccount()
	const
{
	return itsPOPAccount;
}

inline void
GMAccount::SetPOPAccount
	(
	const JCharacter* account
	)
{
	itsPOPAccount	= account;
}

#endif
