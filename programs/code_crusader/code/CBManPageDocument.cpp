/******************************************************************************
 CBManPageDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBManPageDocument.h"
#include "CBTextEditor.h"
#include "CBViewManPageDialog.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JProcess.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

JPtrArray<JString>			CBManPageDocument::theManCmdList(JPtrArrayT::kDeleteAll);
JPtrArray<CBTextDocument>	CBManPageDocument::theManDocList(JPtrArrayT::kForgetAll);

/******************************************************************************
 Constructor function (static)

	First searches the open man pages to see if the requested one already
	exists.  If not, creates a new one and activates it.

	returnDoc can be NULL.

 ******************************************************************************/

JBoolean
CBManPageDocument::Create
	(
	CBManPageDocument**	returnDoc,
	const JCharacter*	origPageName,
	const JCharacter	pageIndex,
	const JBoolean		apropos
	)
{
	JString pageName = origPageName;
	pageName.TrimWhitespace();
	if (pageName.IsEmpty())
		{
		if (returnDoc != NULL)
			{
			*returnDoc = NULL;
			}
		return kJFalse;
		}

	(CBGetViewManPageDialog())->AddToHistory(pageName, pageIndex, apropos);

	// check if the man page is already open

	theManCmdList.SetCompareFunction(JCompareStringsCaseSensitive);

	JString cmd;
	if (apropos)
		{
		cmd = GetCmd2(pageName);
		}
	else
		{
		cmd = GetCmd1(pageName, pageIndex);
		}

	JIndex i;
	if (theManCmdList.SearchSorted(&cmd, JOrderedSetT::kAnyMatch, &i))
		{
		CBManPageDocument* doc =
			dynamic_cast<CBManPageDocument*>(theManDocList.NthElement(i));
		assert( doc != NULL );
		doc->Activate();
		if (returnDoc != NULL)
			{
			*returnDoc = doc;
			}
		return kJTrue;
		}

	// If !apropos, we can't check cmd #2 until cmd #1 fails.

	// create a new one

	CBManPageDocument* trueDoc;
	CBManPageDocument* doc =
		jnew CBManPageDocument(pageName, pageIndex, apropos, &trueDoc);
	assert( doc != NULL );

	if (doc != trueDoc)
		{
		const JBoolean ok = doc->Close();
		assert( ok );
		}

	if (returnDoc != NULL)
		{
		*returnDoc = trueDoc;
		}

	if (trueDoc != NULL)
		{
		trueDoc->Activate();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Constructor (protected)

	If the result is already displayed in a different document, we
	return that in *trueDoc.  If there is nothing relevant, *trueDoc == NULL.
	Otherwise, *trueDoc == this.

 ******************************************************************************/

CBManPageDocument::CBManPageDocument
	(
	const JCharacter*	pageName,
	const JCharacter	pageIndex,
	const JBoolean		apropos,
	CBManPageDocument**	trueDoc
	)
	:
	CBTextDocument(kCBManPageFT, kCBManPageHelpName)
{
	theManCmdList.SetCompareFunction(JCompareStringsCaseSensitive);

	*trueDoc = NULL;

	JString* cmd = jnew JString;
	assert( cmd != NULL );

	JBoolean success = kJFalse;
	if (!apropos)
		{
		*cmd = GetCmd1(pageName, pageIndex);

		// Create() already searched theManCmdList

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, NULL,
							 kJCreatePipe, &fromFD, kJTossOutput);
		if (!execErr.OK())
			{
			execErr.ReportIfError();
			*trueDoc = this;
			return;
			}
		else
			{
			std::ifstream input;
			JString tempName;
			if (JConvertToStream(fromFD, &input, &tempName))
				{
				if (GetTextEditor()->ReadUNIXManOutput(input, kJTrue))
					{
					p->WaitUntilFinished();
					success = p->SuccessfulFinish();
					}
				else
					{
					p->Kill();
					success = kJTrue;	// we started reading, but user cancelled
					}

				input.close();
				JRemoveFile(tempName);
				}

			jdelete p;
			}
		}

	if (!success)
		{
		*cmd = GetCmd2(pageName);

		JIndex i;
		if (theManCmdList.SearchSorted(cmd, JOrderedSetT::kAnyMatch, &i))
			{
			CBManPageDocument* doc =
				dynamic_cast<CBManPageDocument*>(theManDocList.NthElement(i));
			assert( doc != NULL );
			*trueDoc = doc;
			return;
			}

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, NULL,
							 kJCreatePipe, &fromFD, kJTossOutput);
		if (!execErr.OK())
			{
			execErr.ReportIfError();
			*trueDoc = this;
			return;
			}
		else
			{
			std::ifstream input;
			JString tempName, text;
			if (JConvertToStream(fromFD, &input, &tempName))
				{
				input >> std::ws;
				text.SetBlockSize(1000);
				while (!input.eof() && !input.fail())
					{
					text += JReadLine(input);
					text += "\n\n";
					input >> std::ws;
					}
				text.TrimWhitespace();

				input.close();
				JRemoveFile(tempName);
				}

			// We can't check the return value of "man -k" because it is
			// always 1.  We can't use "apropos" either because it always
			// returns 0.

//			p->WaitUntilFinished();
//			success = p->SuccessfulFinish();
			jdelete p;
//			if (success)
//				{
				GetTextEditor()->SetText(text);
//				}
/*			else
				{
				text  = "No references to \"";
				text += pageName;
				text += "\" could be found in either the source files or the man pages.";
				(JGetUserNotification())->DisplayMessage(text);
				*trueDoc = NULL;
				return;
				}
*/			}
		}

	itsIgnoreNameChangedFlag = kJTrue;
	FileChanged(*cmd, kJFalse);
	itsIgnoreNameChangedFlag = kJFalse;

	GetTextEditor()->SetWritable(kJFalse);

	*trueDoc = this;

	JIndex index;
	theManCmdList.InsertSorted(cmd, kJTrue, &index);
	theManDocList.InsertAtIndex(index, this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBManPageDocument::~CBManPageDocument()
{
	RemoveFromManPageList(this);
}

/******************************************************************************
 Receive (protected)

	Listen for calls to FileChanged().

 ******************************************************************************/

void
CBManPageDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXFileDocument::kNameChanged) &&
		!itsIgnoreNameChangedFlag)
		{
		RemoveFromManPageList(this);
		}

	CBTextDocument::Receive(sender, message);
}

/******************************************************************************
 RemoveFromManPageList (static private)

 ******************************************************************************/

void
CBManPageDocument::RemoveFromManPageList
	(
	CBManPageDocument* doc
	)
{
	JIndex i;
	if (theManDocList.Find(doc, &i))
		{
		theManCmdList.DeleteElement(i);
		theManDocList.RemoveElement(i);
		}
}

/******************************************************************************
 GetCmd1 (static protected)

 ******************************************************************************/

JString
CBManPageDocument::GetCmd1
	(
	const JCharacter*	pageName,
	const JCharacter	pageIndex
	)
{
	JString cmd = "man ";
	if (pageIndex != ' ')
		{
		#ifdef _J_MAN_SECTION_VIA_DASH_S
		cmd += "-s ";
		#endif

		const JCharacter pageIndexStr[] = { pageIndex, ' ', '\0' };
		cmd += pageIndexStr;
		}
	cmd += JPrepArgForExec(pageName);
	return cmd;
}

/******************************************************************************
 GetCmd2 (static protected)

 ******************************************************************************/

JString
CBManPageDocument::GetCmd2
	(
	const JCharacter* pageName
	)
{
	JString cmd = "man -k ";
	cmd += JPrepArgForExec(pageName);
	return cmd;
}
