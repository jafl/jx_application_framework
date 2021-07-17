/******************************************************************************
 CBVIKeyHandler.h

	Copyright © 2010 by John Lindal.

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

	virtual bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
									   const JTextEditor::CaretMotion motion,
									   const bool deleteToTabStop) override;

private:

	CBTextEditor*	itsCBTE;

private:

	// not allowed

	CBVIKeyHandler(const CBVIKeyHandler& source);
	const CBVIKeyHandler& operator=(const CBVIKeyHandler& source);
};

#endif
