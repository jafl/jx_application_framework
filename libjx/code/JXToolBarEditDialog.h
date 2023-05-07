/******************************************************************************
 JXToolBarEditDialog.h

	Interface for the JXToolBarEditDialog class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXToolBarEditDir
#define _H_JXToolBarEditDir

#include "JXModalDialogDirector.h"
#include "JXToolBarButton.h"

class JTree;
class JXTextCheckbox;
class JXToolBarEditWidget;

class JXToolBarEditDialog : public JXModalDialogDirector
{
public:

	JXToolBarEditDialog(JTree* tree, const bool show,
					 const bool useSmall,
					 const JXToolBarButton::Type type);
	~JXToolBarEditDialog() override;

	bool	TreeChanged();

	bool	ShowToolBar();
	bool	UseSmallButtons();

	JXToolBarButton::Type	GetType() const;

	void	ReadSetup(std::istream& is);
	void	WriteSetup(std::ostream& os) const;

private:

	JTree*					itsTree;		// we don't own this
	JXToolBarEditWidget*	itsWidget;
	bool					itsTreeChanged;

// begin JXLayout

	JXTextCheckbox* itsShowToolBarCB;
	JXTextCheckbox* itsUseSmallButtonsCB;
	JXTextCheckbox* itsShowImagesCB;
	JXTextCheckbox* itsShowTextCB;

// end JXLayout

private:

	void BuildWindow();
};

#endif
