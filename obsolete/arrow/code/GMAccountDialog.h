/******************************************************************************
 GMAccountDialog.h

	Interface for the GMAccountDialog class

	Copyright @ 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMAccountDialog
#define _H_GMAccountDialog

#include <JXDialogDirector.h>
#include <JPtrArray.h>


class GMAccountList;
class GMAccount;

class JXCardFile;
class JXFileInput;
class JXIntegerInput;
class JXInputField;
class JXPathInput;
class JXRadioGroup;
class JXTextCheckbox;
class JXTextButton;
class JXTextMenu;

class GMAccountDialog : public JXDialogDirector
{
public:

	GMAccountDialog(JXDirector* supervisor, JPtrArray<GMAccount>& array,
					const JIndex defIndex);

	virtual ~GMAccountDialog();

	JBoolean	OKToSwitchAccounts();
	JIndex		GetDefaultIndex() const;

	JPtrArray<GMAccount>&	GetAccountInfoArray();

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToDeactivate();

private:

	JPtrArray<GMAccount>*	itsAccountInfo;

// begin JXLayout

	JXInputField* itsFullNameInput;
	JXInputField* itsSMTPUserName;
	JXInputField* itsSMTPServerInput;
	JXInputField* itsReplyToAddress;
	JXInputField* itsCCInput;
	JXInputField* itsBCCInput;
	JXInputField* itsSigInput;
	JXTextButton* itsCancelButton;
	JXTextButton* itsNewButton;
	JXTextButton* itsDeleteButton;
	JXRadioGroup* itsSigType;
	JXTextButton* itsDupButton;
	JXTextButton* itsDefButton;
	JXTextButton* itsHelpButton;
	JXCardFile*   itsAccountTypeCardFile;
	JXTextMenu*   itsAccountTypeMenu;

// end JXLayout

// begin localLayout

	JXFileInput*  itsLocalDefaultInboxInput;
	JXTextButton* itsChooseLocalInboxButton;
	JXPathInput*  itsLocalPathInput;
	JXTextButton* itsChooseLocalPathButton;

// end localLayout

// begin popLayout

	JXIntegerInput* itsDeleteDaysInput;
	JXIntegerInput* itsAutoCheckMinutesInput;
	JXInputField*   itsPopServerInput;
	JXInputField*   itsPopAccountInput;
	JXFileInput*    itsDefaultInboxInput;
	JXTextButton*   itsChooseInboxButton;
	JXTextCheckbox* itsLeaveMailCB;
	JXTextCheckbox* itsSavePasswdCB;
	JXTextCheckbox* itsUseAPOPCB;
	JXTextCheckbox* itsAutoCheckCB;
	JXTextCheckbox* itsDeleteFromServerCB;
	JXPathInput*    itsAccountFolderInput;
	JXTextButton*   itsChoosePathButton;

// end popLayout

    GMAccountList*	itsAccountList;
    JIndex			itsDefaultIndex;
    JIndex			itsCurrentType;

private:

	void		BuildWindow();
	void		AdjustWidgets();
	void		SetCurrentAccount();
	void		NewAccount(GMAccount* src = NULL);
	JBoolean	CreateMailDir(const JCharacter* path);
	void		CreateInboxLink(const JCharacter* path, const JCharacter* inboxStr, const JBoolean system);
	void		CreateItemsIfNeeded(const JCharacter* path);

	// not allowed

	GMAccountDialog(const GMAccountDialog& source);
	const GMAccountDialog& operator=(const GMAccountDialog& source);
};


/******************************************************************************
 GetAccountInfoArray (public)

 ******************************************************************************/

inline JPtrArray<GMAccount>&
GMAccountDialog::GetAccountInfoArray()
{
	return *itsAccountInfo;
}

/******************************************************************************
 GetDefaultIndex (public)

 ******************************************************************************/

inline JIndex
GMAccountDialog::GetDefaultIndex()
	const
{
	return itsDefaultIndex;
}

#endif
