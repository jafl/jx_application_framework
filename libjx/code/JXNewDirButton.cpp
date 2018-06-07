/******************************************************************************
 JXNewDirButton.cpp

	BASE CLASS = JXTextButton

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JXNewDirButton.h>
#include <JXFontManager.h>
#include <JXWindowPainter.h>
#include <JXImage.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

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
	JXTextButton(JString::empty, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsFolderIcon = jnew JXImage(GetDisplay(), jx_folder_small);
	assert( itsFolderIcon != nullptr );
	itsFolderIcon->ConvertToRemoteStorage();

	const JSize spaceWidth = GetFont().GetCharWidth(GetFontManager(), ' ');
	const JSize spaceCount = JRound(itsFolderIcon->GetWidth() / (JFloat) spaceWidth);
	JString s = JGetString("Label::JXNewDirButton");
	for (JIndex i=1; i<=spaceCount; i++)
		{
		s.Append(" ");
		}
	SetLabel(s);
	itsLabelWidth = GetFont().GetStringWidth(GetFontManager(), s);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXNewDirButton::~JXNewDirButton()
{
	jdelete itsFolderIcon;
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
