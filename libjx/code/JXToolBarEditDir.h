/******************************************************************************
 JXToolBarEditDir.h

	Interface for the JXToolBarEditDir class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXToolBarEditDir
#define _H_JXToolBarEditDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JXToolBarButton.h>

class JTree;
class JXTextCheckbox;
class JXToolBarEditWidget;

class JXToolBarEditDir : public JXDialogDirector
{
public:

	JXToolBarEditDir(JTree* tree, const JBoolean show,
					 const JBoolean useSmall,
					 const JXToolBarButton::Type type,
					 JXDirector* supervisor);
	virtual ~JXToolBarEditDir();

	JBoolean	TreeChanged();

	JBoolean	ShowToolBar();
	JBoolean	UseSmallButtons();

	JXToolBarButton::Type	GetType();

	void		ReadSetup(istream& is);
	void		WriteSetup(ostream& os);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JTree*					itsTree;		// we don't own this
	JXToolBarEditWidget*	itsWidget;
	JBoolean				itsTreeChanged;

// begin JXLayout

    JXTextCheckbox* itsShowToolBarCB;
    JXTextCheckbox* itsUseSmallButtonsCB;
    JXTextCheckbox* itsShowImagesCB;
    JXTextCheckbox* itsShowTextCB;

// end JXLayout

private:

	void BuildWindow();

	// not allowed

	JXToolBarEditDir(const JXToolBarEditDir& source);
	const JXToolBarEditDir& operator=(const JXToolBarEditDir& source);
};


#endif
