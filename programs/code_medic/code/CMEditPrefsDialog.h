/******************************************************************************
 CMEditPrefsDialog.h

	Copyright (C) 2001-10 by John Lindal. All rights reserved.

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
					  const JCharacter* gdbCmd,
					  const JCharacter* jdbCmd,
					  const JCharacter* editFileCmd,
					  const JCharacter* editFileLineCmd,
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

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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

	void	BuildWindow(const JCharacter* gdbCmd,
						const JCharacter* jdbCmd,
						const JCharacter* editFileCmd,
						const JCharacter* editFileLineCmd,
						const JPtrArray<JString>& cSourceSuffixes,
						const JPtrArray<JString>& cHeaderSuffixes,
						const JPtrArray<JString>& javaSuffixes,
						const JPtrArray<JString>& phpSuffixes,
						const JPtrArray<JString>& fortranSuffixes);

	void	ChooseDebugger(const JCharacter* name, JXInputField* input);

	// not allowed

	CMEditPrefsDialog(const CMEditPrefsDialog& source);
	const CMEditPrefsDialog& operator=(const CMEditPrefsDialog& source);
};

#endif
