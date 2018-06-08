/******************************************************************************
 JXFileInput.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileInput
#define _H_JXFileInput

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

	virtual JBoolean	InputValid() override;
	virtual JBoolean	GetFile(JString* fullName) const;
	virtual JString		GetTextForChooseFile() const;

	JBoolean	HasBasePath() const;
	JBoolean	GetBasePath(JString* path) const;
	void		SetBasePath(const JString& path);
	void		ClearBasePath();

	JBoolean	WillAllowInvalidFile() const;
	void		ShouldAllowInvalidFile(const JBoolean allow = kJTrue);

	JBoolean	WillRequireReadable() const;
	void		ShouldRequireReadable(const JBoolean require = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	WillRequireExecutable() const;
	void		ShouldRequireExecutable(const JBoolean require = kJTrue);

	JBoolean	ChooseFile(const JString& prompt, const JString& instr = JString::empty);
	JBoolean	SaveFile(const JString& prompt, const JString& instr = JString::empty);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

	static JFont	GetFont();

	static JColorID	GetTextColor(const JString& fileName, const JString& basePath,
									 const JBoolean requireRead, const JBoolean requireWrite,
									 const JBoolean requireExec);
	static JBoolean		IsCharacterInWord(const JUtf8Character& c);

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

		StyledText(JXFileInput* field, JFontManager* fontManager)
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

		JXFileInput*	itsField;
	};

private:

	JString		itsBasePath;
	JBoolean	itsAllowInvalidFileFlag;
	JBoolean	itsRequireReadFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsRequireExecFlag;
	JBoolean	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// nullptr until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// nullptr until first needed

private:

	JBoolean	GetDroppedFileName(const Time time, const Atom type,
								   const JBoolean reportErrors, JString* fileName);

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
	RecalcAll();
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
