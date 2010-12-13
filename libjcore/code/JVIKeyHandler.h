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
		kTextEntryMode,
		kCommandMode,
		kCommandLineMode,
		kBufferNameMode
	};

	enum
	{
		kNamedCutBufferOffset = '0',
		kNamedCutBufferCount = 'z' - kNamedCutBufferOffset + 1
	};

public:

	JVIKeyHandler(JTextEditor* te);

	virtual ~JVIKeyHandler();

	virtual JBoolean	HandleKeyPress(const JCharacter key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop);

public:

	struct CutBuffer
	{
		JString*	buf;
		JBoolean	line;

		CutBuffer()
			:
			buf(NULL), line(kJFalse)
			{ };

		~CutBuffer()
		{
			delete buf;
		};

		void Set(const JString& s, const JBoolean l);
	};

protected:

	Mode	GetMode() const;
	void	SetMode(const Mode mode);

	JSize	GetOperationCount() const;

	const JString&	GetCommandLine() const;
	void			AppendToCommandLine(const JCharacter key);
	void			ClearKeyBuffers();

	JBoolean	PrehandleKeyPress(const JCharacter key, JBoolean* result);
	void		YankLines(const JArray<JIndexRange>& matchList, const JBoolean del);

	CutBuffer*	GetCutBuffer(const JRegex& r) const;
	CutBuffer*	GetCutBuffer(const JRegex& r, const JArray<JIndexRange>& matchList) const;

private:

	Mode	itsMode;
	JString	itsKeyBuffer;
	JString	itsCmdLineBuffer;

	static CutBuffer theCutBuffer;
	static CutBuffer theNamedCutBuffer[ kNamedCutBufferCount ];

private:

	// not allowed

	JVIKeyHandler(const JVIKeyHandler& source);
	const JVIKeyHandler& operator=(const JVIKeyHandler& source);
};


/******************************************************************************
 GetMode (protected)

 ******************************************************************************/

inline JVIKeyHandler::Mode
JVIKeyHandler::GetMode()
	const
{
	return itsMode;
}

/******************************************************************************
 GetCommandLine (protected)

 ******************************************************************************/

inline const JString&
JVIKeyHandler::GetCommandLine()
	const
{
	return itsCmdLineBuffer;
}

/******************************************************************************
 AppendToCommandLine (protected)

 ******************************************************************************/

inline void
JVIKeyHandler::AppendToCommandLine
	(
	const JCharacter key
	)
{
	itsCmdLineBuffer.AppendCharacter(key);
}

/******************************************************************************
 ClearKeyBuffers (protected)

 ******************************************************************************/

inline void
JVIKeyHandler::ClearKeyBuffers()
{
	itsKeyBuffer.Clear();
	itsCmdLineBuffer.Clear();
}

#endif
