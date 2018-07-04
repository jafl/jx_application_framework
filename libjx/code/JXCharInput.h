/******************************************************************************
 JXCharInput.h

	Interface for the JXCharInput class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCharInput
#define _H_JXCharInput

#include "JXInputField.h"

class JXCharInput : public JXInputField
{
public:

	JXCharInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXCharInput();

	JUtf8Character	GetCharacter() const;
	void			SetCharacter(const JUtf8Character& c);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual void		HandleMouseUp(const JPoint& pt, const JXMouseButton button,
									  const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers) override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;

private:

	// not allowed

	JXCharInput(const JXCharInput& source);
	const JXCharInput& operator=(const JXCharInput& source);
};

#endif
