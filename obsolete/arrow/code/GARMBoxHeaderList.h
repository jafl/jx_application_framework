/******************************************************************************
 GARMBoxHeaderList.h

	Interface for the GARMBoxHeaderList class.

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GARMBoxHeaderList
#define _H_GARMBoxHeaderList

// Superclass Header
#include <GMessageHeader.h>
#include <JPtrArray-JString.h>

class JString;
class JProgressDisplay;

class GARMBoxHeaderList : public JPtrArray<GMessageHeader>
{
public:

	static JBoolean OkToCreate(const JString& mailfile);
	static JBoolean Create(const JString& mailfile, GARMBoxHeaderList** list, JProgressDisplay* pg = NULL);
	GARMBoxHeaderList(const JString& mailfile, JProgressDisplay* pg = NULL);
	GARMBoxHeaderList();
	virtual ~GARMBoxHeaderList();

private:

	JString		itsMailFile;
	JBoolean	itsIsWritable;

private:

	// not allowed

	GARMBoxHeaderList(const GARMBoxHeaderList& source);
	const GARMBoxHeaderList& operator=(const GARMBoxHeaderList& source);
};

#endif
