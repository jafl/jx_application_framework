/******************************************************************************
 GLFitParmsDir.h

	Interface for the GLFitParmsDir class

	Copyright (C) 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLFitParmsDir
#define _H_GLFitParmsDir

#include <JXWindowDirector.h>
#include <JPtrArray.h>

class JXTextMenu;
class GLFitBase;
class JXTextButton;
class JXStaticText;
class GLFitParmsTable;
class GLPlotDir;

class GLFitParmsDir : public JXWindowDirector
{
public:

	GLFitParmsDir(GLPlotDir* supervisor, JPtrArray<GLFitBase>* fits);

	virtual ~GLFitParmsDir();
	void	ShowFit(const JIndex index);
	void	SendAllToSession();

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
private:

	GLPlotDir*					itsPlotDir;
	JXTextMenu*					itsFitMenu;
	JXTextButton*				itsCloseButton;
	JXTextButton*				itsSessionButton;
	GLFitParmsTable*				itsTable;
	JPtrArray<GLFitBase>*		itsFits;	// We don't own this!
	JIndex						itsCurrentIndex;
	
private:

	void				SendToSession(const JIndex index);
	void				HandleFitMenu(const JIndex index);
	void				UpdateFitMenu();

	// not allowed

	GLFitParmsDir(const GLFitParmsDir& source);
	const GLFitParmsDir& operator=(const GLFitParmsDir& source);
};

#endif
