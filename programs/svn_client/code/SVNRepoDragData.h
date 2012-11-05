/******************************************************************************
 SVNRepoDragData.h

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNRepoDragData
#define _H_SVNRepoDragData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionManager.h>

class SVNRepoDragData : public JXSelectionData
{
public:

	SVNRepoDragData(JXDisplay* display, const Atom type, const JCharacter* uri);

	virtual	~SVNRepoDragData();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	Atom	itsType;
	JString	itsURI;

private:

	// not allowed

	SVNRepoDragData(const SVNRepoDragData& source);
	const SVNRepoDragData& operator=(const SVNRepoDragData& source);
};

#endif
