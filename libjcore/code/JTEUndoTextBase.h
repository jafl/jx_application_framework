/******************************************************************************
 JTEUndoTextBase.h

	Interface for the JTEUndoTextBase class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoTextBase
#define _H_JTEUndoTextBase

#include <JTEUndoBase.h>

class JTEUndoTextBase : public JTEUndoBase
{
public:

	JTEUndoTextBase(JStyledTextBuffer* te);

	virtual ~JTEUndoTextBase();

	virtual void	Undo() override;

	virtual void	SetFont(const JString& name, const JSize size) override;

protected:

	void	PrepareForUndo(const JStyledTextBuffer::TextIndex& start,
						   const JStyledTextBuffer::TextCount& count);

	void	PrependToSave(const JUtf8Character& c, const JIndex charIndex);
	void	AppendToSave(const JUtf8Character& c, const JIndex charIndex);

private:

	JString				itsOrigBuffer;
	JRunArray<JFont>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoTextBase(const JTEUndoTextBase& source);
	const JTEUndoTextBase& operator=(const JTEUndoTextBase& source);
};

#endif
