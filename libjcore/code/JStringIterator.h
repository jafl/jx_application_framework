/******************************************************************************
 JStringIterator.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStringIterator
#define _H_JStringIterator

#include <JUtf8Character.h>
#include <JBroadcaster.h>

class JString;

class JStringIterator
{
	friend class JString;

public:

	JStringIterator(const JString& theOrderedSet,
					const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	JStringIterator(JString* theOrderedSet,
					const JIteratorPosition start = kJIteratorStartAtBeginning,
					const JIndex index = 0);
	JStringIterator(const JStringIterator& source);

	~JStringIterator();

	JBoolean	Prev(JUtf8Character* data);
	JBoolean	Next(JUtf8Character* data);

	void		SkipPrev(const JSize count = 1);
	void		SkipNext(const JSize count = 1);

	void		MoveTo(const JIteratorPosition newPosition, const JIndex index);
	JBoolean	AtBeginning();
	JBoolean	AtEnd();

	// only allowed if constructed from non-const JString*

	JBoolean	SetPrev(const JUtf8Character& data);
	JBoolean	SetNext(const JUtf8Character& data);

	JBoolean	RemovePrev();
	JBoolean	RemoveNext();

protected:

	const JString*	GetConstString() const;
	JBoolean		GetString(JString** obj) const;

	JCursorPosition	GetCursor() const;
	void			SetCursor(const JCursorPosition pos);

	void	JStringChanged(const JBroadcaster::Message& message);

private:

	const JString*	itsConstOrderedSet;		// JString that is being iterated over
	JString*		itsOrderedSet;			// NULL if we were passed a const object
	JCursorPosition	itsCursorPosition;		// current iterator position

	JStringIterator*	itsNextIterator;		// next iterator in linked list

private:

	void	JStringIteratorX(const JIteratorPosition start, const JIndex index);

	void	AddToIteratorList();
	void	RemoveFromIteratorList();

	// not allowed

	const JStringIterator& operator=(const JStringIterator& source);
};

#endif
