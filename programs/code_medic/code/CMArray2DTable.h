/******************************************************************************
 CMArray2DTable.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMArray2DTable
#define _H_CMArray2DTable

#include <JXStringTable.h>

class JXMenuBar;
class JXTextMenu;
class CMCommandDirector;
class CMArray2DDir;

class CMArray2DTable : public JXStringTable
{
public:

	CMArray2DTable(CMCommandDirector* cmdDir,
				   CMArray2DDir* arrayDir, JXMenuBar* menuBar,
				   JStringTableData* data,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CMArray2DTable();

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CMCommandDirector*	itsCmdDir;
	CMArray2DDir*		itsArrayDir;
	JXTextMenu*			itsEditMenu;
	JString				itsOrigEditValue;

private:

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);

	// not allowed

	CMArray2DTable(const CMArray2DTable& source);
	const CMArray2DTable& operator=(const CMArray2DTable& source);
};

#endif
