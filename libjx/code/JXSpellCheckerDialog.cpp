/******************************************************************************
 JXSpellCheckerDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/jx/JXSpellCheckerDialog.h"
#include "jx-af/jx/JXSpellChecker.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextButton.h"
#include "jx-af/jx/JXInputField.h"
#include "jx-af/jx/JXStaticText.h"
#include "jx-af/jx/JXSpellList.h"
#include "jx-af/jx/JXScrollbarSet.h"
#include "jx-af/jx/JXColorManager.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JUserNotification.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSpellCheckerDialog::JXSpellCheckerDialog
	(
	JXSpellChecker*					checker,
	JXTEBase*						editor,
	const JStyledText::TextRange&	range
	)
	:
	JXDialogDirector(editor->GetWindow()->GetDirector(), true),
	itsChecker(checker),
	itsEditor(editor),
	itsCheckRange(range),
	itsCurrentIndex(range.GetFirst()),
	itsFoundErrorsFlag(false)
{
	itsSuggestionList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, 100);
	assert( itsSuggestionList != nullptr );

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

bool
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

bool
JXSpellCheckerDialog::Close()
{
	itsEditor->SetCaretLocation(itsCheckRange.charRange.first);

	if (!itsFoundErrorsFlag && itsChecker->WillReportNoErrors())
	{
		JGetUserNotification()->DisplayMessage(JGetString("NoErrors::JXSpellCheckerDialog"));
	}

	return JXDialogDirector::Close();		// deletes us if successful
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::BuildWindow()
{

// begin JXLayout

	const auto* window = jnew JXWindow(this, 530,270, JString::empty);
	assert( window != nullptr );

	const auto* notFoundLabel =
		jnew JXStaticText(JGetString("notFoundLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,15, 110,20);
	assert( notFoundLabel != nullptr );
	notFoundLabel->SetToLabel();

	itsCheckText =
		jnew JXStaticText(JGetString("itsCheckText::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 135,15, 280,20);
	assert( itsCheckText != nullptr );
	itsCheckText->SetToLabel();

	const auto* changeToLabel =
		jnew JXStaticText(JGetString("changeToLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	assert( changeToLabel != nullptr );
	changeToLabel->SetToLabel();

	const auto* suggestionsLabel =
		jnew JXStaticText(JGetString("suggestionsLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,75, 90,20);
	assert( suggestionsLabel != nullptr );
	suggestionsLabel->SetToLabel();

	itsFirstGuess =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 105,45, 230,20);
	assert( itsFirstGuess != nullptr );

	const auto* set =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 105,75, 230,180);
	assert( set != nullptr );

	itsIgnoreButton =
		jnew JXTextButton(JGetString("itsIgnoreButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 75,20);
	assert( itsIgnoreButton != nullptr );
	itsIgnoreButton->SetShortcuts(JGetString("itsIgnoreButton::JXSpellCheckerDialog::shortcuts::JXLayout"));

	itsChangeButton =
		jnew JXTextButton(JGetString("itsChangeButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,60, 75,20);
	assert( itsChangeButton != nullptr );

	itsLearnButton =
		jnew JXTextButton(JGetString("itsLearnButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,130, 165,20);
	assert( itsLearnButton != nullptr );

	itsChangeAllButton =
		jnew JXTextButton(JGetString("itsChangeAllButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,90, 75,20);
	assert( itsChangeAllButton != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 440,235, 75,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::JXSpellCheckerDialog::shortcuts::JXLayout"));

	itsLearnCapsButton =
		jnew JXTextButton(JGetString("itsLearnCapsButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,160, 165,20);
	assert( itsLearnCapsButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXSpellCheckerDialog"));
	window->LockCurrentMinSize();

	SetButtons(itsCloseButton, nullptr);

	itsSuggestionWidget =
		jnew JXSpellList(set, set->GetScrollEnclosure(),
						JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 230,180);
	assert( itsSuggestionWidget != nullptr );
	ListenTo(itsSuggestionWidget);

	ListenTo(itsIgnoreButton);
	ListenTo(itsChangeButton);
	ListenTo(itsLearnButton);
	ListenTo(itsLearnCapsButton);
	ListenTo(itsChangeAllButton);

	itsCheckText->SetCurrentFontBold(true);
	itsCheckText->GetText()->SetDefaultFontStyle(JFontStyle(true, false, 0, false));

	itsFirstGuess->SetBackColor(itsFirstGuess->GetFocusColor());
	ListenTo(itsFirstGuess);

	// place intelligently
	// (outside left, right, bottom, top; inside top, bottom, left, right)

	UseModalPlacement(false);

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
	bool keepGoing = true;
	while (keepGoing)
	{
		const JStyledText::TextIndex end   = itsEditor->GetText()->GetWordEnd(itsCurrentIndex);
		const JStyledText::TextIndex start = itsEditor->GetText()->GetWordStart(end);
		if (end.charIndex < start.charIndex ||
			!itsEditor->GetText()->GetText().CharacterIndexValid(itsCurrentIndex.charIndex) ||
			itsCheckRange.charRange.last < start.charIndex)
		{
			if (IsActive())
			{
				EndDialog(true);
			}
			else
			{
				Close();
			}
			return;
		}
		else
		{
			const JStyledText::TextIndex beyondEnd =
				itsEditor->GetText()->AdjustTextIndex(end, +1);

			itsEditor->SetSelection(JStyledText::TextRange(start, beyondEnd));

			JString word;
			const bool selected = itsEditor->GetSelection(&word);
			assert(selected);

			bool goodFirstSuggestion;
			keepGoing       = itsChecker->CheckWord(word, itsSuggestionList, &goodFirstSuggestion);
			itsCurrentIndex = beyondEnd;

			if (!keepGoing)
			{
				itsFoundErrorsFlag = true;
				itsCheckText->GetText()->SetText(word);

				if (itsSuggestionList->IsEmpty())
				{
					itsFirstGuess->GetText()->SetText(JString::empty);
				}
				else if (goodFirstSuggestion)
				{
					itsFirstGuess->GetText()->SetText(*(itsSuggestionList->GetElement(1)));
				}
				itsSuggestionWidget->SetStringList(itsSuggestionList);

				itsEditor->TEScrollToSelection(false);
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
		const auto* choice =
			dynamic_cast<const JXSpellList::WordSelected*>(&message);
		assert( choice != nullptr );

		itsFirstGuess->GetText()->SetText(choice->GetWord());
		itsFirstGuess->Focus();
		itsIgnoreButton->SetShortcuts(JString::empty);
		itsChangeButton->SetShortcuts(JGetString("DefaultButtonShortcut::JXSpellCheckerDialog"));
	}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWord))
	{
		const auto* choice =
			dynamic_cast<const JXSpellList::ReplaceWord*>(&message);
		assert( choice != nullptr );

		itsFirstGuess->GetText()->SetText(choice->GetWord());
		Change();
		Check();
	}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWordAll))
	{
		const auto* choice =
			dynamic_cast<const JXSpellList::ReplaceWordAll*>(&message);
		assert( choice != nullptr );

		itsFirstGuess->GetText()->SetText(choice->GetWord());
		ChangeAll();
		Check();
	}

	else if (sender == itsFirstGuess &&
			 (message.Is(JStyledText::kTextChanged) ||
			  message.Is(JStyledText::kTextSet)))
	{
		if (itsFirstGuess->GetText()->IsEmpty())
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
		itsChecker->Ignore(itsCheckText->GetText()->GetText());
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
		itsChecker->Learn(itsCheckText->GetText()->GetText());
		Check();
	}
	else if (sender == itsLearnCapsButton && message.Is(JXButton::kPushed))
	{
		itsChecker->LearnCaps(itsCheckText->GetText()->GetText());
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
	itsEditor->Paste(itsFirstGuess->GetText()->GetText());
	itsCurrentIndex = itsEditor->GetInsertionIndex();

	itsCheckRange.charRange.last -= itsCheckText->GetText()->GetText().GetCharacterCount();
	itsCheckRange.byteRange.last -= itsCheckText->GetText()->GetText().GetByteCount();

	itsCheckRange.charRange.last += itsFirstGuess->GetText()->GetText().GetCharacterCount();
	itsCheckRange.byteRange.last += itsFirstGuess->GetText()->GetText().GetByteCount();
}

/******************************************************************************
 ChangeAll (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::ChangeAll()
{
	Change();

	const JString& oldWord = itsCheckText->GetText()->GetText();
	const JString& newWord = itsFirstGuess->GetText()->GetText();

	JRegex pattern(JRegex::BackslashForLiteral(oldWord));

	bool wrapped;
	JCharacterRange r;
	while (!itsEditor->JTextEditor::SearchForward(pattern, true, false, &wrapped).IsEmpty() &&
		   itsEditor->GetSelection(&r) && r.first <= itsCheckRange.charRange.last)
	{
		itsEditor->Paste(newWord);

		itsCheckRange.charRange.last -= oldWord.GetCharacterCount();
		itsCheckRange.byteRange.last -= oldWord.GetByteCount();

		itsCheckRange.charRange.last += newWord.GetCharacterCount();
		itsCheckRange.byteRange.last += newWord.GetByteCount();
	}
}
