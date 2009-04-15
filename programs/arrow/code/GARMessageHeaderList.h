/******************************************************************************
 GARMessageHeaderList.h

	Interface for the GARMessageHeaderList class.

	Copyright © 2001 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GARMessageHeaderList
#define _H_GARMessageHeaderList

// Superclass Header
#include <GMessageHeader.h>
#include <JPtrArray-JString.h>

class JString;
class JProgressDisplay;

class GARMessageHeaderList : public JPtrArray<GMessageHeader>
{
public:

	static JBoolean OkToCreate(const JString& mailfile);
	static JBoolean Create(const JString& mailfile, GARMessageHeaderList** list, JProgressDisplay* pg = NULL);
	GARMessageHeaderList(const JString& mailfile, JProgressDisplay* pg = NULL);
	GARMessageHeaderList();
	virtual ~GARMessageHeaderList();

private:

	JString		itsMailFile;
	JBoolean	itsIsWritable;

private:

	// not allowed

	GARMessageHeaderList(const GARMessageHeaderList& source);
	const GARMessageHeaderList& operator=(const GARMessageHeaderList& source);
};

#endif
