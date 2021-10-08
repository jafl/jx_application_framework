/******************************************************************************
 JVIKeyHandler.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVIKeyHandler
#define _H_JVIKeyHandler

#include "jx-af/jcore/JTEKeyHandler.h"

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

	JVIKeyHandler();

	~JVIKeyHandler();

	void	Initialize(JTextEditor* te) override;
	bool	HandleKeyPress(const JUtf8Character& key, const bool selectText,
						   const JTextEditor::CaretMotion motion,
						   const bool deleteToTabStop) override;

public:

	struct CutBuffer
	{
		JString*	buf;
		bool	line;

		CutBuffer()
			:
			buf(nullptr), line(false)
			{ };

		~CutBuffer()
		{
			jdelete buf;
		};

		void Set(const JString& s, const bool l);
	};

protected:

	Mode	GetMode() const;
	void	SetMode(const Mode mode);

	JSize	GetOperationCount() const;
	bool	GetPrevCharacter(JUtf8Character* c) const;

	const JString&	GetCommandLine() const;
	void			AppendToCommandLine(const JUtf8Character& key);
	void			ClearKeyBuffers();

	bool		PrehandleKeyPress(const JUtf8Character& key, bool* result);
	void		YankLines(const JStringMatch& match, const bool del);
	void		YankToEndOfLine(const bool del, const bool ins);

	CutBuffer*	GetCutBuffer(const JRegex& r) const;
	CutBuffer*	GetCutBuffer(const JRegex& r, const JStringMatch& match) const;

private:

	Mode	itsMode;
	JString	itsKeyBuffer;
	JString	itsCmdLineBuffer;

	JTEDefaultKeyHandler*	itsDefKeyHandler;

	static CutBuffer theCutBuffer;
	static CutBuffer theNamedCutBuffer[ kNamedCutBufferCount ];
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
