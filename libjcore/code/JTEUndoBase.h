/******************************************************************************
 JTEUndoBase.h

	Copyright (C) 1996-2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoBase
#define _H_JTEUndoBase

#include <JUndo.h>
#include <JStyledTextBuffer.h>	// need struct definitions

class JTEUndoBase : public JUndo
{
public:

	JTEUndoBase(JStyledTextBuffer* buffer);

	virtual ~JTEUndoBase();

	// called by JStyledTextBuffer::SetAllFontNameAndSize()

	virtual void	SetFont(const JString& name, const JSize size);

protected:

	JStyledTextBuffer*	GetBuffer() const;

	void	SetFont(JRunArray<JFont>* styles,
					const JString& name, const JSize size);

private:

	JStyledTextBuffer*	itsBuffer;		// we don't own this

private:

	// not allowed

	JTEUndoBase(const JTEUndoBase& source);
	const JTEUndoBase& operator=(const JTEUndoBase& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JStyledTextBuffer*
JTEUndoBase::GetBuffer()
	const
{
	return itsBuffer;
}

#endif
