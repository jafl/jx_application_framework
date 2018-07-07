/******************************************************************************
 CBFileNode.cpp

	BASE CLASS = CBFileNodeBase

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBFileNode.h"
#include "CBProjectTree.h"
#include "CBProjectDocument.h"
#include "CBFileListTable.h"
#include "CBDiffFileDialog.h"
#include "cbGlobals.h"
#include <JXWebBrowser.h>
#include <JProgressDisplay.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileNode::CBFileNode
	(
	CBProjectTree*		tree,
	const JCharacter*	fileName
	)
	:
	CBFileNodeBase(tree, kCBFileNT, fileName)
{
}

CBFileNode::CBFileNode
	(
	std::istream&			input,
	const JFileVersion	vers,
	CBProjectNode*		parent
	)
	:
	CBFileNodeBase(input, vers, parent, kCBFileNT)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileNode::~CBFileNode()
{
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
CBFileNode::OpenFile()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		(CBGetDocumentManager())->OpenSomething(fullName);
		}
	else
		{
		ReportNotFound();
		}
}

/******************************************************************************
 ParseFiles (virtual)

 ******************************************************************************/

JBoolean
CBFileNode::ParseFiles
	(
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	JString fullName, trueName;
	if (GetFullName(&fullName) && JGetTrueName(fullName, &trueName))
		{
		if (!ParseFile(trueName, parser, allSuffixList, symbolList, cTree, javaTree, phpTree, pg))
			{
			return kJFalse;
			}

		const CBTextFileType type = CBGetPrefsManager()->GetFileType(trueName);
		if ((CBGetDocumentManager())->GetComplementFile(trueName, type, &fullName,
														GetProjectDoc(), kJFalse) &&
			JGetTrueName(fullName, &trueName) &&
			!ParseFile(trueName, parser, allSuffixList, symbolList, cTree, javaTree, phpTree, pg))
			{
			return kJFalse;
			}
		}
	return CBFileNodeBase::ParseFiles(parser, allSuffixList, symbolList, cTree, javaTree, phpTree, pg);
}

/******************************************************************************
 ParseFile (private)

 ******************************************************************************/

JBoolean
CBFileNode::ParseFile
	(
	const JString&				fullName,
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	time_t t;
	JGetModificationTime(fullName, &t);
	parser->ParseFile(fullName, allSuffixList, t, symbolList, cTree, javaTree, phpTree);

	return pg.IncrementProgress();
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
CBFileNode::OpenComplementFile()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		const CBTextFileType type = CBGetPrefsManager()->GetFileType(fullName);
		if (type == kCBHTMLFT || type == kCBXMLFT)
			{
			(JXGetWebBrowser())->ShowFileContent(fullName);
			}
		else
			{
			(CBGetDocumentManager())->OpenComplementFile(fullName, type);
			}
		}
	else
		{
		JString msg = "Unable to find complement of \"";
		msg += GetFileName();
		msg.AppendCharacter('"');
		(JGetUserNotification())->ReportError(msg);
		}
}

/******************************************************************************
 ViewPlainDiffs (virtual)

 ******************************************************************************/

void
CBFileNode::ViewPlainDiffs
	(
	const JBoolean silent
	)
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		(CBGetDiffFileDialog())->ViewDiffs(kJTrue, fullName, silent);
		}
	else
		{
		ReportNotFound();
		}
}

/******************************************************************************
 ViewVCSDiffs (virtual)

 ******************************************************************************/

void
CBFileNode::ViewVCSDiffs
	(
	const JBoolean silent
	)
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		(CBGetDiffFileDialog())->ViewVCSDiffs(fullName, silent);
		}
	else
		{
		ReportNotFound();
		}
}

/******************************************************************************
 CreateFilesForTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
CBFileNode::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	CBFileNodeBase::CreateFilesForTemplate(input, vers);

	JBoolean exists;
	input >> exists;
	if (exists)
		{
		JString relName, data;
		input >> relName >> data;

		JString path, name;
		JSplitPathAndName(relName, &path, &name);

		const CBProjectTree* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
		assert( projTree != NULL );

		const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
		path = JCombinePathAndName(basePath, path);

		JUserNotification* un = JGetUserNotification();
		if (!JDirectoryExists(path))
			{
			const JError err = JCreateDirectory(path);
			if (!err.OK())
				{
				JString msg = "Unable to create the file ";
				msg += relName;
				msg += " from the template because:\n\n";
				msg += err.GetMessage();
				un->ReportError(msg);
				return;
				}
			}

		const JString fullName = JCombinePathAndName(path, name);
		if (JFileExists(fullName))
			{
			JString msg = fullName;
			msg.PrependCharacter('"');
			msg += "\" already exists.  Do you want to overwrite it?";
			if (!un->AskUserNo(msg))
				{
				return;
				}
			}

		std::ofstream output(fullName);
		if (output.good())
			{
			data.Print(output);
			}
		else
			{
			JString msg = "Unable to create the file ";
			msg += relName;
			msg += " from the template.";
			un->ReportError(msg);
			}
		}
}

/******************************************************************************
 SaveFilesInTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

#ifdef _J_UNIX
static const JCharacter* kPathPrefix = "./";
#endif

void
CBFileNode::SaveFilesInTemplate
	(
	std::ostream& output
	)
	const
{
	CBFileNodeBase::SaveFilesInTemplate(output);

	const JString& relName = GetFileName();
	JString fullName;
	if (relName.BeginsWith(kPathPrefix) && GetFullName(&fullName))
		{
		JString data;
		JReadFile(fullName, &data);
		output << kJTrue << ' ' << relName << ' ' << data;
		}
	else
		{
		output << kJFalse;
		}
}
