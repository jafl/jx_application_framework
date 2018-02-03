/******************************************************************************
 GXTextSelection.h

	Interface for the GXTextSelection class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GXTextSelection
#define _H_GXTextSelection


#include <JXTextSelection.h>
#include <JString.h>


class GXTextSelection : public JXTextSelection
{
public:

	GXTextSelection(JXDisplay* display, const std::string& text);
	virtual ~GXTextSelection();

	void	SetGloveData(const std::string& text);

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	Atom	itsGloveTextXAtom;
	JString	itsGloveData;

private:

	// not allowed

	GXTextSelection(const GXTextSelection& source);
	const GXTextSelection& operator=(const GXTextSelection& source);

public:
};

#endif
