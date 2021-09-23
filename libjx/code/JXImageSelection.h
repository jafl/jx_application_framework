/******************************************************************************
 JXImageSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageSelection
#define _H_JXImageSelection

#include "JXSelectionData.h"

class JXImage;

class JXImageSelection : public JXSelectionData
{
public:

	JXImageSelection(const JXImage& image);
	JXImageSelection(JXImage* image);
	JXImageSelection(JXWidget* widget, const JUtf8Byte* id);

	virtual	~JXImageSelection();

	bool	GetImage(const JXImage** image) const;

	void	SetData(const JXImage& image);
	void	SetData(JXImage* image);

	static bool	GetImage(const Atom selectionName, const Time time,
						 JXDisplay* display, JXImage** image);
	static bool	GetImage(JXSelectionManager* selMgr,
						 const Atom selectionName, const Time time,
						 JXImage** image);

	static const JUtf8Byte*	GetXPMXAtomName();
	static const JUtf8Byte*	GetGIFXAtomName();
	static const JUtf8Byte*	GetPNGXAtomName();
	static const JUtf8Byte*	GetJPEGXAtomName();

protected:

	virtual void	AddTypes(const Atom selectionName) override;
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
								unsigned char** data, JSize* dataLength,
								JSize* bitsPerBlock) const override;

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

	JXImage*	itsImage;		// nullptr until Resolve()
	Atom		itsAtoms[ kAtomCount ];
	Atom		itsXPMAtom;
	Atom		itsGIFAtom;
	Atom		itsPNGAtom;
	Atom		itsJPEGAtom;
};


/******************************************************************************
 GetImage

 ******************************************************************************/

inline bool
JXImageSelection::GetImage
	(
	const JXImage** image
	)
	const
{
	*image = itsImage;
	return itsImage != nullptr;
}

#endif
