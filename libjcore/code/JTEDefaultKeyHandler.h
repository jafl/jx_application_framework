/******************************************************************************
 JTEDefaultKeyHandler.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEDefaultKeyHandler
#define _H_JTEDefaultKeyHandler

#include "JTEKeyHandler.h"

class JTEDefaultKeyHandler : public JTEKeyHandler
{
public:

	JTEDefaultKeyHandler();

	virtual ~JTEDefaultKeyHandler();

	virtual JBoolean	HandleKeyPress(const JUtf8Character& key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop) override;

private:

	// not allowed

	JTEDefaultKeyHandler(const JTEDefaultKeyHandler& source);
	const JTEDefaultKeyHandler& operator=(const JTEDefaultKeyHandler& source);
};

#endif
