/******************************************************************************
 JXCardFile.h

	Interface for the JXCardFile class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCardFile
#define _H_JXCardFile

#include "JXWidgetSet.h"

class JXCardFile : public JXWidgetSet
{
public:

	JXCardFile(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~JXCardFile() override;

	JSize	GetCardCount() const;
	bool	GetCurrentCardIndex(JIndex* index) const;
	bool	GetCardIndex(JXContainer* card, JIndex* index) const;

	JXContainer*	InsertCard(const JIndex index);
	JXContainer*	PrependCard();
	JXContainer*	AppendCard();

	void	InsertCard(const JIndex index, JXWidgetSet* card);
	void	PrependCard(JXWidgetSet* card);
	void	AppendCard(JXWidgetSet* card);

	JXContainer*	RemoveCard(const JIndex index);
	void			DeleteCard(const JIndex index);
	void			KillFocusOnCurrentCard();

	bool	ShowCard(const JIndex index);
	bool	ShowCard(JXContainer* card);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<JXContainer>*	itsCards;			// we don't own the objects
	JIndex					itsCurrCardIndex;	// zero => none visible

protected:

	class CardIndexBase : public JBroadcaster::Message
		{
		public:

			CardIndexBase(const JUtf8Byte* type, const JIndex cardIndex)
				:
				JBroadcaster::Message(type),
				itsCardIndex(cardIndex)
				{ };

			bool
			IsCardVisible()
				const
			{
				return itsCardIndex > 0;
			};

			bool
			GetCardIndex
				(
				JIndex* index
				)
				const
			{
				*index = itsCardIndex;
				return itsCardIndex > 0;
			};

		private:

			JIndex	itsCardIndex;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kCardIndexChanged;		// ShowCard()
	static const JUtf8Byte* kCardIndexUpdated;		// adjustment from RemoveCard(), etc.
	static const JUtf8Byte* kCardRemoved;

	class CardIndexChanged : public CardIndexBase
	{
	public:

		CardIndexChanged(const JIndex cardIndex)
			:
			CardIndexBase(kCardIndexChanged, cardIndex)
		{ };
	};

	class CardIndexUpdated : public CardIndexBase
	{
	public:

		CardIndexUpdated(const JIndex cardIndex)
			:
			CardIndexBase(kCardIndexUpdated, cardIndex)
		{ };
	};

	class CardRemoved : public CardIndexBase
	{
	public:

		CardRemoved(const JIndex cardIndex)
			:
			CardIndexBase(kCardRemoved, cardIndex)
		{ };
	};
};


/******************************************************************************
 GetCardCount

 ******************************************************************************/

inline JSize
JXCardFile::GetCardCount()
	const
{
	return itsCards->GetElementCount();
}

/******************************************************************************
 GetCurrentCardIndex

 ******************************************************************************/

inline bool
JXCardFile::GetCurrentCardIndex
	(
	JIndex* index
	)
	const
{
	*index = itsCurrCardIndex;
	return itsCards->IndexValid(itsCurrCardIndex);
}

/******************************************************************************
 GetCardIndex

 ******************************************************************************/

inline bool
JXCardFile::GetCardIndex
	(
	JXContainer*	card,
	JIndex*			index
	)
	const
{
	return itsCards->Find(card, index);
}

/******************************************************************************
 PrependCard

 ******************************************************************************/

inline JXContainer*
JXCardFile::PrependCard()
{
	return InsertCard(1);
}

inline void
JXCardFile::PrependCard
	(
	JXWidgetSet* card
	)
{
	InsertCard(1, card);
}

/******************************************************************************
 AppendCard

 ******************************************************************************/

inline JXContainer*
JXCardFile::AppendCard()
{
	return InsertCard(GetCardCount()+1);
}

inline void
JXCardFile::AppendCard
	(
	JXWidgetSet* card
	)
{
	InsertCard(GetCardCount()+1, card);
}

/******************************************************************************
 DeleteCard

	Deletes the specified card and everything on it.

 ******************************************************************************/

inline void
JXCardFile::DeleteCard
	(
	const JIndex index
	)
{
	JXContainer* card = RemoveCard(index);
	jdelete card;
}

#endif
