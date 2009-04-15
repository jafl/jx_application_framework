/******************************************************************************
 GMMessageDragSource.h

	Copyright © 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMMessageDragSource
#define _H_GMMessageDragSource

#include <JXImageWidget.h>
#include <JString.h>

class GMessageViewDir;
class JXInputField;

class GMMessageDragSource : public JXImageWidget
{
public:

	GMMessageDragSource(GMessageViewDir* dir,
					   JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~GMMessageDragSource();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	GetDNDAskActions(const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers,
									 JArray<Atom>* askActionList,
									 JPtrArray<JString>* askDescriptionList);

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JCharacter* id);

private:

	GMessageViewDir*	itsDir;
	Atom				itsMessageXAtom;

private:

	// not allowed

	GMMessageDragSource(const GMMessageDragSource& source);
	const GMMessageDragSource& operator=(const GMMessageDragSource& source);
};

#endif
