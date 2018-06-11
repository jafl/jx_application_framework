/******************************************************************************
 JXVIKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXVIKeyHandler
#define _H_JXVIKeyHandler

#include <JVIKeyHandler.h>

class JXTEBase;

class JXVIKeyHandler : public JVIKeyHandler
{
public:

	JXVIKeyHandler(JXTEBase* te);

	virtual ~JXVIKeyHandler();

	virtual JBoolean	HandleKeyPress(const JUtf8Character& key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop) override;

private:

	JXTEBase*	itsJXTE;

private:

	// not allowed

	JXVIKeyHandler(const JXVIKeyHandler& source);
	const JXVIKeyHandler& operator=(const JXVIKeyHandler& source);
};

#endif
