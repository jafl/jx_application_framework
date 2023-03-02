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

	~JXFSInputBase() override;

	bool	HasBasePath() const;
	bool	GetBasePath(JString* path) const;
	void	SetBasePath(const JString& path);
	void	ClearBasePath();

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

	static JFont	GetFont();
	static bool		IsCharacterInWord(const JUtf8Character& c);

	static bool		Complete(JXInputField* te, const JString& basePath,
							 JDirInfo* completer,
							 JXStringCompletionMenu** menu);

protected:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JXFSInputBase* field, JFontManager* fontManager)
			:
			JXInputField::StyledText(false, fontManager),
			itsField(field)
		{ };

		protected:

		void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const = 0;

		private:

		JXFSInputBase*	itsField;
	};

protected:

	JXFSInputBase(StyledText* text, const bool showFilesForCompletion,
				  const JUtf8Byte* defaultHintID, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	void	HandleUnfocusEvent() override;
	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	void	BoundsMoved(const JCoordinate dx, const JCoordinate dy) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	void		HandleDNDEnter() override;
	void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	void		HandleDNDLeave() override;
	void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	bool	GetDroppedEntry(const Time time, const bool reportErrors,
								JString* name);

private:

	JString		itsBasePath;
	bool	itsExpectURLDropFlag;

	JDirInfo*				itsCompleter;		// nullptr until first needed
	JXStringCompletionMenu*	itsCompletionMenu;	// nullptr until first needed
	const bool			itsShowFilesForCompletionFlag;

	const JUtf8Byte*	itsDefaultHintID;
};


/******************************************************************************
 HasBasePath

 ******************************************************************************/

inline bool
JXFSInputBase::HasBasePath()
	const
{
	return !itsBasePath.IsEmpty();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline bool
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
	GetText()->RestyleAll();
}

#endif
