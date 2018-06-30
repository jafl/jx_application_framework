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

	JXInputField(const JBoolean wordWrap, const JBoolean acceptNewline,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXInputField();

	void	SetFontName(const JString& name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& font);

	JBoolean	IsRequired() const;
	void		SetIsRequired(const JBoolean isRequired = kJTrue);

	void	SetLengthLimits(const JSize minLength, const JSize maxLength);

	JSize	GetMinLength() const;
	void	SetMinLength(const JSize minLength);
	void	ClearMinLength();

	JSize	GetMaxLength() const;
	void	SetMaxLength(const JSize maxLength);
	void	ClearMaxLength();

	virtual JBoolean	InputValid();

	JBoolean	IsTableInput() const;
	JBoolean	GetTable(JXEditTable** table) const;
	void		SetTable(JXEditTable* table);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

	virtual JString	ToString() const override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(const JBoolean acceptNewline, JFontManager* fontManager)
			:
			JXStyledText(kJFalse, kJFalse, fontManager),
			itsAcceptNewlineFlag(acceptNewline)
		{ };

		protected:

		virtual JBoolean	NeedsToFilterText(const JString& text) const override;
		virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style) override;

		private:

		const JBoolean	itsAcceptNewlineFlag;
	};

protected:

	JXInputField(StyledText* text, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual void		HandleFocusEvent() override;
	virtual void		HandleUnfocusEvent() override;
	virtual JBoolean	OKToUnfocus() override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source) override;

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual JCoordinate	GetFTCMinContentSize(const JBoolean horizontal) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JSize	itsMinLength;
	JSize	itsMaxLength;		// 0 => no maximum

	JXTextMenu*		itsContextMenu;	// nullptr until first used
	JXEditTable*	itsTable;		// can be nullptr; if not, it owns us

private:

	void	JXInputFieldX();

	void		CreateContextMenu();
	void		UpdateContextMenu();
	void		HandleContextMenu(const JIndex index);
	JBoolean	ContextMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean	ContextMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	// not allowed

	JXInputField(const JXInputField& source);
	const JXInputField& operator=(const JXInputField& source);
};


/******************************************************************************
 Required

	This is provided for backward compatibility, but is deprecated since
	it is not orthogonal to Get/SetMinLength().

 ******************************************************************************/

inline JBoolean
JXInputField::IsRequired()
	const
{
	return JI2B( itsMinLength > 0 );
}

inline void
JXInputField::SetIsRequired
	(
	const JBoolean isRequired
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

inline JBoolean
JXInputField::IsTableInput()
	const
{
	return JI2B( itsTable != nullptr );
}

inline JBoolean
JXInputField::GetTable
	(
	JXEditTable** table
	)
	const
{
	*table = itsTable;
	return JI2B( itsTable != nullptr );
}

#endif
