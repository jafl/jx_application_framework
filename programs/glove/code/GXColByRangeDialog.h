/******************************************************************************
 GXColByRangeDialog.h

	Interface for the GXColByRangeDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXColByRangeDialog
#define _H_GXColByRangeDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;

class GXColByRangeDialog : public JXDialogDirector
{
public:

	GXColByRangeDialog(JXWindowDirector* supervisor, const JSize count);

	virtual ~GXColByRangeDialog();
	
	void GetDestination(JIndex* dest);
	
	void GetValues(JFloat* beg, JFloat* end, JInteger* count);
				
	JBoolean IsAscending();
	
protected:

	virtual JBoolean	OKToDeactivate();
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	
private:

	JIndex 		itsDestCol;
	JBoolean 	itsIsAscending;

// begin JXLayout

    JXTextMenu*     itsDestMenu;
    JXFloatInput*   itsBeginning;
    JXFloatInput*   itsEnd;
    JXIntegerInput* itsCount;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GXColByRangeDialog(const GXColByRangeDialog& source);
	const GXColByRangeDialog& operator=(const GXColByRangeDialog& source);
};

#endif
