/******************************************************************************
 Partition.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_Partition
#define _H_Partition

#include "MultiContainerWidget.h"

class JXPartition;
class PartitionPanel;

class Partition : public MultiContainerWidget
{
public:

	enum Type
	{
		kHorizType,
		kVertType
	};

public:

	Partition(const Type type, LayoutContainer* layout,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);
	Partition(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~Partition() override;

	void	StreamOut(std::ostream& output) const override;
	JString	GetEnclosureName(const LayoutWidget* widget) const override;

protected:

	JString	GetClassName() const override;
	void	PrintCtorArgsWithComma(std::ostream& output,
								   const JString& varName,
								   JStringManager* stringdb) const override;

	void	AddPanels(WidgetParametersDialog* dlog) override;
	void	SavePanelData() override;

private:

	Type			itsType;
	JXPartition*	itsPartition;
	PartitionPanel*	itsPanel;
};

#endif
