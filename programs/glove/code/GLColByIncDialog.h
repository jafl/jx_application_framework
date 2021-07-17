/******************************************************************************
 GLColByIncDialog.h

	Interface for the GLColByIncDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLColByIncDialog
#define _H_GLColByIncDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;
class JXRadioGroup;

class GLColByIncDialog : public JXDialogDirector
{
public:

	enum
		{
		kAscending = 1,
		kDescending
		};

public:

	GLColByIncDialog(JXWindowDirector* supervisor, const JSize count);

	virtual ~GLColByIncDialog();
	
	void GetDestination(JIndex* source);
	
	void GetValues(JFloat* beg, JFloat* inc, JInteger* count);
				
	bool IsAscending();
	
protected:

	virtual bool	OKToDeactivate() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	JIndex itsDestCol;

// begin JXLayout

	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsInc;
	JXIntegerInput* itsCount;
	JXTextMenu*     itsDestMenu;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GLColByIncDialog(const GLColByIncDialog& source);
	const GLColByIncDialog& operator=(const GLColByIncDialog& source);
};

#endif
