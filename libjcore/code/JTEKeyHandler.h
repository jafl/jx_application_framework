/******************************************************************************
 JTEKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEKeyHandler
#define _H_JTEKeyHandler

#include <JTextEditor.h>

class JTEKeyHandler
{
public:

	JTEKeyHandler(JTextEditor* te);

	virtual ~JTEKeyHandler();

	virtual JBoolean	HandleKeyPress(const JUtf8Character& key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop) = 0;

	// called by JTextEditor

	virtual void	Initialize();

protected:

	JTextEditor*	GetTE() const;
	void			InsertKeyPress(const JUtf8Character& key);
	void			BackwardDelete(const JBoolean deleteToTabStop, JString* text = NULL);
	void			ForwardDelete(const JBoolean deleteToTabStop, JString* text = NULL);
	void			MoveCaretVert(const JInteger deltaLines);

private:

	JTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JTEKeyHandler(const JTEKeyHandler& source);
	const JTEKeyHandler& operator=(const JTEKeyHandler& source);
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
