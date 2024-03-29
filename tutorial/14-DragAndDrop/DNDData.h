/******************************************************************************
 DNDData.h

	Interface for the DNDData class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_DNDData
#define _H_DNDData

#include <jx-af/jx/JXSelectionData.h>
#include <jx-af/jcore/JPoint.h>

class DNDData : public JXSelectionData
{
public:

	DNDData(JXWidget* widget, const JUtf8Byte* id);

	~DNDData() override;

	void	SetData(const JArray<JPoint>& points);

	static const JUtf8Byte* GetDNDAtomName();

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	// Our point array.
	JArray<JPoint>*		itsPoints;

	// The atom that X uses to distinguish our data.
	Atom				itsLinesXAtom;

	// We use this to store the compacted data that X requests
	// when someone performs a paste. By storing it, we only have to
	// calculate it once.
	JString*			itsBuffer;

private:

	void	CreateBuffer() const;
};

#endif
