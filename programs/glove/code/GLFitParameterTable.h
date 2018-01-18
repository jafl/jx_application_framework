/******************************************************************************
 GLFitParameterTable.h

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLFitParameterTable
#define _H_GLFitParameterTable

#include <JXEditTable.h>
#include <J2DPlotWidget.h>

class GLParmColHeaderWidget;
class JXFloatInput;
class JPagePrinter;
class GLFitDescription;

class GLFitParameterTable : public JXEditTable
{
public:

	GLFitParameterTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GLFitParameterTable();

	void	SetColHeaderWidget(GLParmColHeaderWidget* widget);
	void	SetFitDescription(const GLFitDescription& fit);
	void	SetValue(const JIndex index, const JFloat value, const JFloat error);
	void	ClearValues();
	void	CopyParmValuesToStart();
	void	GetValueString(JString* text);

	void		ShowStartCol(const JBoolean show = kJTrue);
	JBoolean	IsShowingStartCol() const;

	const JArray<JFloat>&	GetStartValues() const;

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

	JBoolean		BeginEditingStartValues();
	void			PrintOnPage(JPagePrinter& p, JCoordinate* height);

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual void			PrepareDeleteXInputField();
	virtual JBoolean		ExtractInputData(const JPoint& cell);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JPtrArray<JString>*	itsNameList;	
	JArray<JFloat>*		itsStartValues;
	JArray<JFloat>*		itsFitValues;
	JArray<JFloat>*		itsErrorValues;
	JCoordinate			itsMinColWidth;
	JXFloatInput*		itsInput;
	JBoolean			itsHasStartValues;

	GLParmColHeaderWidget*	itsColHeaderWidget;

private:

	void	AdjustColWidth();
	void	AdjustColWidth(const JSize availabeWidth);

	// not allowed

	GLFitParameterTable(const GLFitParameterTable& source);
	const GLFitParameterTable& operator=(const GLFitParameterTable& source);

public:

	static const JCharacter* kValueChanged;

	class ValueChanged : public JBroadcaster::Message
	{
	public:

		ValueChanged(const JIndex index, const JFloat value)
			:
			JBroadcaster::Message(kValueChanged),
			itsIndex(index),
			itsValue(value)
			{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

		JFloat
		GetValue() const
		{
			return itsValue;
		};

	private:

		JIndex itsIndex;
		JFloat itsValue;
	};

};

/******************************************************************************
 IsShowingStartCol (public)

 ******************************************************************************/

inline JBoolean
GLFitParameterTable::IsShowingStartCol()
	const
{
	return itsHasStartValues;
}


#endif
