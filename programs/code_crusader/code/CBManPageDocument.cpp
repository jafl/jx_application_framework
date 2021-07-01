/******************************************************************************
 CBManPageDocument.cpp

	BASE CLASS = CBTextDocument

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "CBManPageDocument.h"
#include "CBTextEditor.h"
#include "CBViewManPageDialog.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JProcess.h>
#include <jTextUtil.h>
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

	returnDoc can be nullptr.

 ******************************************************************************/

JBoolean
CBManPageDocument::Create
	(
	CBManPageDocument**	returnDoc,
	const JString&		origPageName,
	const JString&		pageIndex,
	const JBoolean		apropos
	)
{
	JString pageName = origPageName;
	pageName.TrimWhitespace();
	if (pageName.IsEmpty())
		{
		if (returnDoc != nullptr)
			{
			*returnDoc = nullptr;
			}
		return kJFalse;
		}

	CBGetViewManPageDialog()->AddToHistory(pageName, pageIndex, apropos);

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
	if (theManCmdList.SearchSorted(&cmd, JListT::kAnyMatch, &i))
		{
		CBManPageDocument* doc =
			dynamic_cast<CBManPageDocument*>(theManDocList.GetElement(i));
		assert( doc != nullptr );
		doc->Activate();
		if (returnDoc != nullptr)
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
	assert( doc != nullptr );

	if (doc != trueDoc)
		{
		const JBoolean ok = doc->Close();
		assert( ok );
		}

	if (returnDoc != nullptr)
		{
		*returnDoc = trueDoc;
		}

	if (trueDoc != nullptr)
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
	return that in *trueDoc.  If there is nothing relevant, *trueDoc == nullptr.
	Otherwise, *trueDoc == this.

 ******************************************************************************/

CBManPageDocument::CBManPageDocument
	(
	const JString&		pageName,
	const JString&		pageIndex,
	const JBoolean		apropos,
	CBManPageDocument**	trueDoc
	)
	:
	CBTextDocument(kCBManPageFT, "CBManPageHelp")
{
	theManCmdList.SetCompareFunction(JCompareStringsCaseSensitive);

	*trueDoc = nullptr;

	JString* cmd = jnew JString;
	assert( cmd != nullptr );

	JBoolean success = kJFalse;
	if (!apropos)
		{
		*cmd = GetCmd1(pageName, pageIndex);

		// Create() already searched theManCmdList

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, nullptr,
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
				JReadUNIXManOutput(input, GetTextEditor()->GetText());
				p->WaitUntilFinished();
				success = p->SuccessfulFinish();

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
		if (theManCmdList.SearchSorted(cmd, JListT::kAnyMatch, &i))
			{
			CBManPageDocument* doc =
				dynamic_cast<CBManPageDocument*>(theManDocList.GetElement(i));
			assert( doc != nullptr );
			*trueDoc = doc;
			return;
			}

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, nullptr,
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
				GetTextEditor()->GetText()->SetText(text);
//				}
/*			else
				{
				text  = "No references to \"";
				text += pageName;
				text += "\" could be found in either the source files or the man pages.";
				JGetUserNotification()->DisplayMessage(text);
				*trueDoc = nullptr;
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
	const JString&	pageName,
	const JString&	pageIndex
	)
{
	JString cmd("man ");
	if (!pageIndex.IsEmpty())
		{
		#ifdef _J_MAN_SECTION_VIA_DASH_S
		cmd += "-s ";
		#endif

		cmd += JPrepArgForExec(pageIndex);
		cmd += " ";
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
	const JString& pageName
	)
{
	JString cmd("man -k ");
	cmd += JPrepArgForExec(pageName);
	return cmd;
}
