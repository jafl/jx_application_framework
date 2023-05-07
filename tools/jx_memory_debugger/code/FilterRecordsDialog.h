/******************************************************************************
 FilterRecordsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_FilterRecordsDialog
#define _H_FilterRecordsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JMemoryManager.h>

class JXTextCheckbox;
class JXInputField;
class JXIntegerInput;

class FilterRecordsDialog : public JXModalDialogDirector
{
public:

	FilterRecordsDialog();

	~FilterRecordsDialog() override;

	void	BuildFilter(JMemoryManager::RecordFilter* filter) const;

private:

// begin JXLayout

	JXTextCheckbox* itsFileCB;
	JXTextCheckbox* itsSizeCB;
	JXInputField*   itsFileInput;
	JXIntegerInput* itsSizeInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
