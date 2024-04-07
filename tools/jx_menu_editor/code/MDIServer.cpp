/******************************************************************************
 MDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "MDIServer.h"
#include "MenuDocument.h"
#include "globals.h"
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MDIServer::MDIServer()
	:
	JXMDIServer()
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDIServer::~MDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
MDIServer::HandleMDIRequest
	(
	const JString&				dir,
	const JPtrArray<JString>&	argList
	)
{
	MenuDocument* doc;

	JString fileName;
	if (argList.GetItemCount() > 1)
	{
		const JString origDir = JGetCurrentDirectory();

		JError err = JNoError();
		if (!JChangeDirectory(dir, &err))
		{
			err.ReportIfError();
			return;
		}

		JPtrArray<JString> fullNameList(argList, JPtrArrayT::kForgetAll);
		fullNameList.RemoveItem(1);

		for (const auto* n : fullNameList)
		{
			MenuDocument::Create(*n, &doc);
		}

		JChangeDirectory(origDir);
	}
	else if (!ChooseFiles() && !GetDocumentManager()->HasDocuments())
	{
		MenuDocument::Create(&doc);
	}
}

/******************************************************************************
 ChooseFiles (static)

 *****************************************************************************/

bool
MDIServer::ChooseFiles()
{
	auto* dlog = JXChooseFileDialog::Create(
		JXChooseFileDialog::kSelectMultipleFiles,
		JString::empty, "*.jxm");

	if (dlog->DoDialog())
	{
		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
		dlog->GetFullNames(&fullNameList);

		MenuDocument* doc;
		for (const auto* n : fullNameList)
		{
			MenuDocument::Create(*n, &doc);
		}
		return true;
	}

	return false;
}
