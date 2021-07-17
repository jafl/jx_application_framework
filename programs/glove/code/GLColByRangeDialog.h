/******************************************************************************
 GLColByRangeDialog.h

	Interface for the GLColByRangeDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLColByRangeDialog
#define _H_GLColByRangeDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;

class GLColByRangeDialog : public JXDialogDirector
{
public:

	GLColByRangeDialog(JXWindowDirector* supervisor, const JSize count);

	virtual ~GLColByRangeDialog();
	
	void GetDestination(JIndex* dest);
	
	void GetValues(JFloat* beg, JFloat* end, JInteger* count);
				
	bool IsAscending();
	
protected:

	virtual bool	OKToDeactivate() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	JIndex 		itsDestCol;
	bool 	itsIsAscending;

// begin JXLayout

	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsEnd;
	JXIntegerInput* itsCount;
	JXTextMenu*     itsDestMenu;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GLColByRangeDialog(const GLColByRangeDialog& source);
	const GLColByRangeDialog& operator=(const GLColByRangeDialog& source);
};

#endif
