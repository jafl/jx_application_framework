/******************************************************************************
 FitParmsDir.h

	Interface for the FitParmsDir class

	Copyright © 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_FitParmsDir
#define _H_FitParmsDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPtrArray.h>

class JXTextMenu;
class JFitBase;
class JXTextButton;
class JXStaticText;
class FitParmsTable;
class PlotDir;

class FitParmsDir : public JXWindowDirector
{
public:

	FitParmsDir(PlotDir* supervisor, JPtrArray<JFitBase>* fits);

	virtual ~FitParmsDir();
	void	ShowFit(const JIndex index);
	void	SendAllToSession();

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	PlotDir*					itsPlotDir;
	JXTextMenu*					itsFitMenu;
	JXTextButton*				itsCloseButton;
	JXTextButton*				itsSessionButton;
	FitParmsTable*				itsTable;
	JPtrArray<JFitBase>*		itsFits;	// We don't own this!
	JIndex						itsCurrentIndex;
	
private:

	void				SendToSession(const JIndex index);
	void				HandleFitMenu(const JIndex index);
	void				UpdateFitMenu();

	// not allowed

	FitParmsDir(const FitParmsDir& source);
	const FitParmsDir& operator=(const FitParmsDir& source);
};

#endif
