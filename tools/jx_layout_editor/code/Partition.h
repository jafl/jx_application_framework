/******************************************************************************
 Partition.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_Partition
#define _H_Partition

#include "MultiContainerWidget.h"

class JXPartition;
class PartitionPanel;
class LayoutUndo;

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

	void	PrepareToGenerateCode(std::ostream& output, const JString& indent,
								  JStringManager* stringdb) const override;

protected:

	bool	StealMouse(const int eventType, const JPoint& ptG,
					   const JXMouseButton button,
					   const unsigned int state) override;

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
	LayoutUndo*		itsUndo;

private:

	void	PartitionX();
};

#endif
