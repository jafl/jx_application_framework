/******************************************************************************
 SetElasticDialog.h

	Interface for the SetElasticDialog class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_SetElasticDialog
#define _H_SetElasticDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXIntegerInput;

class SetElasticDialog : public JXDialogDirector
{
public:

	SetElasticDialog(JXWindowDirector* supervisor,
					 const JIndex index, const JIndex maxIndex);

	virtual ~SetElasticDialog();

	JIndex	GetElasticIndex() const;

private:

// begin JXLayout

    JXIntegerInput* itsElasticIndex;

// end JXLayout

private:

	void	BuildWindow(const JIndex index, const JIndex maxIndex);

	// not allowed

	SetElasticDialog(const SetElasticDialog& source);
	const SetElasticDialog& operator=(const SetElasticDialog& source);
};

#endif
