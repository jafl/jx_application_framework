/******************************************************************************
 CMEditPrefsDialog.h

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMEditPrefsDialog
#define _H_CMEditPrefsDialog

#include <JXDialogDirector.h>
#include <JPtrArray-JString.h>

class JXInputField;
class JXTextButton;
class CMDebuggerProgramInput;

class CMEditPrefsDialog : public JXDialogDirector
{
public:

	CMEditPrefsDialog(JXDirector* supervisor,
					  const JString& gdbCmd,
					  const JString& jdbCmd,
					  const JString& editFileCmd,
					  const JString& editFileLineCmd,
					  const JPtrArray<JString>& cSourceSuffixes,
					  const JPtrArray<JString>& cHeaderSuffixes,
					  const JPtrArray<JString>& javaSuffixes,
					  const JPtrArray<JString>& phpSuffixes,
					  const JPtrArray<JString>& fortranSuffixes);

	virtual ~CMEditPrefsDialog();

	void	GetPrefs(JString* gdbCmd, JString* jdbCmd,
					 JString* editFileCmd, JString* editFileLineCmd,
					 JPtrArray<JString>* cSourceSuffixes,
					 JPtrArray<JString>* cHeaderSuffixes,
					 JPtrArray<JString>* javaSuffixes,
					 JPtrArray<JString>* phpSuffixes,
					 JPtrArray<JString>* fortranSuffixes) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	CMDebuggerProgramInput* itsGDBCmd;
	CMDebuggerProgramInput* itsJDBCmd;
	JXInputField*           itsEditFileCmdInput;
	JXInputField*           itsEditFileLineCmdInput;
	JXInputField*           itsCSourceSuffixInput;
	JXInputField*           itsCHeaderSuffixInput;
	JXInputField*           itsJavaSuffixInput;
	JXTextButton*           itsChooseGDBButton;
	JXTextButton*           itsChooseJDBButton;
	JXInputField*           itsPHPSuffixInput;
	JXInputField*           itsFortranSuffixInput;

// end JXLayout

private:

	void	BuildWindow(const JString& gdbCmd,
						const JString& jdbCmd,
						const JString& editFileCmd,
						const JString& editFileLineCmd,
						const JPtrArray<JString>& cSourceSuffixes,
						const JPtrArray<JString>& cHeaderSuffixes,
						const JPtrArray<JString>& javaSuffixes,
						const JPtrArray<JString>& phpSuffixes,
						const JPtrArray<JString>& fortranSuffixes);

	void	ChooseDebugger(const JString& name, JXInputField* input);

	// not allowed

	CMEditPrefsDialog(const CMEditPrefsDialog& source);
	const CMEditPrefsDialog& operator=(const CMEditPrefsDialog& source);
};

#endif
