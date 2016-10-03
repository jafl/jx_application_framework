/******************************************************************************
 GMFilterMBoxTable.h

	Interface for the GMFilterMBoxTable class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterMBoxTable
#define _H_GMFilterMBoxTable


#include <JXEditTable.h>

class GMFilterDialog;
class JXTextButton;
class JXInputField;
class GMFilter;

class GMFilterMBoxTable : public JXEditTable
{
public:

public:

	static GMFilterMBoxTable* Create(GMFilterDialog* dialog,
				JXTextButton* chooseButton, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMFilterMBoxTable();

	void		SetFilter(GMFilter* filter);
	JBoolean	OKToSwitch();

protected:

	GMFilterMBoxTable(GMFilterDialog* dialog,
				JXTextButton* chooseButton, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

private:

	GMFilterDialog* itsDialog;
	JXTextButton*	itsChooseButton;
	JXInputField*	itsInput;

	GMFilter*		itsFilter;	// we don't own this.

private:

	void GMFilterMBoxTableX();
	void AdjustColWidths();
	void AdjustButtons();

	// not allowed

	GMFilterMBoxTable(const GMFilterMBoxTable& source);
	const GMFilterMBoxTable& operator=(const GMFilterMBoxTable& source);

public:
};

#endif
