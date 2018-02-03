/******************************************************************************
 GMDirectorTimerTask.h

	Interface for the GMDirectorTimerTask class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMDirectorTimerTask
#define _H_GMDirectorTimerTask


#include <JXTimerTask.h>

class GMDirectorMenu;
class GMDirectorManager;

class GMDirectorTimerTask : public JXTimerTask
{
public:

	GMDirectorTimerTask(GMDirectorManager* mgr, GMDirectorMenu* menu);
	virtual ~GMDirectorTimerTask();

	GMDirectorMenu*	GetMenu();

protected:

	virtual void Receive(JBroadcaster* sender, const Message& message);

private:

	GMDirectorManager*	itsMgr;
	GMDirectorMenu*		itsMenu;

private:

	// not allowed

	GMDirectorTimerTask(const GMDirectorTimerTask& source);
	const GMDirectorTimerTask& operator=(const GMDirectorTimerTask& source);

public:

	static const JCharacter* kDirMenuTimerWentOff;

	class DirMenuTimerWentOff : public JBroadcaster::Message
		{
		public:

			DirMenuTimerWentOff(GMDirectorMenu* menu)
				:
				JBroadcaster::Message(kDirMenuTimerWentOff),
				itsMenu(menu)
				{ };

			GMDirectorMenu*
			GetMenu()
				const
				{
				return itsMenu;
				}

		private:

			GMDirectorMenu* itsMenu;
		};
};

/******************************************************************************
 GetMenu (public

 ******************************************************************************/

inline GMDirectorMenu*
GMDirectorTimerTask::GetMenu()
{
	return itsMenu;
}


#endif
