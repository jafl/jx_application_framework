/******************************************************************************
 GMDirectorManager.cc

	BASE CLASS = public JBroadcaster

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMDirectorManager.h>
#include <GMDirectorMenu.h>
#include <GMDirectorTimerTask.h>

#include <JXApplication.h>
#include <GMManagedDirector.h>
#include <JXWindow.h>

#include <GMGlobals.h>

#include <jAssert.h>

const JCharacter* GMDirectorManager::kDirectorListChanged = "kDirectorListChanged::GMDirectorManager";

const JSize kGMWindowSeparatedIndex = 4;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMDirectorManager::GMDirectorManager()
{
	itsDirectors = new JPtrArray<GMManagedDirector>(JPtrArrayT::kForgetAll);
	assert(itsDirectors != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMDirectorManager::~GMDirectorManager()
{
	delete itsDirectors;
}

/******************************************************************************
 DirectorCreated (public)

 ******************************************************************************/

void
GMDirectorManager::DirectorCreated
	(
	GMManagedDirector* dir
	)
{
	JIndex findex;
	if (!itsDirectors->Find(dir, &findex))
		{
		itsDirectors->Append(dir);
		Broadcast(DirectorListChanged());
		}
}

/******************************************************************************
 DirectorDeleted (public)

 ******************************************************************************/

void
GMDirectorManager::DirectorDeleted
	(
	GMManagedDirector* dir
	)
{
	JIndex findex;
	if (itsDirectors->Find(dir, &findex))
		{
		itsDirectors->Remove(dir);
		Broadcast(DirectorListChanged());
		}
}

/******************************************************************************
 DirectorChanged (public)

 ******************************************************************************/

void
GMDirectorManager::DirectorChanged
	(
	GMManagedDirector* dir
	)
{
	JIndex findex;
	if (itsDirectors->Find(dir, &findex))
		{
		Broadcast(DirectorListChanged());
		}
}

/******************************************************************************
 ActivateDirector (public)

 ******************************************************************************/

void
GMDirectorManager::ActivateDirector
	(
	const JIndex index
	)
{
	assert(itsDirectors->IndexValid(index));
	itsDirectors->NthElement(index)->Activate();
}

/******************************************************************************
 UpdateDirectorMenu (public)

 ******************************************************************************/

void
GMDirectorManager::UpdateDirectorMenu
	(
	GMDirectorMenu* menu
	)
{
	if (menu->IsOpen())
		{
		GMDirectorTimerTask* task =
			new GMDirectorTimerTask(this, menu);
		assert(task != NULL);
		ListenTo(task);
		JXGetApplication()->InstallIdleTask(task);
		return;
		}

	menu->RemoveAllItems();
	const JSize count = itsDirectors->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		GMManagedDirector* dir = itsDirectors->NthElement(i);
		const JString name = dir->GetWindow()->GetTitle();
		menu->AppendItem(name);

		menu->SetItemImage(i, dir->GetMenuIcon(), kJFalse);
		JString str;
		if (dir->GetShortcut(&str))
			{
			menu->SetItemNMShortcut(i, str);
			}
		if (dir->GetID(&str))
			{
			menu->SetItemID(i, str);
			}
		if (i == kGMWindowSeparatedIndex)
			{
			menu->ShowSeparatorAfter(i, kJTrue);
			}
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GMDirectorManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(GMDirectorTimerTask::kDirMenuTimerWentOff))
		{
		const GMDirectorTimerTask::DirMenuTimerWentOff* info =
			dynamic_cast(const GMDirectorTimerTask::DirMenuTimerWentOff*, &message);
		assert(info != NULL);
		UpdateDirectorMenu(info->GetMenu());
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
