/******************************************************************************
 THX3DPlotWidget.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THX3DPlotWidget
#define _H_THX3DPlotWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JX3DWidget.h>

class J3DUniverse;
class J3DAxes;
class J3DSurface;
class JXMesaCamera;

class THX3DPlotWidget : public JX3DWidget
{
public:

	THX3DPlotWidget(J3DUniverse* universe, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~THX3DPlotWidget();

	void	PlotData(const JSize xCount, const JSize yCount, JMatrix* data);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

private:

	JXMesaCamera*	itsMesaCamera;
	J3DAxes*		itsAxes;
	J3DSurface*		itsSurface;

private:

	void	AdjustFarZ();

	// not allowed

	THX3DPlotWidget(const THX3DPlotWidget& source);
	const THX3DPlotWidget& operator=(const THX3DPlotWidget& source);
};

#endif
