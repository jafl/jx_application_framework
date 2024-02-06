/******************************************************************************
 PartitionPanel.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_PartitionPanel
#define _H_PartitionPanel

#include <jx-af/jcore/JBroadcaster.h>
#include "WidgetPanelBase.h"

class JXIntegerInput;
class JXTextCheckbox;

class PartitionPanel : public WidgetPanelBase, public JBroadcaster
{
public:

	PartitionPanel(WidgetParametersDialog* dlog,
					const JArray<JCoordinate>& minSizes, const JIndex elasticIndex);

	~PartitionPanel();

	bool	Validate() const override;

	void	GetValues(JArray<JCoordinate>* minSizes, JIndex* elasticIndex);

private:

// begin Panel

	JXTextCheckbox* itsHasElasticIndexCB;
	JXIntegerInput* itsElasticIndexInput;

// end Panel

private:

	void	UpdateDisplay() const;
};

#endif
