/******************************************************************************
 DNDData.h

	Interface for the DNDData class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_DNDData
#define _H_DNDData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionManager.h>

#include <JArray.h>
#include <JPoint.h>

class DNDData : public JXSelectionData
{
public:

	DNDData(JXWidget* widget, const JCharacter* id);
	virtual ~DNDData();

	void	SetData(const JArray<JPoint>& points);

	static const JCharacter* GetDNDAtomName();

protected:

			virtual void		AddTypes(const Atom selectionName);
			virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
											unsigned char** data, JSize* dataLength,
											JSize* bitsPerBlock) const;
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

	// not allowed

	DNDData(const DNDData& source);
	const DNDData& operator=(const DNDData& source);

public:
};

#endif
