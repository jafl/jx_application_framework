/******************************************************************************
 FilterRecordsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_FilterRecordsDialog
#define _H_FilterRecordsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JMemoryManager.h>

class JXTextCheckbox;
class JXInputField;
class JXIntegerInput;

class FilterRecordsDialog : public JXDialogDirector
{
public:

	FilterRecordsDialog(JXDirector* supervisor);

	virtual ~FilterRecordsDialog();

	void	BuildFilter(JMemoryManager::RecordFilter* filter) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

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
