/******************************************************************************
 JXVIKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXVIKeyHandler
#define _H_JXVIKeyHandler

#include <jx-af/jcore/JVIKeyHandler.h>

class JXTEBase;

class JXVIKeyHandler : public JVIKeyHandler
{
public:

	JXVIKeyHandler();

	~JXVIKeyHandler();

	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
						   const JTextEditor::CaretMotion motion,
						   const bool deleteToTabStop) override;

	void	Initialize(JTextEditor* te) override;

private:

	JXTEBase*	itsJXTE;
};

#endif
