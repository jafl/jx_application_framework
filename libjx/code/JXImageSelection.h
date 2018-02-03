/******************************************************************************
 JXImageSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageSelection
#define _H_JXImageSelection

#include <JXSelectionData.h>

class JXImage;
class JXColormap;

class JXImageSelection : public JXSelectionData
{
public:

	JXImageSelection(const JXImage& image);
	JXImageSelection(JXImage* image);
	JXImageSelection(JXWidget* widget, const JUtf8Byte* id);

	virtual	~JXImageSelection();

	JBoolean	GetImage(const JXImage** image) const;

	void	SetData(const JXImage& image);
	void	SetData(JXImage* image);

	static JBoolean	GetImage(const Atom selectionName, const Time time,
							 JXDisplay* display, JXImage** image);
	static JBoolean	GetImage(JXSelectionManager* selMgr,
							 const Atom selectionName, const Time time,
							 JXColormap* colormap, JXImage** image);

	static const JUtf8Byte*	GetXPMXAtomName();
	static const JUtf8Byte*	GetGIFXAtomName();
	static const JUtf8Byte*	GetPNGXAtomName();
	static const JUtf8Byte*	GetJPEGXAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

public:		// kAtomCount required at global scope

	enum
	{
		kXPMAtomIndex,
		kGIFAtomIndex,
		kPNGAtomIndex,
		kJPEGAtomIndex,

		kAtomCount
	};

private:

	JXImage*	itsImage;		// NULL until Resolve()
	Atom		itsAtoms[ kAtomCount ];
	Atom		itsXPMAtom;
	Atom		itsGIFAtom;
	Atom		itsPNGAtom;
	Atom		itsJPEGAtom;

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
