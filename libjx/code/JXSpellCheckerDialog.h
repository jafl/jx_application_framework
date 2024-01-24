/******************************************************************************
 JXSpellCheckerDialog.h

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXSpellCheckerDialog
#define _H_JXSpellCheckerDialog

#include "JXModalDialogDirector.h"
#include <jx-af/jcore/JStyledText.h>

class JXTextButton;
class JXTEBase;
class JXStaticText;
class JXInputField;
class JXSpellChecker;
class JXSpellList;

class JXSpellCheckerDialog : public JXWindowDirector
{
public:

	JXSpellCheckerDialog(JXSpellChecker* checker,
						 JXTEBase* editor, const JStyledText::TextRange& range);

	~JXSpellCheckerDialog() override;

	void	Activate() override;
	bool	Deactivate() override;
	bool	Close() override;

	void	Check();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXSpellChecker*			itsChecker;
	JXTEBase*				itsEditor;				// not owned
	JStyledText::TextRange	itsCheckRange;
	JStyledText::TextIndex	itsCurrentIndex;
	bool					itsFoundErrorsFlag;

	JPtrArray<JString>*	itsSuggestionList;

// begin JXLayout

	JXStaticText* itsCheckText;
	JXTextButton* itsChangeButton;
	JXTextButton* itsIgnoreButton;
	JXSpellList*  itsSuggestionWidget;
	JXTextButton* itsChangeAllButton;
	JXTextButton* itsLearnButton;
	JXTextButton* itsLearnCapsButton;
	JXTextButton* itsCloseButton;
	JXInputField* itsFirstGuess;

// end JXLayout

private:

	void	BuildWindow();
	void	Change();
	void	ChangeAll();
};

#endif
