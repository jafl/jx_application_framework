/******************************************************************************
 SetElasticDialog.h

	Interface for the SetElasticDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_SetElasticDialog
#define _H_SetElasticDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXIntegerInput;

class SetElasticDialog : public JXDialogDirector
{
public:

	SetElasticDialog(JXWindowDirector* supervisor,
					 const JIndex index, const JIndex maxIndex);

	~SetElasticDialog();

	JIndex	GetElasticIndex() const;

private:

// begin JXLayout

	JXIntegerInput* itsElasticIndex;

// end JXLayout

private:

	void	BuildWindow(const JIndex index, const JIndex maxIndex);
};

#endif
