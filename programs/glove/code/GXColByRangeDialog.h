/******************************************************************************
 GXColByRangeDialog.h

	Interface for the GXColByRangeDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXColByRangeDialog
#define _H_GXColByRangeDialog

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

	virtual JBoolean	OKToDeactivate() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	JIndex 		itsDestCol;
	JBoolean 	itsIsAscending;

// begin JXLayout

	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsEnd;
	JXIntegerInput* itsCount;
	JXTextMenu*     itsDestMenu;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GXColByRangeDialog(const GXColByRangeDialog& source);
	const GXColByRangeDialog& operator=(const GXColByRangeDialog& source);
};

#endif
