/******************************************************************************
 JXSpellCheckerDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <JXSpellCheckerDialog.h>
#include <JXSpellChecker.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXSpellList.h>
#include <JXScrollbarSet.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <JUserNotification.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSpellCheckerDialog::JXSpellCheckerDialog
	(
	JXSpellChecker*		checker,
	JXTEBase*			editor,
	const JIndexRange&	range
	)
	:
	JXDialogDirector(editor->GetWindow()->GetDirector(), kJTrue),
	itsChecker(checker),
	itsEditor(editor),
	itsCheckRange(range),
	itsCurrentIndex(range.first),
	itsFoundErrorsFlag(kJFalse)
{
	itsSuggestionList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, 100);
	assert( itsSuggestionList != NULL );

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSpellCheckerDialog::~JXSpellCheckerDialog()
{
	jdelete itsSuggestionList;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXSpellCheckerDialog::Activate()
{
	JXDialogDirector::Activate();
	itsEditor->TEActivate();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
JXSpellCheckerDialog::Deactivate()
{
	if (IsActive())
		{
		JXWindow* w = GetWindow();
		itsChecker->SaveWindowSize(w->GetFrameGlobal());
		}

	itsEditor->TEDeactivate();
	return JXDialogDirector::Deactivate();
}

/******************************************************************************
 Close (virtual)

	We will close successfully, and we cannot access our member variables
	after calling inherited, because it deletes us, so we do the clean up first.

 ******************************************************************************/

JBoolean
JXSpellCheckerDialog::Close()
{
	itsEditor->SetCaretLocation(itsCheckRange.first);

	if (!itsFoundErrorsFlag && itsChecker->WillReportNoErrors())
		{
		(JGetUserNotification())->DisplayMessage(JGetString(kNoErrorsID));
		}

	return JXDialogDirector::Close();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::BuildWindow()
{

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 530,270, JString::empty);
	assert( window != NULL );

	JXStaticText* notFoundLabel =
		jnew JXStaticText(JGetString("notFoundLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,15, 110,20);
	assert( notFoundLabel != NULL );
	notFoundLabel->SetToLabel();

	itsCheckText =
		jnew JXStaticText(JGetString("itsCheckText::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 135,15, 280,20);
	assert( itsCheckText != NULL );
	itsCheckText->SetToLabel();

	JXStaticText* changeToLabel =
		jnew JXStaticText(JGetString("changeToLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	assert( changeToLabel != NULL );
	changeToLabel->SetToLabel();

	JXStaticText* suggestionsLabel =
		jnew JXStaticText(JGetString("suggestionsLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,75, 90,20);
	assert( suggestionsLabel != NULL );
	suggestionsLabel->SetToLabel();

	itsFirstGuess =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 105,45, 230,20);
	assert( itsFirstGuess != NULL );

	JXScrollbarSet* set =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 105,75, 230,180);
	assert( set != NULL );

	itsIgnoreButton =
		jnew JXTextButton(JGetString("itsIgnoreButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 75,20);
	assert( itsIgnoreButton != NULL );
	itsIgnoreButton->SetShortcuts(JGetString("itsIgnoreButton::JXSpellCheckerDialog::shortcuts::JXLayout"));

	itsChangeButton =
		jnew JXTextButton(JGetString("itsChangeButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,60, 75,20);
	assert( itsChangeButton != NULL );

	itsLearnButton =
		jnew JXTextButton(JGetString("itsLearnButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,130, 165,20);
	assert( itsLearnButton != NULL );

	itsChangeAllButton =
		jnew JXTextButton(JGetString("itsChangeAllButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,90, 75,20);
	assert( itsChangeAllButton != NULL );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 440,235, 75,20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXSpellCheckerDialog::shortcuts::JXLayout"));

	itsLearnCapsButton =
		jnew JXTextButton(JGetString("itsLearnCapsButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,160, 165,20);
	assert( itsLearnCapsButton != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXSpellCheckerDialog"));
	window->LockCurrentMinSize();

	SetButtons(itsCloseButton, NULL);

	itsSuggestionWidget =
		jnew JXSpellList(set, set->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 230,180);
	assert( itsSuggestionWidget != NULL );
	ListenTo(itsSuggestionWidget);

	ListenTo(itsIgnoreButton);
	ListenTo(itsChangeButton);
	ListenTo(itsLearnButton);
	ListenTo(itsLearnCapsButton);
	ListenTo(itsChangeAllButton);

	itsCheckText->SetCurrentFontBold(kJTrue);
	itsCheckText->SetDefaultFontStyle(JFontStyle(kJTrue, kJFalse, 0, kJFalse));

	itsFirstGuess->SetBackColor(itsFirstGuess->GetFocusColor());
	itsFirstGuess->ShouldBroadcastAllTextChanged(kJTrue);
	ListenTo(itsFirstGuess);

	// place intelligently
	// (outside left, right, bottom, top; inside top, bottom, left, right)

	UseModalPlacement(kJFalse);

	const JPoint& pt = itsChecker->GetWindowSize();
	if (pt.x > 0 && pt.y > 0)
		{
		window->SetSize(pt.x, pt.y);
		}

	const JRect wFrame   = window->GetFrameGlobal();
	const JCoordinate ww = wFrame.width();
	const JCoordinate wh = wFrame.height();

	const JRect rootBounds = GetDisplay()->GetBounds();

	JXWindow* w        = itsEditor->GetWindow();
	const JRect eFrame = w->GlobalToRoot(w->GetFrameGlobal());

	JCoordinate x = eFrame.left + (eFrame.width() - ww) / 2;
	JCoordinate y = eFrame.top + (eFrame.height() - wh) / 2;

	if (eFrame.right + ww <= rootBounds.right)
		{
		x = eFrame.right;
		}
	else if (eFrame.left - ww >= rootBounds.left)
		{
		x = eFrame.left - ww;
		}
	else if (eFrame.bottom + wh <= rootBounds.bottom)
		{
		y = eFrame.bottom;
		}
	else if (eFrame.top - wh >= rootBounds.top)
		{
		y = eFrame.top - wh;
		}
	else
		{
		x = eFrame.right;
		}

	// try to keep entire window visible

	if (x + ww > rootBounds.right)
		{
		x = rootBounds.right - ww;
		}
	x = JMax(rootBounds.left, x);

	if (y + wh > rootBounds.bottom)
		{
		y = rootBounds.bottom - wh;
		}
	y = JMax(rootBounds.top, y);

	window->Place(x, y);
}

/******************************************************************************
 Check

 ******************************************************************************/

void
JXSpellCheckerDialog::Check()
{
	JBoolean keepGoing = kJTrue;
	while (keepGoing)
		{
		const JIndex end   = itsEditor->GetWordEnd(itsCurrentIndex);
		const JIndex start = itsEditor->GetWordStart(end);
		if (end < start ||
			!itsEditor->IndexValid(itsCurrentIndex) ||
			itsCheckRange.last < start)
			{
			if (IsActive())
				{
				EndDialog(kJTrue);
				}
			else
				{
				Close();
				}
			return;
			}
		else
			{
			itsEditor->SetSelection(start, end);

			JString word;
			const JBoolean selected = itsEditor->GetSelection(&word);
			assert(selected);

			JBoolean goodFirstSuggestion;
			keepGoing       = itsChecker->CheckWord(word, itsSuggestionList, &goodFirstSuggestion);
			itsCurrentIndex = end + 1;

			if (!keepGoing)
				{
				itsFoundErrorsFlag = kJTrue;
				itsCheckText->SetText(word);

				if (itsSuggestionList->IsEmpty())
					{
					itsFirstGuess->SetText(JString::empty);
					}
				else if (goodFirstSuggestion)
					{
					itsFirstGuess->SetText(*(itsSuggestionList->GetElement(1)));
					}
				itsSuggestionWidget->SetStringList(itsSuggestionList);

				itsEditor->TEScrollToSelection(kJFalse);
				BeginDialog();
				}
			}
		}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
JXSpellCheckerDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSuggestionWidget && message.Is(JXSpellList::kWordSelected))
		{
		const JXSpellList::WordSelected* choice =
			dynamic_cast<const JXSpellList::WordSelected*>(&message);
		assert( choice != NULL );

		itsFirstGuess->SetText(choice->GetWord());
		itsFirstGuess->Focus();
		itsIgnoreButton->SetShortcuts(JString::empty);
		itsChangeButton->SetShortcuts(JGetString("DefaultButtonShortcut::JXSpellCheckerDialog"));
		}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWord))
		{
		const JXSpellList::ReplaceWord* choice =
			dynamic_cast<const JXSpellList::ReplaceWord*>(&message);
		assert( choice != NULL );

		itsFirstGuess->SetText(choice->GetWord());
		Change();
		Check();
		}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWordAll))
		{
		const JXSpellList::ReplaceWordAll* choice =
			dynamic_cast<const JXSpellList::ReplaceWordAll*>(&message);
		assert( choice != NULL );

		itsFirstGuess->SetText(choice->GetWord());
		ChangeAll();
		Check();
		}

	else if (sender == itsFirstGuess &&
			 (message.Is(JTextEditor::kTextChanged) ||
			  message.Is(JTextEditor::kTextSet)))
		{
		if (itsFirstGuess->IsEmpty())
			{
			itsIgnoreButton->SetShortcuts(JGetString("DefaultButtonShortcut::JXSpellCheckerDialog"));
			itsChangeButton->SetShortcuts(JString::empty);
			itsChangeButton->Deactivate();
			itsChangeAllButton->Deactivate();
			}
		else
			{
			itsIgnoreButton->SetShortcuts(JString::empty);
			itsChangeButton->SetShortcuts(JGetString("DefaultButtonShortcut::JXSpellCheckerDialog"));
			itsChangeButton->Activate();
			itsChangeAllButton->Activate();
			}
		}

	else if (sender == itsIgnoreButton && message.Is(JXButton::kPushed))
		{
		itsChecker->Ignore(itsCheckText->GetText());
		Check();
		}
	else if (sender == itsChangeButton && message.Is(JXButton::kPushed))
		{
		Change();
		Check();
		}
	else if (sender == itsChangeAllButton && message.Is(JXButton::kPushed))
		{
		ChangeAll();
		Check();
		}
	else if (sender == itsLearnButton && message.Is(JXButton::kPushed))
		{
		itsChecker->Learn(itsCheckText->GetText());
		Check();
		}
	else if (sender == itsLearnCapsButton && message.Is(JXButton::kPushed))
		{
		itsChecker->LearnCaps(itsCheckText->GetText());
		Check();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Change (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::Change()
{
	itsEditor->Paste(itsFirstGuess->GetText());
	itsCurrentIndex     = itsEditor->GetInsertionIndex();
	itsCheckRange.last -= itsCheckText->GetTextLength();
	itsCheckRange.last += itsFirstGuess->GetTextLength();
}

/******************************************************************************
 ChangeAll (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::ChangeAll()
{
	Change();

	const JString& oldWord = itsCheckText->GetText();
	const JString& newWord = itsFirstGuess->GetText();

	JBoolean wrapped;
	JCharacterRange r;
	while (itsEditor->JTextEditor::SearchForward(oldWord, kJTrue, kJTrue, kJFalse, &wrapped) &&
		   itsEditor->GetSelection(&r) && r.first <= itsCheckRange.last)
		{
		itsEditor->Paste(newWord);
		itsCheckRange.last -= oldWord.GetCharacterCount();
		itsCheckRange.last += newWord.GetCharacterCount();
		}
}
