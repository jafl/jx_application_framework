/******************************************************************************
 JXWindowDirector.h

	Interface for the JXWindowDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindowDirector
#define _H_JXWindowDirector

#include <JXDirector.h>

class JString;
class JXImage;
class JXDisplay;
class JXWindow;
class JXColormap;

class JXWindowDirector : public JXDirector
{
	friend class JXWindow;

public:

	JXWindowDirector(JXDirector* supervisor);

	virtual ~JXWindowDirector();

	virtual void		Activate();
	virtual JBoolean	Deactivate();

	virtual void	Suspend() override;
	virtual void	Resume() override;

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

private:

	JXWindow*	itsWindow;

private:

	void	SetWindow(JXWindow* window);

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
