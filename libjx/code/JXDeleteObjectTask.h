/******************************************************************************
 JXDeleteObjectTask.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDeleteObjectTask
#define _H_JXDeleteObjectTask

#include "JXUrgentTask.h"

template <class T>
class JXDeleteObjectTask : public JXUrgentTask
{
public:

	static void	Delete(T* obj);

protected:

	JXDeleteObjectTask(T* obj);

	~JXDeleteObjectTask() override;

	void	Perform() override;

private:

	T*	itsObject;
};

#include "JXDeleteObjectTask.tmpl"

#endif
