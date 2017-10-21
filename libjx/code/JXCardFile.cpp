/******************************************************************************
 JXCardFile.cpp

	UI independent base class for a set of equally sized cards, only one
	of which can be displayed at a time.  Essentially, this acts like a
	stack of 3x5 cards that one holds in one's hand.

	Derived or layered classes are responsible for providing a way to
	switch between cards.  Examples are menus, tables, or a row of tabs
	across the top.

	BASE CLASS = JXWidgetSet

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXCardFile.h>
#include <JXWindow.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JXCardFile::kCardIndexChanged = "CardIndexChanged::JXCardFile";
const JCharacter* JXCardFile::kCardIndexUpdated = "CardIndexUpdated::JXCardFile";
const JCharacter* JXCardFile::kCardRemoved      = "CardRemoved::JXCardFile";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCardFile::JXCardFile
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsCards = jnew JPtrArray<JXContainer>(JPtrArrayT::kForgetAll);
	assert( itsCards != NULL );

	itsCurrCardIndex = 0;

	SetNeedsInternalFTC();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCardFile::~JXCardFile()
{
	jdelete itsCards;	// objects owned by JXContainer
}

/******************************************************************************
 InsertCard

	The first version creates a new card, inserts it at the specified index,
	and returns it.

	The second version lets you provide the card, so that you can use
	a derived class to add intelligence.  The card's enclosure must be
	the JXCardFile.  We own the card after you give it to us.

 ******************************************************************************/

JXWidgetSet*
JXCardFile::InsertCard
	(
	const JIndex index
	)
{
	JXWidgetSet* card = jnew JXWidgetSet(this, kHElastic, kVElastic, 0,0, 10,10);
	assert( card != NULL );
	InsertCard(index, card);
	return card;
}

void
JXCardFile::InsertCard
	(
	const JIndex index,
	JXWidgetSet* card
	)
{
	assert( const_cast<JXContainer*>(card->GetEnclosure()) == this );

	card->SetSizing(kHElastic, kVElastic);
	card->FitToEnclosure();
	card->Hide();
	card->SetNeedsInternalFTC();

	itsCards->InsertAtIndex(index, card);
	if (index <= itsCurrCardIndex)
		{
		itsCurrCardIndex++;
		Broadcast(CardIndexUpdated(itsCurrCardIndex));
		}
}

/******************************************************************************
 RemoveCard

	Removes the specified card and returns a pointer to it so you can
	keep track of it.

 ******************************************************************************/

JXWidgetSet*
JXCardFile::RemoveCard
	(
	const JIndex index
	)
{
	JXWidgetSet* card = dynamic_cast<JXWidgetSet*>(itsCards->NthElement(index));
	assert( card != NULL );

	itsCards->RemoveElement(index);
	Broadcast(CardRemoved(index));

	if (index < itsCurrCardIndex)
		{
		itsCurrCardIndex--;
		Broadcast(CardIndexUpdated(itsCurrCardIndex));
		}
	else if (index == itsCurrCardIndex)
		{
		itsCurrCardIndex = 0;
		Broadcast(CardIndexUpdated(itsCurrCardIndex));
		}

	return card;
}

/******************************************************************************
 KillFocusOnCurrentCard

	If one of the widgets on the current card has focus, we call KillFocus().
	This is useful when you want to discard the current card without saving
	its contents.

 ******************************************************************************/

void
JXCardFile::KillFocusOnCurrentCard()
{
	if (itsCurrCardIndex > 0)
		{
		JXWindow* window      = GetWindow();
		JXContainer* currCard = itsCards->NthElement(itsCurrCardIndex);

		JXWidget* widget;
		if (window->GetFocusWidget(&widget) &&
			currCard->IsAncestor(widget))
			{
			window->KillFocus();
			}
		}
}

/******************************************************************************
 ShowCard

	If the current card is willing to disappear, then we show the requested
	one and return kJTrue.  Otherwise, we return kJFalse.

 ******************************************************************************/

JBoolean
JXCardFile::ShowCard
	(
	JXWidgetSet* card
	)
{
	JIndex index;
	if (itsCards->Find(card, &index))
		{
		return ShowCard(index);
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JXCardFile::ShowCard
	(
	const JIndex index
	)
{
	if (itsCurrCardIndex == index)
		{
		return kJTrue;
		}

	JXWindow* window = GetWindow();

	// If a card is visible and a widget on this card has focus,
	// then make sure that it is willing to give up focus.

	JBoolean hadFocus = kJFalse;
	if (itsCurrCardIndex > 0)
		{
		JXContainer* currCard = itsCards->NthElement(itsCurrCardIndex);

		JXWidget* widget;
		if (window->GetFocusWidget(&widget) &&
			currCard->IsAncestor(widget))
			{
			if (widget->OKToUnfocus())
				{
				hadFocus = kJTrue;
				}
			else
				{
				return kJFalse;
				}
			}

		currCard->Hide();
		}

	// Show the requested card and focus to the first widget on it.

	itsCurrCardIndex      = index;
	JXContainer* currCard = itsCards->NthElement(itsCurrCardIndex);

	currCard->Show();
	if (hadFocus)
		{
		window->SwitchFocusToFirstWidgetWithAncestor(currCard);
		}

	Broadcast(CardIndexChanged(itsCurrCardIndex));
	return kJTrue;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXCardFile::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCards)
	{
		Broadcast(message);
	}
	else
	{
		JXWidgetSet::Receive(sender, message);
	}
}
