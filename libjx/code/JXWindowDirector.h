/******************************************************************************
 JXWindowDirector.h

	Interface for the JXWindowDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindowDirector
#define _H_JXWindowDirector

#include "JXDirector.h"

class JString;
class JXImage;
class JXDisplay;
class JXWindow;

class JXWindowDirector : public JXDirector
{
	friend class JXWindow;

public:

	JXWindowDirector(JXDirector* supervisor);

	virtual ~JXWindowDirector();

	virtual void	Activate() override;
	virtual bool	Deactivate() override;

	virtual void	Suspend() override;
	virtual void	Resume() override;

	JXWindow*	GetWindow() const;
	JXDisplay*	GetDisplay() const;

	// used by JXWDManager

	virtual const JString&	GetName() const;
	virtual bool			GetMenuIcon(const JXImage** icon) const;
	virtual bool			NeedsSave() const;

	// needed by JXDialogDirector until dynamic_cast works

	virtual bool	IsWindowDirector() const override;

protected:

	virtual bool	OKToDeactivate();

private:

	JXWindow*	itsWindow;

private:

	void	SetWindow(JXWindow* window);
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
