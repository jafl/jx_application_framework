/******************************************************************************
 CMPlot2DExprTable.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMPlot2DExprTable
#define _H_CMPlot2DExprTable

#include <JXStringTable.h>

class JXMenuBar;
class JXColHeaderWidget;

class CMPlot2DExprTable : public JXStringTable
{
public:

	enum
	{
		kXExprColIndex = 1,
		kYExprColIndex,
		kRangeMinColIndex,
		kRangeMaxColIndex,	// = kColCount

		kColCount = kRangeMaxColIndex
	};

public:

	CMPlot2DExprTable(JXMenuBar* menuBar, JStringTableData* data,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CMPlot2DExprTable();

	void	ConfigureColHeader(JXColHeaderWidget* colHeader);

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

private:

	// not allowed

	CMPlot2DExprTable(const CMPlot2DExprTable& source);
	const CMPlot2DExprTable& operator=(const CMPlot2DExprTable& source);
};

#endif
