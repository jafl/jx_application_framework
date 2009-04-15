/******************************************************************************
 JXNewDirButton.cpp

	BASE CLASS = JXTextButton

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXNewDirButton.h>
#include <JXFontManager.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <JString.h>
#include <jMath.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

static const JCharacter* kLabel = "New  ";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXNewDirButton::JXNewDirButton
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextButton("", enclosure, hSizing, vSizing, x,y, w,h)
{
	itsFolderIcon = new JXImage(GetDisplay(), GetColormap(), jx_folder_small);
	assert( itsFolderIcon != NULL );
	itsFolderIcon->ConvertToRemoteStorage();

	const JSize spaceWidth =
		(GetFontManager())->GetCharWidth(GetFontName(), GetFontSize(),
										 GetFontStyle(), ' ');
	const JSize spaceCount = JRound(itsFolderIcon->GetWidth() / (JFloat) spaceWidth);
	JString s = kLabel;
	for (JIndex i=1; i<=spaceCount; i++)
		{
		s.AppendCharacter(' ');
		}
	SetLabel(s);
	itsLabelWidth =
		(GetFontManager())->GetStringWidth(GetFontName(), GetFontSize(),
										   GetFontStyle(), s);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXNewDirButton::~JXNewDirButton()
{
	delete itsFolderIcon;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXNewDirButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXTextButton::Draw(p, rect);

	const JRect bounds = GetBounds();
	JRect r            = itsFolderIcon->GetBounds();
	r.Shift(-r.left + (bounds.width() + itsLabelWidth)/2 - r.width(),
			-r.top  + (bounds.height() - r.height())/2);
	p.Image(*itsFolderIcon, itsFolderIcon->GetBounds(), r);
}
