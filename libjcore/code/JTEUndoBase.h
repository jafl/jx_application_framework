/******************************************************************************
 JTEUndoBase.h

	Copyright (C) 1996-2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEUndoBase
#define _H_JTEUndoBase

#include <JUndo.h>
#include <JStyledText.h>	// need struct definitions

class JTEUndoBase : public JUndo
{
public:

	JTEUndoBase(JStyledText* text);

	virtual ~JTEUndoBase();

	virtual void	SetCount(const JStyledText::TextCount& count);

	// called by JStyledText::SetAllFontNameAndSize()

	virtual void	SetFont(const JString& name, const JSize size);

protected:

	JStyledText*	GetText() const;

	void	SetFont(JRunArray<JFont>* styles,
					const JString& name, const JSize size);

private:

	JStyledText*	itsText;		// we don't own this

private:

	// not allowed

	JTEUndoBase(const JTEUndoBase& source);
	const JTEUndoBase& operator=(const JTEUndoBase& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JStyledText*
JTEUndoBase::GetText()
	const
{
	return itsText;
}

#endif
