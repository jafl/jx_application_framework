/******************************************************************************
 MDFilterRecordsDialog.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_MDFilterRecordsDialog
#define _H_MDFilterRecordsDialog

#include <JXDialogDirector.h>
#include <JMemoryManager.h>

class JXTextCheckbox;
class JXInputField;
class JXIntegerInput;

class MDFilterRecordsDialog : public JXDialogDirector
{
public:

	MDFilterRecordsDialog(JXDirector* supervisor);

	virtual	~MDFilterRecordsDialog();

	void	BuildFilter(JMemoryManager::RecordFilter* filter) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsFileCB;
	JXTextCheckbox* itsSizeCB;
	JXInputField*   itsFileInput;
	JXIntegerInput* itsSizeInput;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	MDFilterRecordsDialog(const MDFilterRecordsDialog& source);
	const MDFilterRecordsDialog& operator=(const MDFilterRecordsDialog& source);
};

#endif
