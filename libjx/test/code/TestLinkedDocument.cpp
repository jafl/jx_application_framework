/******************************************************************************
 TestLinkedDocument.cpp

	Document to test JXApplication's document interaction services.
	Normally, one will store the paths and names of linked documents in the
	data file and then try to open them after reading one's own data.
	For this simple example, however, we hard code the file names and look for
	them in the current directory.

	BASE CLASS = TestTextEditDocument

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestLinkedDocument.h"
#include <JXDocumentManager.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JCharacter* kOurFile      = "about_owner_docs";
static const JCharacter* kRequiredFile = "about_owned_docs";

/******************************************************************************
 Constructor

 ******************************************************************************/

TestLinkedDocument::TestLinkedDocument
	(
	JXDirector* supervisor
	)
	:
	TestTextEditDocument(supervisor)
{
	itsDoc = NULL;

	if (JFileExists(kOurFile))
		{
		FileChanged(kOurFile, kJTrue);
		ReadFile(kOurFile);
		}
	else
		{
		(JGetUserNotification())->ReportError("Unable to find \"about_owner_docs\"");
		}

	// Normally, currDir would be relative to the directory containing our data file,
	// and fileName would come from our data file.

	const JString currDir  = JGetCurrentDirectory();
	const JString fileName = currDir + kRequiredFile;

	JXDocumentManager* docMgr = JXGetDocumentManager();

	JString newName;
	if (docMgr->FindFile(fileName, currDir, &newName))
		{
		JXFileDocument* doc = NULL;
		if (docMgr->FileDocumentIsOpen(newName, &doc))
			{
			itsDoc = dynamic_cast(TestTextEditDocument*, doc);
			if (itsDoc == NULL)
				{
				(JGetUserNotification())->ReportError(
					"The file that we require is already open for a different purpose.");
				}
			}
		else
			{
			itsDoc = new TestTextEditDocument(supervisor, newName);
			assert( itsDoc != NULL );
			}

		// This would be the place to update the path + name so the file can be
		// easily located next time.
		}
	else
		{
		JString msg = "Unable to open ";
		msg += kRequiredFile;
		msg += ".\n\nI hope you didn't delete it!";
		(JGetUserNotification())->ReportError(msg);
		}

	// Normally, one would only care that the document is open.  In this case,
	// we make it visible so the user can read the tutorial text.

	if (itsDoc != NULL)
		{
		itsDoc->Activate();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestLinkedDocument::~TestLinkedDocument()
{
	// itsDoc will automatically close if it is not currently active
}

/******************************************************************************
 NeedDocument (virtual)

	Keep our linked document open.

 ******************************************************************************/

JBoolean
TestLinkedDocument::NeedDocument
	(
	JXDocument* doc
	)
	const
{
	return JConvertToBoolean( doc == itsDoc );
}
