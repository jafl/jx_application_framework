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

	~JTEDefaultKeyHandler() override;

	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
						   const JTextEditor::CaretMotion motion,
						   const bool deleteToTabStop) override;
};

#endif
