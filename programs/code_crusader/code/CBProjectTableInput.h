/******************************************************************************
 CBProjectTableInput.h

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectTableInput
#define _H_CBProjectTableInput

#include <JXFileInput.h>

class CBProjectTable;

class CBProjectTableInput : public JXFileInput
{
public:

	CBProjectTableInput(CBProjectTable* table, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~CBProjectTableInput();

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

private:

	CBProjectTable*	itsProjectTable;

private:

	// not allowed

	CBProjectTableInput(const CBProjectTableInput& source);
	const CBProjectTableInput& operator=(const CBProjectTableInput& source);
};

#endif
