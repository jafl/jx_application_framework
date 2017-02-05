/******************************************************************************
 TestLinkedDocument.h

	Interface for the TestLinkedDocument class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestLinkedDocument
#define _H_TestLinkedDocument

#include "TestTextEditDocument.h"

class TestLinkedDocument : public TestTextEditDocument
{
public:

	TestLinkedDocument(JXDirector* supervisor);

	virtual ~TestLinkedDocument();

	virtual JBoolean	NeedDocument(JXDocument* doc) const;

private:

	TestTextEditDocument*	itsDoc;

private:

	// not allowed

	TestLinkedDocument(const TestLinkedDocument& source);
	const TestLinkedDocument& operator=(const TestLinkedDocument& source);
};

#endif
