/******************************************************************************
 JXPathInput.h

	Interface for the JXPathInput class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPathInput
#define _H_JXPathInput

#include "JXInputField.h"

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

	virtual JBoolean	InputValid() override;
	virtual JBoolean	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

	JBoolean	HasBasePath() const;
	JBoolean	GetBasePath(JString* path) const;
	void		SetBasePath(const JString& path);
	void		ClearBasePath();

	JBoolean	WillAllowInvalidPath() const;
	void		ShouldAllowInvalidPath(const JBoolean allow = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	ChoosePath(const JString& prompt, const JString& instr = JString::empty);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

	static JFont	GetFont();

	static JColorID	GetTextColor(const JString& path, const JString& base,
								 const JBoolean requireWrite);
	static JBoolean		IsCharacterInWord(const JUtf8Character& c);

	static JBoolean		Complete(JXInputField* te, const JString& basePath,
								 JDirInfo* completer,
								 JXStringCompletionMenu** menu);

protected:

	virtual void	HandleUnfocusEvent() override;
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDEnter() override;
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void		HandleDNDLeave() override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

private:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JXPathInput* field, JFontManager* fontManager)
			:
			JXInputField::StyledText(kJFalse, fontManager),
			itsField(field)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const JBoolean deletion) override;

		private:

		JXPathInput*	itsField;
	};

private:

	JString		itsBasePath;
	JBoolean	itsAllowInvalidPathFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// nullptr until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// nullptr until first needed

private:

	JBoolean	GetDroppedDirectory(const Time time, const JBoolean reportErrors,
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
	RecalcAll();
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
