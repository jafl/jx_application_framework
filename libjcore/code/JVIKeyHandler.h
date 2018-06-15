/******************************************************************************
 JVIKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVIKeyHandler
#define _H_JVIKeyHandler

#include "JTEKeyHandler.h"

class JTEDefaultKeyHandler;

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

	virtual void		Initialize() override;
	virtual JBoolean	HandleKeyPress(const JUtf8Character& key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop) override;

public:

	struct CutBuffer
	{
		JString*	buf;
		JBoolean	line;

		CutBuffer()
			:
			buf(nullptr), line(kJFalse)
			{ };

		~CutBuffer()
		{
			jdelete buf;
		};

		void Set(const JString& s, const JBoolean l);
	};

protected:

	Mode	GetMode() const;
	void	SetMode(const Mode mode);

	JSize		GetOperationCount() const;
	JBoolean	GetPrevCharacter(JUtf8Character* c) const;

	const JString&	GetCommandLine() const;
	void			AppendToCommandLine(const JUtf8Character& key);
	void			ClearKeyBuffers();

	JBoolean	PrehandleKeyPress(const JUtf8Character& key, JBoolean* result);
	void		YankLines(const JStringMatch& match, const JBoolean del);
	void		YankToEndOfLine(const JBoolean del, const JBoolean ins);

	CutBuffer*	GetCutBuffer(const JRegex& r) const;
	CutBuffer*	GetCutBuffer(const JRegex& r, const JStringMatch& match) const;

private:

	Mode	itsMode;
	JString	itsKeyBuffer;
	JString	itsCmdLineBuffer;

	JTEDefaultKeyHandler*	itsDefKeyHandler;

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
	const JUtf8Character& key
	)
{
	itsCmdLineBuffer.Append(key);
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
