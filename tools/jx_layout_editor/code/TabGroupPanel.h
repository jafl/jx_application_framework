/******************************************************************************
 TabGroupPanel.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_TabGroupPanel
#define _H_TabGroupPanel

#include <jx-af/jcore/JBroadcaster.h>
#include "WidgetPanelBase.h"
#include <jx-af/jx/JXTabGroup.h>	// need defn of Edge

class JStringTableData;
class JXTextButton;
class TabTitleTable;

class TabGroupPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	TabGroupPanel(WidgetParametersDialog* dlog,
				  const JPtrArray<JString>& titles, const JXTabGroup::Edge edge);

	~TabGroupPanel();

	void	GetValues(JPtrArray<JString>* titles, JXTabGroup::Edge* edge);

private:

	JStringTableData*	itsData;
	JIndex				itsEdgeIndex;

// begin Panel

	TabTitleTable* itsTable;
	JXTextButton*  itsAddRowButton;
	JXTextButton*  itsRemoveRowButton;
	JXTextMenu*    itsEdgeMenu;

// end Panel

private:

	void	UpdateDisplay() const;
};

#endif
