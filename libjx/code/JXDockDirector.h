/******************************************************************************
 JXDockDirector.h

	Interface for the JXDockDirector class

	Copyright (C) 2002-03 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDockDirector
#define _H_JXDockDirector

#include "JXWindowDirector.h"

class JXHorizDockPartition;
class JXVertDockPartition;
class JXDockWidget;

class JXDockDirector : public JXWindowDirector
{
public:

	JXDockDirector(const JString& title, const bool splitHoriz);
	JXDockDirector(std::istream& input, const JFileVersion vers, const JString& title);

	~JXDockDirector() override;

	bool	Close() override;

	bool	FindDock(const JIndex id, JXDockWidget** dock);

	bool	HasWindows() const;
	bool	CloseAllWindows();

	void	UpdateMinSize();

	void	StreamOut(std::ostream& output) const;

	bool	GetHorizPartition(JXHorizDockPartition** p) const;
	bool	GetVertPartition(JXVertDockPartition** p) const;

	// for use by JXWindow and JXDocktab

	bool	GetFocusWindow(JXWindow** window);
	void	SetFocusWindow(JXWindow* window);
	void	ClearFocusWindow(JXWindow* window);

private:

	JXHorizDockPartition*	itsHorizPartition;
	JXVertDockPartition*	itsVertPartition;
	JXWindow*				itsFocusWindow;

private:

	void	BuildWindow(const JString& title, const bool splitHoriz);
};

#endif
