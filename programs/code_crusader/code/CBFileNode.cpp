/******************************************************************************
 CBFileNode.cpp

	BASE CLASS = CBFileNodeBase

	Copyright © 1999 John Lindal.

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
	CBProjectTree*	tree,
	const JString&	fileName
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
		CBGetDocumentManager()->OpenSomething(fullName);
		}
	else
		{
		ReportNotFound();
		}
}

/******************************************************************************
 ParseFiles (virtual)

 ******************************************************************************/

bool
CBFileNode::ParseFiles
	(
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBDTree*					dTree,
	CBGoTree*					goTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	JString fullName, trueName;
	if (GetFullName(&fullName) && JGetTrueName(fullName, &trueName))
		{
		if (!ParseFile(trueName, parser, allSuffixList, symbolList, cTree, dTree, goTree, javaTree, phpTree, pg))
			{
			return false;
			}

		const CBTextFileType type = CBGetPrefsManager()->GetFileType(trueName);
		if (CBGetDocumentManager()->GetComplementFile(trueName, type, &fullName,
														GetProjectDoc(), false) &&
			JGetTrueName(fullName, &trueName) &&
			!ParseFile(trueName, parser, allSuffixList, symbolList, cTree, dTree, goTree, javaTree, phpTree, pg))
			{
			return false;
			}
		}
	return CBFileNodeBase::ParseFiles(parser, allSuffixList, symbolList, cTree, dTree, goTree, javaTree, phpTree, pg);
}

/******************************************************************************
 ParseFile (private)

 ******************************************************************************/

bool
CBFileNode::ParseFile
	(
	const JString&				fullName,
	CBFileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	CBSymbolList*				symbolList,
	CBCTree*					cTree,
	CBDTree*					dTree,
	CBGoTree*					goTree,
	CBJavaTree*					javaTree,
	CBPHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	time_t t;
	JGetModificationTime(fullName, &t);
	parser->ParseFile(fullName, allSuffixList, t, symbolList, cTree, dTree, goTree, javaTree, phpTree);

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
			JXGetWebBrowser()->ShowFileContent(fullName);
			}
		else
			{
			CBGetDocumentManager()->OpenComplementFile(fullName, type);
			}
		}
	else
		{
		const JUtf8Byte* map[] =
		{
			"name", GetFileName().GetBytes()
		};
		const JString msg = JGetString("ComplFileNotFound::CBFileNode", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		}
}

/******************************************************************************
 ViewPlainDiffs (virtual)

 ******************************************************************************/

void
CBFileNode::ViewPlainDiffs
	(
	const bool silent
	)
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		CBGetDiffFileDialog()->ViewDiffs(true, fullName, silent);
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
	const bool silent
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

	bool exists;
	input >> JBoolFromString(exists);
	if (exists)
		{
		JString relName, data;
		input >> relName >> data;

		JString path, name;
		JSplitPathAndName(relName, &path, &name);

		const auto* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
		assert( projTree != nullptr );

		const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
		path = JCombinePathAndName(basePath, path);

		if (!JDirectoryExists(path))
			{
			const JError err = JCreateDirectory(path);
			if (!err.OK())
				{
				const JUtf8Byte* map[] =
				{
					"name", relName.GetBytes(),
					"err",  err.GetMessage().GetBytes()
				};
				const JString msg = JGetString("CreateFileFailedWithError::CBFileNode", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				return;
				}
			}

		const JString fullName = JCombinePathAndName(path, name);
		if (JFileExists(fullName))
			{
			const JUtf8Byte* map[] =
			{
				"name", fullName.GetBytes()
			};
			const JString msg = JGetString("WarnReplaceFile::CBFileNode", map, sizeof(map));
			if (!JGetUserNotification()->AskUserNo(msg))
				{
				return;
				}
			}

		std::ofstream output(fullName.GetBytes());
		if (output.good())
			{
			data.Print(output);
			}
		else
			{
			const JUtf8Byte* map[] =
			{
				"name", relName.GetBytes()
			};
			const JString msg = JGetString("CreateFileFailed::CBFileNode", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		}
}

/******************************************************************************
 SaveFilesInTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

#ifdef _J_UNIX
static const JUtf8Byte* kPathPrefix = "./";
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
		output << JBoolToString(true) << ' ' << relName << ' ' << data;
		}
	else
		{
		output << JBoolToString(false);
		}
}
