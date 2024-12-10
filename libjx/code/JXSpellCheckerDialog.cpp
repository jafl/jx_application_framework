/******************************************************************************
 JXSpellCheckerDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JXSpellCheckerDialog.h"
#include "JXSpellChecker.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXInputField.h"
#include "JXStaticText.h"
#include "JXSpellList.h"
#include "JXScrollbarSet.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
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
	JXWindowDirector(editor->GetWindow()->GetDirector()),
	itsChecker(checker),
	itsEditor(editor),
	itsCheckRange(range),
	itsCurrentIndex(range.GetFirst()),
	itsFoundErrorsFlag(false)
{
	itsSuggestionList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll, 25);
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
	JXWindowDirector::Activate();
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
	return JXWindowDirector::Deactivate();
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

	return JXWindowDirector::Close();		// deletes us if successful
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXSpellCheckerDialog::BuildWindow()
{

// begin JXLayout

	auto* window = jnew JXWindow(this, 530,270, JGetString("WindowTitle::JXSpellCheckerDialog::JXLayout"));

	auto* notFoundLabel =
		jnew JXStaticText(JGetString("notFoundLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,15, 110,20);
	notFoundLabel->SetToLabel(false);

	itsCheckText =
		jnew JXStaticText(JString::empty, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 135,15, 280,20);
	itsCheckText->SetToLabel(false);

	auto* changeToLabel =
		jnew JXStaticText(JGetString("changeToLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	changeToLabel->SetToLabel(false);

	itsChangeButton =
		jnew JXTextButton(JGetString("itsChangeButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,60, 75,20);

	itsIgnoreButton =
		jnew JXTextButton(JGetString("itsIgnoreButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 440,60, 75,20);
	itsIgnoreButton->SetShortcuts(JGetString("itsIgnoreButton::shortcuts::JXSpellCheckerDialog::JXLayout"));

	auto* suggestionsLabel =
		jnew JXStaticText(JGetString("suggestionsLabel::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,75, 90,20);
	suggestionsLabel->SetToLabel(false);

	auto* set =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 105,75, 230,180);
	assert( set != nullptr );

	itsSuggestionWidget =
		jnew JXSpellList(set, set->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 230,180);

	itsChangeAllButton =
		jnew JXTextButton(JGetString("itsChangeAllButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,90, 75,20);

	itsLearnButton =
		jnew JXTextButton(JGetString("itsLearnButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,130, 165,20);

	itsLearnCapsButton =
		jnew JXTextButton(JGetString("itsLearnCapsButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 350,160, 165,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::JXSpellCheckerDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 440,235, 75,20);
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::shortcuts::JXSpellCheckerDialog::JXLayout"));

	itsFirstGuess =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 105,45, 230,20);

// end JXLayout

	window->LockCurrentMinSize();

	ListenTo(itsCloseButton);
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
	window->SetTransientFor(itsEditor->GetWindow()->GetDirector());
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
			Close();
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
					itsFirstGuess->GetText()->SetText(*(itsSuggestionList->GetItem(1)));
				}
				itsSuggestionWidget->SetStringList(itsSuggestionList);

				itsEditor->TEScrollToSelection(false);
				Activate();
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
		auto& choice = dynamic_cast<const JXSpellList::WordSelected&>(message);
		itsFirstGuess->GetText()->SetText(choice.GetWord());
		itsFirstGuess->Focus();
		itsIgnoreButton->SetShortcuts(JString::empty);
		itsChangeButton->SetShortcuts(JGetString("DefaultButtonShortcut::JXSpellCheckerDialog"));
	}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWord))
	{
		auto& choice = dynamic_cast<const JXSpellList::ReplaceWord&>(message);
		itsFirstGuess->GetText()->SetText(choice.GetWord());
		Change();
		Check();
	}
	else if (sender == itsSuggestionWidget && message.Is(JXSpellList::kReplaceWordAll))
	{
		auto& choice = dynamic_cast<const JXSpellList::ReplaceWordAll&>(message);
		itsFirstGuess->GetText()->SetText(choice.GetWord());
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

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Close();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
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
	while (!itsEditor->SearchForward(pattern, true, false, &wrapped).IsEmpty() &&
		   itsEditor->GetSelection(&r) && r.first <= itsCheckRange.charRange.last)
	{
		itsEditor->Paste(newWord);

		itsCheckRange.charRange.last -= oldWord.GetCharacterCount();
		itsCheckRange.byteRange.last -= oldWord.GetByteCount();

		itsCheckRange.charRange.last += newWord.GetCharacterCount();
		itsCheckRange.byteRange.last += newWord.GetByteCount();
	}
}
