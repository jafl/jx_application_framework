/******************************************************************************
 NewDirButton.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_NewDirButton
#define _H_NewDirButton

#include "CoreWidget.h"

class JXNewDirButton;

class NewDirButton : public CoreWidget
{
public:

	enum Type
	{
		kTextType,
		kImageType
	};

public:

	NewDirButton(LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	NewDirButton(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	~NewDirButton() override;

	void	StreamOut(std::ostream& output) const override;

protected:

	JString	GetClassName() const override;

private:

	JXNewDirButton*	itsButton;

private:

	void	NewDirButtonX(const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);
};

#endif
