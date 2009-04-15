/******************************************************************************
 JXImageSelection.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageSelection
#define _H_JXImageSelection

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionData.h>

class JXImage;
class JXColormap;

class JXImageSelection : public JXSelectionData
{
public:

	JXImageSelection(const JXImage& image);
	JXImageSelection(JXImage* image);
	JXImageSelection(JXWidget* widget, const JCharacter* id);

	virtual	~JXImageSelection();

	JBoolean	GetImage(const JXImage** image) const;

	void	SetData(const JXImage& image);
	void	SetData(JXImage* image);

	static JBoolean	GetImage(const Atom selectionName, const Time time,
							 JXDisplay* display,
							 JXImage** image, const JBoolean allowApproxColors = kJTrue);
	static JBoolean	GetImage(JXSelectionManager* selMgr,
							 const Atom selectionName, const Time time,
							 JXColormap* colormap, JXImage** image,
							 const JBoolean allowApproxColors = kJTrue);

	static const JCharacter*	GetXPMXAtomName();
	static const JCharacter*	GetGIFXAtomName();
	static const JCharacter*	GetPNGXAtomName();
	static const JCharacter*	GetJPEGXAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXImage*	itsImage;		// NULL until Resolve()
	Atom		itsXPMXAtom;
	Atom		itsGIFXAtom;
	Atom		itsPNGXAtom;
	Atom		itsJPEGXAtom;

private:

	// not allowed

	JXImageSelection(const JXImageSelection& source);
	const JXImageSelection& operator=(const JXImageSelection& source);
};


/******************************************************************************
 GetImage

 ******************************************************************************/

inline JBoolean
JXImageSelection::GetImage
	(
	const JXImage** image
	)
	const
{
	*image = itsImage;
	return JI2B( itsImage != NULL );
}

#endif
