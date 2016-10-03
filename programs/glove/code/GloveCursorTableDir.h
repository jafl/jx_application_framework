/******************************************************************************
 GloveCursorTableDir.h

	Interface for the GloveCursorTableDir class

	Copyright (C) 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GloveCursorTableDir
#define _H_GloveCursorTableDir

#include <JXWindowDirector.h>

class JXTextButton;
class GloveCursorTable;
class GlovePlotter;
class PlotDir;

class GloveCursorTableDir : public JXWindowDirector
{
public:

	GloveCursorTableDir(PlotDir* supervisor, GlovePlotter* plot);

	virtual ~GloveCursorTableDir();
	void 	SendToSession();

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	PlotDir*					itsPlotDir;
	JXTextButton*				itsCloseButton;
	JXTextButton*				itsSessionButton;
	GloveCursorTable*			itsTable;
	
private:

	// not allowed

	GloveCursorTableDir(const GloveCursorTableDir& source);
	const GloveCursorTableDir& operator=(const GloveCursorTableDir& source);
};

#endif
