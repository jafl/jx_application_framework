/******************************************************************************
 JX2DCurveNameList.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JX2DCurveNameList
#define _H_JX2DCurveNameList

#include <JXEditTable.h>
#include "J2DCurveInfo.h"

class JXInputField;

class JX2DCurveNameList : public JXEditTable
{
public:

	JX2DCurveNameList(const JArray<J2DCurveInfo>& curveInfo,
						const JIndex startIndex,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	virtual ~JX2DCurveNameList();

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual void			PrepareDeleteXInputField() override;
	virtual JBoolean		ExtractInputData(const JPoint& cell) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JPtrArray<JString>*	itsNameList;	// we don't own the strings
	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;

private:

	void	AdjustColWidth();

	// not allowed

	JX2DCurveNameList(const JX2DCurveNameList& source);
	const JX2DCurveNameList& operator=(const JX2DCurveNameList& source);

public:

	static const JUtf8Byte* kNameSelected;

	class NameSelected : public JBroadcaster::Message
	{
	public:

		NameSelected(const JIndex index)
			:
			JBroadcaster::Message(kNameSelected),
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
