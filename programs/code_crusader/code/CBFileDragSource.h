/******************************************************************************
 CBFileDragSource.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileDragSource
#define _H_CBFileDragSource

#include <JXImageWidget.h>

class JXInputField;
class CBTextDocument;

class CBFileDragSource : public JXImageWidget
{
public:

	CBFileDragSource(CBTextDocument* doc, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CBFileDragSource();

	void	ProvideDirectSave(JXInputField* nameInput);

protected:

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	DNDInit(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

private:

	CBTextDocument*	itsDoc;			// owns us
	JXInputField*	itsNameInput;	// nullptr => dragging for XDND

private:

	// not allowed

	CBFileDragSource(const CBFileDragSource& source);
	const CBFileDragSource& operator=(const CBFileDragSource& source);
};


/******************************************************************************
 ProvideDirectSave

 ******************************************************************************/

inline void
CBFileDragSource::ProvideDirectSave
	(
	JXInputField* nameInput
	)
{
	itsNameInput = nameInput;
}

#endif
