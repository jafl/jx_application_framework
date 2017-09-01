/******************************************************************************
 JXInputField.h

	Interface for the JXInputField class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXInputField
#define _H_JXInputField

#include <JXTEBase.h>

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

	void	SetFontName(const JCharacter* name);
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

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	virtual JString	ToString() const;

protected:

	virtual void		HandleFocusEvent();
	virtual void		HandleUnfocusEvent();
	virtual JBoolean	OKToUnfocus();

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
								  const Atom action, const Time time,
								  const JXWidget* source);

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual JBoolean	NeedsToFilterText(const JCharacter* text) const;
	virtual JBoolean	FilterText(JString* text, JRunArray<JFont>* style);

	virtual JCoordinate	GetFTCMinContentSize(const JBoolean horizontal) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsAcceptNewlineFlag;
	JSize		itsMinLength;
	JSize		itsMaxLength;		// 0 => no maximum

	JXTextMenu*		itsContextMenu;	// NULL until first used
	JXEditTable*	itsTable;		// can be NULL; if not, it owns us

private:

	void	JXInputFieldX(const JBoolean acceptNewline);

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
	return JI2B( itsTable != NULL );
}

inline JBoolean
JXInputField::GetTable
	(
	JXEditTable** table
	)
	const
{
	*table = itsTable;
	return JI2B( itsTable != NULL );
}

#endif
