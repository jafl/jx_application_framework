/******************************************************************************
 SetElasticDialog.h

	Interface for the SetElasticDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_SetElasticDialog
#define _H_SetElasticDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXIntegerInput;

class SetElasticDialog : public JXModalDialogDirector
{
public:

	SetElasticDialog(const JIndex index, const JIndex maxIndex);

	~SetElasticDialog() override;

	JIndex	GetElasticIndex() const;

private:

// begin JXLayout

	JXIntegerInput* itsElasticIndex;

// end JXLayout

private:

	void	BuildWindow(const JIndex index, const JIndex maxIndex);
};

#endif
