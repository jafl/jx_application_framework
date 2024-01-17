/******************************************************************************
 FileInput.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileInput
#define _H_FileInput

#include "InputFieldBase.h"

class FileInputPanel;

class FileInput : public InputFieldBase
{
public:

	FileInput(LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	FileInput(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~FileInput() override;

	void	StreamOut(std::ostream& output) const override;
	JString	ToString() const override;

protected:

	JString	GetClassName() const override;
	void	PrintConfiguration(std::ostream& output,
							   const JString& indent,
							   const JString& varName,
							   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	bool			itsFileRequiredFlag;
	bool			itsAllowInvalidFlag;
	bool			itsReadFlag;
	bool			itsWriteFlag;
	bool			itsExecFlag;
	FileInputPanel*	itsPanel;	// nullptr unless editing
};

#endif
