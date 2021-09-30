/******************************************************************************
 RecordTable.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_RecordTable
#define _H_RecordTable

#include <jx-af/jx/JXEditTable.h>

class RecordList;
class Record;

class RecordTable : public JXEditTable
{
public:

	RecordTable(RecordList* list,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~RecordTable();

	bool	GetSelectedRecord(const Record** entry) const;
	void		OpenSelectedFiles() const;

	virtual bool	IsEditable(const JPoint& cell) const override;

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleFocusEvent() override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool			ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const override;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	RecordList*		itsRecordList;
	mutable JString		itsKeyBuffer;
	const Record* 	itsSelectedRecord;	// nullptr unless updating
	JCoordinate			itsMinDataWidth;

private:

	void	UpdateTable();
	void	AdjustDataColWidth(const JCoordinate w);
	void	DrawRowBackground(JPainter& p, const JPoint& cell, const JRect& rect,
							  const JColorID color);
	void	DrawRecordState(const Record& record,
							JPainter& p, const JRect& rect);
};

#endif
