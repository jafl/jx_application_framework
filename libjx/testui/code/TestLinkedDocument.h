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

	~TestLinkedDocument() override;

	bool	NeedDocument(JXDocument* doc) const override;

private:

	TestTextEditDocument*	itsDoc;
};

#endif
