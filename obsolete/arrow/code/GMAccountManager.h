/******************************************************************************
 GMAccountManager.h

	Interface for the GMAccountManager class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMAccountManager
#define _H_GMAccountManager

// Superclass Headers
#include <JPrefObject.h>
#include <JBroadcaster.h>

#include <JPtrArray.h>

class GMAccount;
class GMAccountDialog;
class GPOPRetriever;

class GMAccountManager : public JPrefObject, virtual public JBroadcaster
{
public:

	GMAccountManager();
	virtual ~GMAccountManager();

	void		EditAccounts();
	JBoolean	FinishedChecking() const;

	JSize	GetPOPAccountCount() const;
	void	GetPOPAccounts(JPtrArray<JString>* accounts);
	void	CheckAccount(const JIndex index);
	void	CheckAccount(GPOPRetriever* r);
	void	CheckAllAccounts();

	JBoolean	GetPOPAccount(const JCharacter* accountName, GMAccount** account);
	JBoolean	GetReplyAccount(const JCharacter* to, GMAccount** account);
	JBoolean	GetAccountWithNickname(const JCharacter* nickname, GMAccount** account);
	GMAccount*	GetFirstAccount();
	GMAccount*	GetDefaultAccount();

	JSize		GetAccountCount() const;
	GMAccount*	GetAccount(const JIndex index);
	void		GetAccountNames(JPtrArray<JString>* accounts);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	JBoolean			itsFirstTime;
	GMAccountDialog*	itsDialog;

	JPtrArray<GMAccount>*		itsAccounts;
	JPtrArray<GPOPRetriever>*	itsRetrievers;
	JPtrArray<GPOPRetriever>*	itsPendingRetrievers;

	JIndex						itsDefaultIndex;

private:

	void	GenerateDefaultAccount();
	void	GeneratePOPRetreivers();
	void	GetEditedAccounts();
	void	CheckNextAccount();

	// not allowed

	GMAccountManager(const GMAccountManager& source);
	const GMAccountManager& operator=(const GMAccountManager& source);

public:

	static const JCharacter* kAccountsChanged;

	class AccountsChanged : public JBroadcaster::Message
		{
		public:

			AccountsChanged()
				:
				JBroadcaster::Message(kAccountsChanged)
				{ };
		};

};
#endif
