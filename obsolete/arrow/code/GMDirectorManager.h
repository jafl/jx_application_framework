/******************************************************************************
 GMDirectorManager.h

	Interface for the GMDirectorManager class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMDirectorManager
#define _H_GMDirectorManager

#include <JPtrArray.h>

class GMManagedDirector;
class GMDirectorMenu;

class GMDirectorManager : public JBroadcaster
{
public:

public:

	GMDirectorManager();
	virtual ~GMDirectorManager();

	void	DirectorCreated(GMManagedDirector* dir);
	void	DirectorDeleted(GMManagedDirector* dir);
	void	DirectorChanged(GMManagedDirector* dir);

	void	ActivateDirector(const JIndex index);

	void	UpdateDirectorMenu(GMDirectorMenu* menu);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<GMManagedDirector>*	itsDirectors;

private:

	// not allowed

	GMDirectorManager(const GMDirectorManager& source);
	const GMDirectorManager& operator=(const GMDirectorManager& source);

public:

	static const JCharacter* kDirectorListChanged;

	class DirectorListChanged : public JBroadcaster::Message
		{
		public:

			DirectorListChanged()
				:
				JBroadcaster::Message(kDirectorListChanged)
				{ };
		};

};

#endif
