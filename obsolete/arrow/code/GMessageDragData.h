/******************************************************************************
 GMessageDragData.h

	Interface for the GMessageDragData class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMessageDragData
#define _H_GMessageDragData

#include <JXSelectionManager.h>
#include <JPtrArray.h>

class GMessageTableDir;
class GMessageHeader;

class GMessageDragData : public JXSelectionData
{
public:

	GMessageDragData(JXWidget* widget, const JCharacter* id);
	virtual ~GMessageDragData();

	void	SetDirector(GMessageTableDir* dir);
	void	SetHeaders(JPtrArray<GMessageHeader>* headers);

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	GMessageTableDir*			itsDir;			// We don't own this.
	JPtrArray<GMessageHeader>*	itsHeaders;		// We don't own the headers.
	Atom						itsMessageXAtom;

private:

	// not allowed

	GMessageDragData(const GMessageDragData& source);
	const GMessageDragData& operator=(const GMessageDragData& source);

public:
};

#endif
