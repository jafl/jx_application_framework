/******************************************************************************
 JXDocument.cpp

	Maintains some information and a window that displays it.

	Derived classes must override OKToClose so they can save data
	or otherwise let the user clean things up before closing.  (Clean up
	that doesn't involve the user should be done in the destructor.)

	OKToDeactivate() is automatically wired to OKToClose() on the assumption
	that the user can't tell the difference (and shouldn't have to, either!)
	between closing a document and merely hiding its window.

	Derived classes must also override the following function:

		NeedsSave
			Returns true if the data needs to be saved.

		OKToRevert
			Ask the user if it is OK to revert to the latest saved version
			of this document.

		CanRevert
			Returns true if it is possible to revert to a saved version
			of this document.

		SafetySave
			Save the data in a temporary file, in case the program crashes.
			No error reporting is allowed because one possibility is
			that the GUI itself has died.

		GetMenuIcon
			Return a small icon for the document.  This is used when building
			a menu of all the documents that are open.  JXFileDocument
			implements this to return jx_plain_file_small.

			The result is JXPM instead of JXImage because JXImage is
			restricted to a single display.

	Derived classes may want to override the following functions:

		GetName
			Return a name for the document.  This is used when building
			a menu of all the documents that are open, so it should be
			unique (though it doesn't have to be).  The default implementation
			is to return the title of our window.  JXFileDocument overrides
			this to return the name of its file.

		NeedDocument
			Return true if you need the given document to stay open.
			The document dependency graph must be acyclic.

	BASE CLASS = JXWindowDirector

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#include "JXDocument.h"
#include "JXDocumentManager.h"
#include "JXWDManager.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocument::JXDocument
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	(JXGetDocumentManager())->DocumentCreated(this);

	JXWDManager* mgr;
	if ((JXGetApplication()->GetCurrentDisplay())->GetWDManager(&mgr))
	{
		mgr->DirectorCreated(this);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocument::~JXDocument()
{
	(JXGetDocumentManager())->DocumentDeleted(this);
}

/******************************************************************************
 Close

	Returns true if the user ok's it and nobody needs us.

 ******************************************************************************/

bool
JXDocument::Close()
{
	if (!OKToClose())
	{
		return false;
	}

	if (JXGetDocumentManager()->OKToCloseDocument(this))
	{
		return JXWindowDirector::Close();
	}
	else
	{
		if (NeedsSave())
		{
			DiscardChanges();
		}
		Deactivate();
		return !IsActive();
	}
}

/******************************************************************************
 Deactivate (virtual)

	The user can't tell the difference between hiding and closing.

 ******************************************************************************/

bool
JXDocument::Deactivate()
{
	if (JXWindowDirector::Deactivate())
	{
		if (NeedsSave())
		{
			DiscardChanges();	// OKToDeactivate() returned true
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	The user can't tell the difference between hiding and closing.

 ******************************************************************************/

bool
JXDocument::OKToDeactivate()
{
	return OKToClose();
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
JXDocument::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = (JXGetDocumentManager())->GetDefaultMenuIcon();
	return true;
}

/******************************************************************************
 NeedsSave (virtual)

	Must be overridden, but can't be pure virtual because originally
	defined in JXWindowDirector.

 ******************************************************************************/

bool
JXDocument::NeedsSave()
	const
{
	assert_msg( 0, "programmer forgot to override JXDocument::NeedsSave()" );
	return false;
}

/******************************************************************************
 RevertToSaved

 ******************************************************************************/

void
JXDocument::RevertToSaved()
{
	if (CanRevert() && OKToRevert())
	{
		DiscardChanges();
	}
}

/******************************************************************************
 NeedDocument (virtual)

	Derived classes should override if they require that other documents
	must stay open.

	*** The dependency graph must be acyclic!

 ******************************************************************************/

bool
JXDocument::NeedDocument
	(
	JXDocument* doc
	)
	const
{
	return false;
}
