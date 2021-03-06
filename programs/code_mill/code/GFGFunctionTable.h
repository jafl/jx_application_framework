/******************************************************************************
 GFGFunctionTable.h

	Interface for the GFGFunctionTable class

	Copyright (C) 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GFGFunctionTable
#define _H_GFGFunctionTable

// Superclass Header
#include <JXTable.h>

class GFGClass;

class GFGFunctionTable : public JXTable
{
public:

enum
{
	kFUsed	= 1,
	kFReturnType,
	kFFunctionName,
	kFConst,
	kFArgs
};

public:

	static GFGFunctionTable* Create(GFGClass* list,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	virtual ~GFGFunctionTable();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	GFGFunctionTable(GFGClass* list,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);

private:

	GFGClass*		itsList;	// we don't own this.
	JBoolean		itsNeedsAdjustment;

private:

	void	GFGFunctionTableX();
	void	AdjustColumnWidths();

	// not allowed

	GFGFunctionTable(const GFGFunctionTable& source);
	const GFGFunctionTable& operator=(const GFGFunctionTable& source);

};
#endif
