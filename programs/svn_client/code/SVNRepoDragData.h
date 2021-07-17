/******************************************************************************
 SVNRepoDragData.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNRepoDragData
#define _H_SVNRepoDragData

#include <JXSelectionManager.h>

class SVNRepoDragData : public JXSelectionData
{
public:

	SVNRepoDragData(JXDisplay* display, const Atom type, const JString& uri);

	virtual	~SVNRepoDragData();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
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
