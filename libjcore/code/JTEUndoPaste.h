/******************************************************************************
 JTEUndoPaste.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#include <JTEUndoTextBase.h>

class JTEUndoPaste : public JTEUndoTextBase
{
public:

	JTEUndoPaste(JStyledTextBuffer* te, const JStyledTextBuffer::TextIndex& start,
				 const JStyledTextBuffer::TextCount& pasteCount);

	virtual ~JTEUndoPaste();

	virtual void	Undo() override;

	void	SetCount(const JStyledTextBuffer::TextCount& count);

protected:


private:

	JStyledTextBuffer::TextIndex	itsStart;
	JStyledTextBuffer::TextCount	itsCount;

private:

	// not allowed

	JTEUndoPaste(const JTEUndoPaste& source);
	const JTEUndoPaste& operator=(const JTEUndoPaste& source);
};


/******************************************************************************
 SetCount

 ******************************************************************************/

inline void
JTEUndoPaste::SetCount
	(
	const JStyledTextBuffer::TextCount& count
	)
{
	itsCount = count;
}

#endif
