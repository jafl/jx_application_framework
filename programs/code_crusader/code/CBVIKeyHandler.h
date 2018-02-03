/******************************************************************************
 CBVIKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBVIKeyHandler
#define _H_CBVIKeyHandler

#include <JXVIKeyHandler.h>

class CBTextEditor;

class CBVIKeyHandler : public JXVIKeyHandler
{
public:

	CBVIKeyHandler(CBTextEditor* te);

	virtual ~CBVIKeyHandler();

	virtual JBoolean	HandleKeyPress(const JCharacter key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop);

private:

	CBTextEditor*	itsCBTE;

private:

	// not allowed

	CBVIKeyHandler(const CBVIKeyHandler& source);
	const CBVIKeyHandler& operator=(const CBVIKeyHandler& source);
};

#endif
