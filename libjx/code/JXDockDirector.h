/******************************************************************************
 JXDockDirector.h

	Interface for the JXDockDirector class

	Copyright © 2002-03 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDockDirector
#define _H_JXDockDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXHorizDockPartition;
class JXVertDockPartition;
class JXDockWidget;

class JXDockDirector : public JXWindowDirector
{
public:

	JXDockDirector(const JCharacter* title, const JBoolean splitHoriz);
	JXDockDirector(istream& input, const JFileVersion vers, const JCharacter* title);

	virtual ~JXDockDirector();

	virtual JBoolean	Close();

	JBoolean	FindDock(const JIndex id, JXDockWidget** dock);

	JBoolean	HasWindows() const;
	JBoolean	CloseAllWindows();

	void	UpdateMinSize();

	void	StreamOut(ostream& output) const;

	JBoolean	GetHorizPartition(JXHorizDockPartition** p) const;
	JBoolean	GetVertPartition(JXVertDockPartition** p) const;

	// for use by JXWindow and JXDocktab

	JBoolean	GetFocusWindow(JXWindow** window);
	void		SetFocusWindow(JXWindow* window);
	void		ClearFocusWindow(JXWindow* window);

private:

	JXHorizDockPartition*	itsHorizPartition;
	JXVertDockPartition*	itsVertPartition;
	JXWindow*				itsFocusWindow;

private:

	void	BuildWindow(const JCharacter* title, const JBoolean splitHoriz);

	// not allowed

	JXDockDirector(const JXDockDirector& source);
	const JXDockDirector& operator=(const JXDockDirector& source);
};

#endif
