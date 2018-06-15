/******************************************************************************
 JXSpellCheckerDialog.h

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXSpellCheckerDialog
#define _H_JXSpellCheckerDialog

#include "JXDialogDirector.h"
#include <JStyledText.h>

class JXTextButton;
class JXTEBase;
class JXStaticText;
class JXInputField;
class JXSpellChecker;
class JXSpellList;

class JXSpellCheckerDialog : public JXDialogDirector
{
public:

	JXSpellCheckerDialog(JXSpellChecker* checker,
						 JXTEBase* editor, const JStyledText::TextRange& range);

	virtual ~JXSpellCheckerDialog();

	virtual void		Activate() override;
	virtual JBoolean	Deactivate() override;
	virtual JBoolean	Close() override;

	void	Check();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXSpellChecker*			itsChecker;
	JXTEBase*				itsEditor;				// not owned
	JStyledText::TextRange	itsCheckRange;
	JStyledText::TextIndex	itsCurrentIndex;
	JBoolean				itsFoundErrorsFlag;

	JXSpellList*		itsSuggestionWidget;
	JPtrArray<JString>*	itsSuggestionList;

// begin JXLayout

	JXStaticText* itsCheckText;
	JXInputField* itsFirstGuess;
	JXTextButton* itsIgnoreButton;
	JXTextButton* itsChangeButton;
	JXTextButton* itsLearnButton;
	JXTextButton* itsChangeAllButton;
	JXTextButton* itsCloseButton;
	JXTextButton* itsLearnCapsButton;

// end JXLayout

private:

	void	BuildWindow();
	void	Change();
	void	ChangeAll();

	// not allowed

	JXSpellCheckerDialog(const JXSpellCheckerDialog& source);
	const JXSpellCheckerDialog& operator=(const JXSpellCheckerDialog& source);
};

#endif
