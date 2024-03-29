/******************************************************************************
 JXInputField.h

	Interface for the JXInputField class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXInputField
#define _H_JXInputField

#include "JXTEBase.h"
#include "JXStyledText.h"

class JXTextMenu;
class JXEditTable;

class JXInputField : public JXTEBase
{
public:

	JXInputField(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	JXInputField(const bool wordWrap, const bool acceptNewline,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~JXInputField() override;

	void	SetFontName(const JString& name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& font);

	bool	IsRequired() const;
	void	SetIsRequired(const bool isRequired = true);

	void	SetLengthLimits(const JSize minLength, const JSize maxLength);

	JSize	GetMinLength() const;
	void	SetMinLength(const JSize minLength);
	void	ClearMinLength();

	JSize	GetMaxLength() const;
	void	SetMaxLength(const JSize maxLength);
	void	ClearMaxLength();

	void	SetValidationPattern(JRegex* pattern, const JUtf8Byte* msgID);

	virtual bool	InputValid();

	bool	IsTableInput() const;
	bool	GetTable(JXEditTable** table) const;
	void	SetTable(JXEditTable* table);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

	JString	ToString() const override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(const bool acceptNewline, JFontManager* fontManager)
			:
			JXStyledText(false, false, fontManager),
			itsAcceptNewlineFlag(acceptNewline)
		{ };

		protected:

		bool	NeedsToFilterText(const JString& text) const override;
		bool	FilterText(JString* text, JRunArray<JFont>* style) override;

		private:

		const bool	itsAcceptNewlineFlag;
	};

protected:

	JXInputField(StyledText* text, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	void	HandleFocusEvent() override;
	void	HandleUnfocusEvent() override;
	bool	OKToUnfocus() override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source) override;

	void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

private:

	JSize	itsMinLength;
	JSize	itsMaxLength;			// 0 => no maximum

	JRegex*				itsPattern;	// nullptr => no validation
	const JUtf8Byte*	itsPatternMsgID;

	JXTextMenu*		itsContextMenu;	// nullptr until first used
	JXEditTable*	itsTable;		// can be nullptr; if not, it owns us

private:

	void	JXInputFieldX();

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);
	bool	ContextMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	bool	ContextMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;
};


/******************************************************************************
 Required

	This is provided for backward compatibility, but is deprecated since
	it is not orthogonal to Get/SetMinLength().

 ******************************************************************************/

inline bool
JXInputField::IsRequired()
	const
{
	return itsMinLength > 0;
}

inline void
JXInputField::SetIsRequired
	(
	const bool isRequired
	)
{
	itsMinLength = isRequired ? 1 : 0;
}

/******************************************************************************
 Min length

 ******************************************************************************/

inline JSize
JXInputField::GetMinLength()
	const
{
	return itsMinLength;
}

inline void
JXInputField::ClearMinLength()
{
	itsMinLength = 0;
}

/******************************************************************************
 Max length

 ******************************************************************************/

inline JSize
JXInputField::GetMaxLength()
	const
{
	return itsMaxLength;
}

inline void
JXInputField::ClearMaxLength()
{
	itsMaxLength = 0;
}

/******************************************************************************
 GetTable

 ******************************************************************************/

inline bool
JXInputField::IsTableInput()
	const
{
	return itsTable != nullptr;
}

inline bool
JXInputField::GetTable
	(
	JXEditTable** table
	)
	const
{
	*table = itsTable;
	return itsTable != nullptr;
}

#endif
