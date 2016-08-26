/******************************************************************************
 GXColByIncDialog.h

	Interface for the GXColByIncDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXColByIncDialog
#define _H_GXColByIncDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;
class JXRadioGroup;

class GXColByIncDialog : public JXDialogDirector
{
public:

	enum
	{
	kAscending = 1,
	kDescending
	};

public:

	GXColByIncDialog(JXWindowDirector* supervisor, const JSize count);

	virtual ~GXColByIncDialog();
	
	void GetDestination(JIndex* source);
	
	void GetValues(JFloat* beg, JFloat* inc, JInteger* count);
				
	JBoolean IsAscending();
	
protected:

	virtual JBoolean	OKToDeactivate();
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	
private:

	JIndex itsDestCol;

// begin JXLayout

    JXTextMenu*     itsDestMenu;
    JXFloatInput*   itsBeginning;
    JXFloatInput*   itsInc;
    JXIntegerInput* itsCount;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GXColByIncDialog(const GXColByIncDialog& source);
	const GXColByIncDialog& operator=(const GXColByIncDialog& source);
};

#endif
