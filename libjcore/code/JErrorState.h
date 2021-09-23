/******************************************************************************
 JErrorState.h

	Interface for the JErrorState class

	Copyright (C) 1997 by Dustin Laurence.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JErrorState
#define _H_JErrorState

#include "JError.h"

class JErrorState
{
public:

	JErrorState();
	virtual ~JErrorState();

	virtual bool   OK() const;
	virtual JError Error() const;
	virtual void   ClearError();

protected:

	virtual void SetError(const JError& error);

	virtual void MutableClearError() const; // Nasty!
	virtual void MutableSetError(const JError& error) const; // Nasty!

private:

	JError itsError;
};

#endif
