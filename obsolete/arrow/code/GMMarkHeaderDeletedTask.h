/******************************************************************************
 GMMarkHeaderDeletedTask.h

	Interface for the GMMarkHeaderDeletedTask class

	Copyright © 2003 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GMMarkHeaderDeletedTask
#define _H_GMMarkHeaderDeletedTask

#include <JXUrgentTask.h>

class GMessageHeader;
class GMMailboxData;

class GMMarkHeaderDeletedTask : public JXUrgentTask
{
public:

	GMMarkHeaderDeletedTask(GMMailboxData* data, GMessageHeader* header);
	virtual ~GMMarkHeaderDeletedTask();

	virtual void	Perform();


protected:

private:

	GMMailboxData*		itsData;
	GMessageHeader*		itsHeader;

private:

	GMMarkHeaderDeletedTask(const GMMarkHeaderDeletedTask& source);
	const GMMarkHeaderDeletedTask& operator=(const GMMarkHeaderDeletedTask& source);


};

#endif
