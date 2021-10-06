/******************************************************************************
 JTEDefaultKeyHandler.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTEDefaultKeyHandler
#define _H_JTEDefaultKeyHandler

#include "jx-af/jcore/JTEKeyHandler.h"

class JTEDefaultKeyHandler : public JTEKeyHandler
{
public:

	JTEDefaultKeyHandler();

	virtual ~JTEDefaultKeyHandler();

	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
									   const JTextEditor::CaretMotion motion,
									   const bool deleteToTabStop) override;
};

#endif
