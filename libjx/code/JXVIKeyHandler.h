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

	JXVIKeyHandler();

	virtual ~JXVIKeyHandler();

	virtual bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
									   const JTextEditor::CaretMotion motion,
									   const bool deleteToTabStop) override;

	virtual void	Initialize(JTextEditor* te) override;

private:

	JXTEBase*	itsJXTE;
};

#endif
