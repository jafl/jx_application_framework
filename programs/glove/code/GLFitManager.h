/******************************************************************************
 GLFitManager.h

	Interface for the GLFitManager class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLFitManager
#define _H_GLFitManager

#include "GLFitDescription.h"
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

	bool	FitIsRemovable(const JIndex index);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	JPtrArray<GLFitDescription>* itsFitDescriptions;

	bool		itsIsInitialized;

private:

	void	InitializeList();

	// not allowed

	GLFitManager(const GLFitManager& source);
	const GLFitManager& operator=(const GLFitManager& source);

public:

	static const JUtf8Byte* kFitsChanged;

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
