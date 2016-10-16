/******************************************************************************
 JStringIterator.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JStringIterator
#define _H_JStringIterator

#include <JBroadcaster.h>
#include <JCharacterRange.h>
#include <JUtf8ByteRange.h>
#include <JUtf8Character.h>

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

	JBoolean	LocateNext(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocateNext(const JUtf8Character* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocateNext(const JUtf8Character* str, const JSize length, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocateNext(const std::string& str, const JBoolean caseSensitive = kJTrue) const;

	JBoolean	LocatePrev(const JString& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocatePrev(const JUtf8Character* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocatePrev(const JUtf8Character* str, const JSize length, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	LocatePrev(const std::string& str, const JBoolean caseSensitive = kJTrue) const;

	JString		GetSubstring(const JIndex firstCharIndex, const JIndex lastCharIndex) const;
	JString		GetSubstring(const JCharacterRange& range) const;	// allows empty range

	// only allowed if constructed from non-const JString*

	JBoolean	SetPrev(const JUtf8Character& data);
	JBoolean	SetNext(const JUtf8Character& data);

	JBoolean	RemovePrev(const JSize count = 1);
	JBoolean	RemoveNext(const JSize count = 1);

	void		ReplaceNext(const JSize count, const JString& str);
	void		ReplaceNext(const JSize count, const JString& str, const JCharacterRange& range);
	void		ReplaceNext(const JSize count, const JUtf8Byte* str);
	void		ReplaceNext(const JSize count, const JUtf8Byte* str, const JSize length);
	void		ReplaceNext(const JSize count, const JUtf8Byte* str, const JUtf8ByteRange& range);
	void		ReplaceNext(const JSize count, const std::string& str);
	void		ReplaceNext(const JSize count, const std::string& str, const JUtf8ByteRange& range);

	void		ReplacePrev(const JSize count, const JString& str);
	void		ReplacePrev(const JSize count, const JString& str, const JCharacterRange& range);
	void		ReplacePrev(const JSize count, const JUtf8Byte* str);
	void		ReplacePrev(const JSize count, const JUtf8Byte* str, const JSize length);
	void		ReplacePrev(const JSize count, const JUtf8Byte* str, const JUtf8ByteRange& range);
	void		ReplacePrev(const JSize count, const std::string& str);
	void		ReplacePrev(const JSize count, const std::string& str, const JUtf8ByteRange& range);

protected:

	const JString*	GetConstString() const;
	JBoolean		GetString(JString** obj) const;

	JCursorPosition	GetCursor() const;
	void			SetCursor(const JCursorPosition pos);

	void	StringChanged(const JBroadcaster::Message& message);

private:

	const JString*	itsConstString;			// JString that is being iterated over
	JString*		itsString;				// NULL if we were passed a const object
	JCursorPosition	itsCursorPosition;		// current iterator position - bytes!

	JStringIterator*	itsNextIterator;	// next iterator in linked list

private:

	void	JStringIteratorX(const JIteratorPosition start, const JIndex index);

	void	AddToIteratorList();
	void	RemoveFromIteratorList();

	// not allowed

	const JStringIterator& operator=(const JStringIterator& source);
};

#endif
