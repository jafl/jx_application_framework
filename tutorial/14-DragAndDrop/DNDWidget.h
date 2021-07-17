/******************************************************************************
 DNDWidget.h

	Interface for the DNDWidget class

	Written by Glenn Bach - 2000.

 ******************************************************************************/

#ifndef _H_DNDWidget
#define _H_DNDWidget

#include <JXScrollableWidget.h>
#include <JXSelectionManager.h>

#include <JArray.h>

class DNDWidget : public JXScrollableWidget
{
public:

	DNDWidget(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~DNDWidget();

	virtual void	HandleKeyPress(const int key,				
								   const JXKeyModifiers& modifiers) override;
	
protected:
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JCharacter* id) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;

	virtual void 	HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void 	HandleDNDLeave() override;
	virtual void 	HandleDNDDrop(	const JPoint& pt, const JArray<Atom>& typeList,
									const Atom 	action, const Time time, 
									const JXWidget* source);
	

private:

	// The atom that X uses to distinguish our data.
	Atom	itsLinesXAtom;

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;
	
	JArray<JPoint>* itsPoints;
	
private:

	// not allowed

	DNDWidget(const DNDWidget& source);
	const DNDWidget& operator=(const DNDWidget& source);
};

#endif
