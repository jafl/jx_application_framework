/******************************************************************************
 CBEditStylerDialog.h

	Interface for the CBEditStylerDialog class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditStylerDialog
#define _H_CBEditStylerDialog

#include <JXDialogDirector.h>
#include "CBStylerBase.h"		// need definition of WordStyle

class JXTextButton;
class JXTextCheckbox;
class CBStylerTable;

class CBEditStylerDialog : public JXDialogDirector
{
public:

	CBEditStylerDialog(const JString& windowTitle, const JBoolean active,
					   const JUtf8Byte** typeNames,
					   const JArray<JFontStyle>& typeStyles,
					   const JArray<CBStylerBase::WordStyle>& wordList,
					   const CBTextFileType fileType);

	virtual ~CBEditStylerDialog();

	void	GetData(JBoolean* active, JArray<JFontStyle>* typeStyles,
					JStringMap<JFontStyle>* wordStyles) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBStylerTable*	itsTypeTable;
	CBStylerTable*	itsWordTable;

// begin JXLayout

	JXTextCheckbox* itsActiveCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle, const JBoolean active,
						const JUtf8Byte** typeNames,
						const JArray<JFontStyle>& typeStyles,
						const JArray<CBStylerBase::WordStyle>& wordList,
						const CBTextFileType fileType);

	// not allowed

	CBEditStylerDialog(const CBEditStylerDialog& source);
	const CBEditStylerDialog& operator=(const CBEditStylerDialog& source);
};

#endif
