/******************************************************************************
 JTEKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEKeyHandler
#define _H_JTEKeyHandler

#include "JTextEditor.h"

class JTEKeyHandler
{
public:

	JTEKeyHandler();

	virtual ~JTEKeyHandler();

	virtual bool	HandleKeyPress(const JUtf8Character& c, const bool selectText,
								   const JTextEditor::CaretMotion motion,
								   const bool deleteToTabStop) = 0;

	// called by JTextEditor

	virtual void	Initialize(JTextEditor* te);

protected:

	JTextEditor*	GetTE() const;
	void			InsertKeyPress(const JUtf8Character& c);
	void			BackwardDelete(const bool deleteToTabStop, JString* text = nullptr);
	void			ForwardDelete(const bool deleteToTabStop, JString* text = nullptr);
	void			MoveCaretVert(const JInteger deltaLines);

private:

	JTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JTEKeyHandler(const JTEKeyHandler&) = delete;
	JTEKeyHandler& operator=(const JTEKeyHandler&) = delete;
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JTextEditor*
JTEKeyHandler::GetTE()
	const
{
	return itsTE;
}

#endif
