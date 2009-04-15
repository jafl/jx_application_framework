/******************************************************************************
 JXFileInput.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFileInput
#define _H_JXFileInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JDirInfo;
class JXStringCompletionMenu;

class JXFileInput : public JXInputField
{
public:

	JXFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXFileInput();

	virtual JBoolean	InputValid();
	virtual JBoolean	GetFile(JString* fullName) const;
	virtual JString		GetTextForChooseFile() const;

	JBoolean	HasBasePath() const;
	JBoolean	GetBasePath(JString* path) const;
	void		SetBasePath(const JCharacter* path);
	void		ClearBasePath();

	JBoolean	WillAllowInvalidFile() const;
	void		ShouldAllowInvalidFile(const JBoolean allow = kJTrue);

	JBoolean	WillRequireReadable() const;
	void		ShouldRequireReadable(const JBoolean require = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	WillRequireExecutable() const;
	void		ShouldRequireExecutable(const JBoolean require = kJTrue);

	JBoolean	ChooseFile(const JCharacter* prompt, const JCharacter* instr = NULL);
	JBoolean	SaveFile(const JCharacter* prompt, const JCharacter* instr = NULL);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	static const JCharacter*	GetFont(JSize* size);

	static JColorIndex	GetTextColor(const JCharacter* fileName, const JCharacter* basePath,
									 const JBoolean requireRead, const JBoolean requireWrite,
									 const JBoolean requireExec, const JColormap* colormap);
	static JBoolean		IsCharacterInWord(const JString& text, const JIndex charIndex);

protected:

	virtual void	HandleUnfocusEvent();
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<Font>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	JString		itsBasePath;
	JBoolean	itsAllowInvalidFileFlag;
	JBoolean	itsRequireReadFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsRequireExecFlag;
	JBoolean	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// NULL until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// NULL until first needed

private:

	JBoolean		GetDroppedFileName(const Time time, const JBoolean reportErrors,
									   JString* fileName);

	// not allowed

	JXFileInput(const JXFileInput& source);
	const JXFileInput& operator=(const JXFileInput& source);
};


/******************************************************************************
 HasBasePath

 ******************************************************************************/

inline JBoolean
JXFileInput::HasBasePath()
	const
{
	return !itsBasePath.IsEmpty();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline JBoolean
JXFileInput::GetBasePath
	(
	JString* path
	)
	const
{
	*path = itsBasePath;
	return HasBasePath();
}

/******************************************************************************
 ClearBasePath

 ******************************************************************************/

inline void
JXFileInput::ClearBasePath()
{
	itsBasePath.Clear();
	RecalcAll(kJTrue);
}

/******************************************************************************
 Allow invalid file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillAllowInvalidFile()
	const
{
	return itsAllowInvalidFileFlag;
}

inline void
JXFileInput::ShouldAllowInvalidFile
	(
	const JBoolean allow
	)
{
	itsAllowInvalidFileFlag = allow;
}

/******************************************************************************
 Require readable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireReadable()
	const
{
	return itsRequireReadFlag;
}

inline void
JXFileInput::ShouldRequireReadable
	(
	const JBoolean require
	)
{
	itsRequireReadFlag = require;
}

/******************************************************************************
 Require writable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXFileInput::ShouldRequireWritable
	(
	const JBoolean require
	)
{
	itsRequireWriteFlag = require;
}

/******************************************************************************
 Require executable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireExecutable()
	const
{
	return itsRequireExecFlag;
}

inline void
JXFileInput::ShouldRequireExecutable
	(
	const JBoolean require
	)
{
	itsRequireExecFlag = require;
}

#endif
