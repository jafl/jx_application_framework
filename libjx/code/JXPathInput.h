/******************************************************************************
 JXPathInput.h

	Interface for the JXPathInput class

	Copyright © 1996 by Glenn W. Bach.
	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPathInput
#define _H_JXPathInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JDirInfo;
class JXStringCompletionMenu;

class JXPathInput : public JXInputField
{
public:

	JXPathInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXPathInput();

	virtual JBoolean	InputValid();
	virtual JBoolean	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

	JBoolean	HasBasePath() const;
	JBoolean	GetBasePath(JString* path) const;
	void		SetBasePath(const JCharacter* path);
	void		ClearBasePath();

	JBoolean	WillAllowInvalidPath() const;
	void		ShouldAllowInvalidPath(const JBoolean allow = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	ChoosePath(const JCharacter* prompt, const JCharacter* instr = NULL);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	static const JCharacter*	GetFont(JSize* size);

	static JColorIndex	GetTextColor(const JCharacter* path, const JCharacter* base,
									 const JBoolean requireWrite, const JColormap* colormap);
	static JBoolean		IsCharacterInWord(const JString& text, const JIndex charIndex);

	static JBoolean		Complete(JXInputField* te, const JCharacter* basePath,
								 JDirInfo* completer,
								 JXStringCompletionMenu** menu);

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
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	JString		itsBasePath;
	JBoolean	itsAllowInvalidPathFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// NULL until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// NULL until first needed

private:

	JBoolean		GetDroppedDirectory(const Time time, const JBoolean reportErrors,
										JString* dirName);

	// not allowed

	JXPathInput(const JXPathInput& source);
	const JXPathInput& operator=(const JXPathInput& source);
};


/******************************************************************************
 HasBasePath

 ******************************************************************************/

inline JBoolean
JXPathInput::HasBasePath()
	const
{
	return !itsBasePath.IsEmpty();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline JBoolean
JXPathInput::GetBasePath
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
JXPathInput::ClearBasePath()
{
	itsBasePath.Clear();
	RecalcAll(kJTrue);
}

/******************************************************************************
 Allow invalid path

 ******************************************************************************/

inline JBoolean
JXPathInput::WillAllowInvalidPath()
	const
{
	return itsAllowInvalidPathFlag;
}

inline void
JXPathInput::ShouldAllowInvalidPath
	(
	const JBoolean allow
	)
{
	itsAllowInvalidPathFlag = allow;
}

/******************************************************************************
 Require writable directory

 ******************************************************************************/

inline JBoolean
JXPathInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXPathInput::ShouldRequireWritable
	(
	const JBoolean require
	)
{
	itsRequireWriteFlag = require;
}

#endif
