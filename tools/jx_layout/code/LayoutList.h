/******************************************************************************
 LayoutList.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutList
#define _H_LayoutList

#include <jx-af/jx/JXStringList.h>

class MainDocument;

class LayoutList : public JXStringList
{

public:

	LayoutList(MainDocument* doc,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~LayoutList() override;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

private:

	MainDocument*	itsDoc;

private:

	void	OpenSelectedLayouts() const;
	void	OpenLayout(const JIndex index) const;
};

#endif
