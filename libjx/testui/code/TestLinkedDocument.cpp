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

#include "TestLinkedDocument.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JString kOurFile     ("about_owner_docs", false);
static const JString kRequiredFile("about_owned_docs", false);

/******************************************************************************
 Constructor

 ******************************************************************************/

TestLinkedDocument::TestLinkedDocument
	(
	JXDirector* supervisor
	)
	:
	TestTextEditDocument(supervisor),
	itsDoc(nullptr)
{
	if (JFileExists(kOurFile))
	{
		FileChanged(kOurFile, true);
		ReadFile(kOurFile);
	}
	else
	{
		JGetUserNotification()->ReportError(
			JGetString("FileNotFound::TestLinkedDocument"));
	}

	// Normally, currDir would be relative to the directory containing our data file,
	// and fileName would come from our data file.

	const JString currDir  = JGetCurrentDirectory();
	const JString fileName = currDir + kRequiredFile;

	JXDocumentManager* docMgr = JXGetDocumentManager();

	JString newName;
	if (docMgr->FindFile(fileName, currDir, &newName))
	{
		JXFileDocument* doc = nullptr;
		if (docMgr->FileDocumentIsOpen(newName, &doc))
		{
			itsDoc = dynamic_cast<TestTextEditDocument*>(doc);
			if (itsDoc == nullptr)
			{
				JGetUserNotification()->ReportError(
					JGetString("FileAlreadyOpen::TestLinkedDocument"));
			}
		}
		else
		{
			itsDoc = jnew TestTextEditDocument(supervisor, newName, false);
		}

		// This would be the place to update the path + name so the file can be
		// easily located next time.
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"name", kRequiredFile.GetBytes()
		};
		JGetUserNotification()->ReportError(
			JGetString("OwnedFileNotFound::TestLinkedDocument", map, sizeof(map)));
	}

	// Normally, one would only care that the document is open.  In this case,
	// we make it visible so the user can read the tutorial text.

	if (itsDoc != nullptr)
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

bool
TestLinkedDocument::NeedDocument
	(
	JXDocument* doc
	)
	const
{
	return doc == itsDoc;
}
