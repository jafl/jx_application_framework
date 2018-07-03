/******************************************************************************
 JXFSInputBase.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFSInputBase
#define _H_JXFSInputBase

#include "JXInputField.h"

class JDirInfo;
class JXStringCompletionMenu;

class JXFSInputBase : public JXInputField
{
public:

	virtual ~JXFSInputBase();

	JBoolean	HasBasePath() const;
	JBoolean	GetBasePath(JString* path) const;
	void		SetBasePath(const JString& path);
	void		ClearBasePath();

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

	static JFont		GetFont();
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

	JBoolean	GetDroppedEntry(const Time time, const JBoolean reportErrors,
								JString* name);

protected:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JXFSInputBase* field, JFontManager* fontManager)
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

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const = 0;

		private:

		JXFSInputBase*	itsField;
	};

protected:

	JXFSInputBase(StyledText* text, const JBoolean showFilesForCompletion,
				  const JUtf8Byte* defaultHintID, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

private:

	JString		itsBasePath;
	JBoolean	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// nullptr until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// nullptr until first needed
	const JBoolean			itsShowFilesForCompletionFlag;

	const JUtf8Byte*	itsDefaultHintID;

private:

	// not allowed

	JXFSInputBase(const JXFSInputBase& source);
	const JXFSInputBase& operator=(const JXFSInputBase& source);
};


/******************************************************************************
 HasBasePath

 ******************************************************************************/

inline JBoolean
JXFSInputBase::HasBasePath()
	const
{
	return !itsBasePath.IsEmpty();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline JBoolean
JXFSInputBase::GetBasePath
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
JXFSInputBase::ClearBasePath()
{
	itsBasePath.Clear();
	RecalcAll();
}

#endif
