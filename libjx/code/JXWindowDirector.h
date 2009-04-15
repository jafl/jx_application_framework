/******************************************************************************
 JXWindowDirector.h

	Interface for the JXWindowDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWindowDirector
#define _H_JXWindowDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirector.h>

class JString;
class JXImage;
class JXDisplay;
class JXWindow;
class JXColormap;

class JXWindowDirector : public JXDirector
{
public:

	JXWindowDirector(JXDirector* supervisor);

	virtual ~JXWindowDirector();

	virtual void		Activate();
	virtual JBoolean	Deactivate();

	virtual void	Suspend();
	virtual void	Resume();

	JXWindow*	GetWindow() const;
	JXDisplay*	GetDisplay() const;
	JXColormap*	GetColormap() const;

	// used by JXWDManager

	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;
	virtual JBoolean		NeedsSave() const;

	// needed by JXDialogDirector until dynamic_cast works

	virtual JBoolean	IsWindowDirector() const;

protected:

	virtual JBoolean	OKToDeactivate();

	void	SetWindow(JXWindow* window);

private:

	JXWindow*	itsWindow;

private:

	void	PrivateSuspend();

	// not allowed

	JXWindowDirector(const JXWindowDirector& source);
	const JXWindowDirector& operator=(const JXWindowDirector& source);
};

/******************************************************************************
 GetWindow

 ******************************************************************************/

inline JXWindow*
JXWindowDirector::GetWindow()
	const
{
	return itsWindow;
}

#endif
