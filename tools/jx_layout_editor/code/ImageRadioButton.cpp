/******************************************************************************
 ImageRadioButton.cpp

	BASE CLASS = LayoutWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageRadioButton.h"
#include "RadioButtonIDPanel.h"
#include "ImageChooserPanel.h"
#include "LayoutContainer.h"
#include "ImageCache.h"
#include "globals.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageRadioButton::ImageRadioButton
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LayoutWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
}

ImageRadioButton::ImageRadioButton
	(
	const JString&		id,
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
	itsID(id)
{
}

ImageRadioButton::ImageRadioButton
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
	LayoutWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	input >> itsID;

	itsImageFullName = ImageChooserPanel::ReadImageName(input, GetParentContainer()->GetDocument());
}

// private

void
ImageRadioButton::ImageRadioButtonX()
{
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageRadioButton::~ImageRadioButton()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ImageRadioButton::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ImageRadioButton") << std::endl;

	LayoutWidget::StreamOut(output);

	output << itsID << std::endl;

	ImageChooserPanel::WriteImageName(itsImageFullName, output);
}

/******************************************************************************
 ToString (virtual)

 ******************************************************************************/

JString
ImageRadioButton::ToString()
	const
{
	JString s = LayoutWidget::ToString();

	s += JString::newline;
	s += JGetString("IDLabel::TextRadioButton");
	s += itsID;

	return s;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ImageRadioButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXImage* image;
	if (!itsImageFullName.IsEmpty() &&
		GetImageCache()->GetImage(itsImageFullName, &image))
	{
		p.Image(*image, image->GetBounds(), GetBounds());
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ImageRadioButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawUpFrame(p, frame, kJXDefaultBorderWidth);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ImageRadioButton::GetClassName()
	const
{
	return "JXImageRadioButton";
}

/******************************************************************************
 PrintCtorArgsWithComma (virtual protected)

 ******************************************************************************/

void
ImageRadioButton::PrintCtorArgsWithComma
	(
	std::ostream&	output,
	const JString&	varName,
	JStringManager* stringdb
	)
	const
{
	itsID.Print(output);
	output << ", ";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
ImageRadioButton::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (!itsImageFullName.IsEmpty())
	{
		const JString name = ImageCache::PrintInclude(itsImageFullName, output);

		indent.Print(output);
		varName.Print(output);
		output << "->SetXPM(";
		name.Print(output);
		output << ");" << std::endl;
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
ImageRadioButton::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsIDPanel    = jnew RadioButtonIDPanel(dlog, itsID);
	itsImagePanel = jnew ImageChooserPanel(dlog, GetParentContainer()->GetDocument(),
											itsImageFullName);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
ImageRadioButton::SavePanelData()
{
	itsIDPanel->GetValues(&itsID);
	itsImagePanel->GetValues(&itsImageFullName);

	itsIDPanel    = nullptr;
	itsImagePanel = nullptr;
}
