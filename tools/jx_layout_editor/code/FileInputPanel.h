/******************************************************************************
 FileInputPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileInputPanel
#define _H_FileInputPanel

#include "WidgetPanelBase.h"

class JXTextCheckbox;

class FileInputPanel : public WidgetPanelBase
{
public:

	FileInputPanel(WidgetParametersDialog* dlog,
					const bool required, const bool invalid,
					const bool read, const bool write, const bool exec);

	~FileInputPanel();

	void	GetValues(bool* required, bool* invalid, bool* read, bool* write, bool* exec);

private:

// begin Panel

	JXTextCheckbox* itsFileRequiredCB;
	JXTextCheckbox* itsAllowInvalidFileCB;
	JXTextCheckbox* itsRequireFileReadCB;
	JXTextCheckbox* itsRequireFileWriteCB;
	JXTextCheckbox* itsRequireFileExecCB;

// end Panel

private:

	void	BuildPanel(WidgetParametersDialog* dlog,
						const bool required, const bool invalid,
						const bool read, const bool write, const bool exec);
};

#endif
