/******************************************************************************
 JXToolBarEditWidget.h

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXToolBarEditWidget
#define _H_JXToolBarEditWidget

#include "JXNamedTreeListWidget.h"

class JNamedTreeList;
class JTree;
class JNamedTreeNode;
class JXImage;

class JXToolBarEditWidget : public JXNamedTreeListWidget
{
public:

	JXToolBarEditWidget(JNamedTreeList* treeList,
						JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	~JXToolBarEditWidget() override;

protected:

	bool	GetImage(const JIndex index, const JXImage** image) const override;
	void	HandleMouseDown(const JPoint& pt,
									const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	// not owned

	JXImage*	itsMenuImage;
	JXImage*	itsCheckedItemImage;
	JXImage*	itsUncheckedItemImage;
};

#endif
