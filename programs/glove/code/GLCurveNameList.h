/******************************************************************************
 GLCurveNameList.h

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLCurveNameList
#define _H_GLCurveNameList

#include <JXEditTable.h>
#include <J2DPlotWidget.h>

class JXInputField;
class PlotDir;

class GLCurveNameList : public JXEditTable
{
public:

	GLCurveNameList(PlotDir* dir, J2DPlotWidget* plot,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GLCurveNameList();

	JBoolean		GetCurrentCurveIndex(JIndex* index);
	void			SetCurrentCurveIndex(const JIndex index);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

private:

	JPtrArray<JString>*	itsNameList;	// we don't own the strings
	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;
	J2DPlotWidget*		itsPlot;
	PlotDir*			itsDir;

private:

	void	AdjustColWidth();

	// not allowed

	GLCurveNameList(const GLCurveNameList& source);
	const GLCurveNameList& operator=(const GLCurveNameList& source);

public:

	static const JCharacter* kCurveSelected;

	class CurveSelected : public JBroadcaster::Message
	{
	public:

		CurveSelected(const JIndex index)
			:
			JBroadcaster::Message(kCurveSelected),
			itsIndex(index)
			{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JIndex itsIndex;
	};

};

#endif
