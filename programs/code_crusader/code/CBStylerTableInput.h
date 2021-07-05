/******************************************************************************
 CBStylerTableInput.h

	Interface for the CBStylerTableInput class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBStylerTableInput
#define _H_CBStylerTableInput

#include <JXInputField.h>
#include "CBTextFileType.h"

class CBStylerTable;

class CBStylerTableInput : public JXInputField
{
public:

	CBStylerTableInput(const CBTextFileType fileType, CBStylerTable* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBStylerTableInput();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	CBStylerTable*	itsStylerTable;		// the table owns us
	CBTextFileType	itsFileType;

private:

	// not allowed

	CBStylerTableInput(const CBStylerTableInput& source);
	const CBStylerTableInput& operator=(const CBStylerTableInput& source);
};

#endif
