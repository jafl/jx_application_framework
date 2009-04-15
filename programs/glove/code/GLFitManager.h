/******************************************************************************
 GLFitManager.h

	Interface for the GLFitManager class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GLFitManager
#define _H_GLFitManager


#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GLFitDescription.h>

#include <JPrefObject.h>
#include <JBroadcaster.h>
#include <JPtrArray.h>

class GLFitManager : public JPrefObject, virtual public JBroadcaster
{
public:

public:

	GLFitManager();
	virtual ~GLFitManager();

	JSize					GetFitCount() const;
	const GLFitDescription&	GetFitDescription(const JIndex index) const;
	GLFitDescription&		GetFitDescription(const JIndex index);
	
	void	AddFitDescription(const GLFitDescription& fit);
	void	AddFitDescription(GLFitDescription* fit);
	void	NewFitDescription(const GLFitDescription::FitType type);
	void	RemoveFitDescription(const JIndex index);

	JBoolean	FitIsRemovable(const JIndex index);

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	JPtrArray<GLFitDescription>* itsFitDescriptions;

	JBoolean		itsIsInitialized;

private:

	void	InitializeList();

	// not allowed

	GLFitManager(const GLFitManager& source);
	const GLFitManager& operator=(const GLFitManager& source);

public:

	static const JCharacter* kFitsChanged;

	class FitsChanged : public JBroadcaster::Message
		{
		public:

			FitsChanged()
				:
				JBroadcaster::Message(kFitsChanged)
				{ };
		};
};

#endif
