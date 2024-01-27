/******************************************************************************
 ImageWidget.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageWidget.h"
#include "ImageChooserPanel.h"
#include "LayoutDocument.h"
#include "LayoutContainer.h"
#include "ImageCache.h"
#include "globals.h"
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageWidget::ImageWidget
	(
	const Type			type,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(false, layout, hSizing, vSizing, x,y, w,h),
	itsType(type)
{
	ImageWidgetX();
}

ImageWidget::ImageWidget
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(input, vers, layout, hSizing, vSizing, x,y, w,h),
	itsType(kImageType)
{
	ImageWidgetX();

	if (vers >= 4)
	{
		int type;
		input >> type;
		itsType = (Type) type;
	}

	itsFullName = ImageChooserPanel::ReadImageName(input, GetParentContainer()->GetDocument());
}

// private

void
ImageWidget::ImageWidgetX()
{
	if (itsType != kImageType)
	{
		SetBorderWidth(kJXDefaultBorderWidth);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageWidget::~ImageWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ImageWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ImageWidget") << std::endl;

	LayoutWidget::StreamOut(output);

	output << (int) itsType << std::endl;
	ImageChooserPanel::WriteImageName(itsFullName, output);
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ImageWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXImage* image;
	if (!itsFullName.IsEmpty() &&
		GetImageCache()->GetImage(itsFullName, &image))
	{
		p.Image(*image, image->GetBounds(), GetBounds());
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ImageWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (itsType != kImageType)
	{
		JXDrawUpFrame(p, frame, kJXDefaultBorderWidth);
	}
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ImageWidget::GetClassName()
	const
{
	return (itsType == kButtonType   ? "JXImageButton" :
			itsType == kCheckboxType ? "JXImageCheckbox" :
			"JXImageWidget");
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
ImageWidget::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (!itsFullName.IsEmpty())
	{
		const JString name = ImageCache::PrintInclude(itsFullName, output);

		indent.Print(output);
		varName.Print(output);
		output << "->SetImage(GetDisplay()->GetImageCache()->GetImage(";
		name.Print(output);
		output << "), false);" << std::endl;
	}
	else
	{
		LayoutWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
ImageWidget::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew ImageChooserPanel(dlog, GetParentContainer()->GetDocument(),
									  itsFullName);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
ImageWidget::SavePanelData()
{
	itsPanel->GetValues(&itsFullName);
	itsPanel = nullptr;
}
