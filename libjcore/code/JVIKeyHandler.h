/******************************************************************************
 JVIKeyHandler.h

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVIKeyHandler
#define _H_JVIKeyHandler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEKeyHandler.h>

class JVIKeyHandler : public JTEKeyHandler
{
public:

	enum Mode
	{
		kCommandMode,
		kTextEntryMode
	};

public:

	JVIKeyHandler(JTextEditor* te);

	virtual ~JVIKeyHandler();

	virtual JBoolean	HandleKeyPress(const JCharacter key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop);

protected:

	Mode	GetMode() const;
	void	ClearKeyBuffer();

	JBoolean	PrehandleKeyPress(const JCharacter key, JBoolean* result);

private:

	Mode	itsMode;
	JString	itsKeyBuffer;

private:

	// not allowed

	JVIKeyHandler(const JVIKeyHandler& source);
	const JVIKeyHandler& operator=(const JVIKeyHandler& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JVIKeyHandler::Mode
JVIKeyHandler::GetMode()
	const
{
	return itsMode;
}

/******************************************************************************
 ClearKeyBuffer (protected)

 ******************************************************************************/

inline void
JVIKeyHandler::ClearKeyBuffer()
{
	itsKeyBuffer.Clear();
}

#endif
