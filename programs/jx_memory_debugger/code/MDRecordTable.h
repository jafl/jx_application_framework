/******************************************************************************
 MDRecordTable.h

	Copyright (C) 2010 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_MDRecordTable
#define _H_MDRecordTable

#include <JXEditTable.h>

class MDRecordList;
class MDRecord;

class MDRecordTable : public JXEditTable
{
public:

	MDRecordTable(MDRecordList* list,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~MDRecordTable();

	JBoolean	GetSelectedRecord(const MDRecord** entry) const;
	void		OpenSelectedFiles() const;

	virtual JBoolean	IsEditable(const JPoint& cell) const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

	virtual void	HandleFocusEvent();
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	MDRecordList*		itsRecordList;
	mutable JString		itsKeyBuffer;
	const MDRecord* 	itsSelectedRecord;	// NULL unless updating
	JCoordinate			itsMinDataWidth;

private:

	void	UpdateTable();
	void	AdjustDataColWidth(const JCoordinate w);
	void	DrawRowBackground(JPainter& p, const JPoint& cell, const JRect& rect,
							  const JColorIndex color);
	void	DrawRecordState(const MDRecord& record,
							JPainter& p, const JRect& rect);

	// not allowed

	MDRecordTable(const MDRecordTable& source);
	const MDRecordTable& operator=(const MDRecordTable& source);
};

#endif
