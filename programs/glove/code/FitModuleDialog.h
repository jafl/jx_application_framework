/******************************************************************************
 FitModuleDialog.h

	Interface for the FitModuleDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_FitModuleDialog
#define _H_FitModuleDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextMenu;
class JXTextButton;

class FitModuleDialog : public JXDialogDirector
{
public:

	FitModuleDialog(JXWindowDirector* supervisor);

	virtual ~FitModuleDialog();
	
	JIndex GetFilterIndex();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	
private:

	JIndex itsFilterIndex;

// begin JXLayout

    JXTextMenu* 	itsFilterMenu;
    JXTextButton* 	itsReloadButton;
    JXTextButton* 	itsOKButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	FitModuleDialog(const FitModuleDialog& source);
	const FitModuleDialog& operator=(const FitModuleDialog& source);
};

#endif
