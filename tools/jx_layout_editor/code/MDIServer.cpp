/******************************************************************************
 MDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "MDIServer.h"
#include "LayoutDocument.h"
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
			LayoutDocument::Create(*n);
		}

		JChangeDirectory(origDir);
	}
	else if (!ChooseFiles())
	{
		LayoutDocument* doc;
		LayoutDocument::Create(&doc);
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
		JString::empty, "*.jxl *.fd");

	if (dlog->DoDialog())
	{
		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
		dlog->GetFullNames(&fullNameList);

		for (const auto* n : fullNameList)
		{
			LayoutDocument::Create(*n);
		}
		return true;
	}

	return false;
}
