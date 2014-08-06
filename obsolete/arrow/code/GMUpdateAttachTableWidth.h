/******************************************************************************
 GMUpdateAttachTableWidth.h

	Interface for the GMUpdateAttachTableWidth class

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMUpdateAttachTableWidth
#define _H_GMUpdateAttachTableWidth

#include <JXUrgentTask.h>

class GMAttachmentTable;

class GMUpdateAttachTableWidth : public JXUrgentTask
{
public:

	GMUpdateAttachTableWidth(GMAttachmentTable* table);

	virtual ~GMUpdateAttachTableWidth();

	virtual void	Perform();

private:

	GMAttachmentTable*	itsTable;		// we don't own this

private:

	// not allowed

	GMUpdateAttachTableWidth(const GMUpdateAttachTableWidth& source);
	const GMUpdateAttachTableWidth& operator=(const GMUpdateAttachTableWidth& source);
};

#endif
