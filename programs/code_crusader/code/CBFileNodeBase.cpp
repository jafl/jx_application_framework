/******************************************************************************
 CBFileNodeBase.cpp

	BASE CLASS = CBProjectNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "CBFileNodeBase.h"
#include "CBProjectDocument.h"
#include "CBProjectTree.h"
#include "CBFileNode.h"
#include "CBLibraryNode.h"
#include "CBRelPathCSF.h"
#include "cbGlobals.h"
#include <JXWebBrowser.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileNodeBase::CBFileNodeBase
	(
	CBProjectTree*			tree,
	const CBProjectNodeType	type,
	const JString&			fileName
	)
	:
	CBProjectNode(tree, type, JString::empty, false),
	itsFileName(fileName)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);
	SetName(name);
}

CBFileNodeBase::CBFileNodeBase
	(
	std::istream&			input,
	const JFileVersion		vers,
	CBProjectNode*			parent,
	const CBProjectNodeType	type
	)
	:
	CBProjectNode(input, vers, parent, type, false)
{
	input >> itsFileName;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileNodeBase::~CBFileNodeBase()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CBFileNodeBase::StreamOut
	(
	std::ostream& output
	)
	const
{
	CBProjectNode::StreamOut(output);
	output << itsFileName << '\n';
}

/******************************************************************************
 SetFileName

 ******************************************************************************/

void
CBFileNodeBase::SetFileName
	(
	const JString& fileName
	)
{
	if (fileName != itsFileName)
		{
		itsFileName = fileName;

		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		const bool nameChanged = name != GetName();
		SetName(name);

		JTree* tree;
		if (!nameChanged && GetTree(&tree))
			{
			tree->BroadcastChange(this);
			}
		}
}

/******************************************************************************
 GetFullName (virtual)

	Returns true if this file can be found.

 ******************************************************************************/

bool
CBFileNodeBase::GetFullName
	(
	JString* fullName
	)
	const
{
	return GetFullName(itsFileName, fullName);
}

/******************************************************************************
 GetFullName (protected)

	Returns true if the specified file can be found.

 ******************************************************************************/

bool
CBFileNodeBase::GetFullName
	(
	const JString&	fileName,
	JString*		fullName
	)
	const
{
	const auto* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert( projTree != nullptr );

	const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
	return !fileName.IsEmpty() &&
				JConvertToAbsolutePath(fileName, basePath, fullName);
}

/******************************************************************************
 FindFile1 (virtual protected)

 ******************************************************************************/

bool
CBFileNodeBase::FindFile1
	(
	const JString&	fullName,
	CBProjectNode**	node
	)
{
	JString name;
	if (GetFullName(&name) &&
		JSameDirEntry(name, fullName))
		{
		*node = this;
		return true;
		}
	else
		{
		*node = nullptr;
		return false;
		}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

bool
CBFileNodeBase::IncludedInMakefile()
	const
{
	const CBTextFileType type = CBGetPrefsManager()->GetFileType(itsFileName);
	return CBIncludeInMakeFiles(type);
}

/******************************************************************************
 BuildMakeFiles (virtual)

 ******************************************************************************/

void
CBFileNodeBase::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	if (IncludedInMakefile())
		{
		JString fullName;
		if (invalidList == nullptr || GetFullName(&fullName))
			{
			JString root, suffix;
			JSplitRootAndSuffix(itsFileName, &root, &suffix);
			*text += ".";
			*text += suffix;
			*text += " ";
			*text += root;
			*text += "\n";
			}
		else
			{
			invalidList->Append(const_cast<CBFileNodeBase*>(this));
			}
		}

	CBProjectNode::BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
}

/******************************************************************************
 IncludedInCMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
CBFileNodeBase::IncludedInCMakeData()
	const
{
	const CBTextFileType type = CBGetPrefsManager()->GetFileType(itsFileName);
	return CBIncludeInCMakeSource(type) || CBIncludeInCMakeHeader(type);
}

/******************************************************************************
 BuildCMakeData (virtual)

 ******************************************************************************/

void
CBFileNodeBase::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	if (IncludedInCMakeData())
		{
		JString fullName;
		if (GetFullName(&fullName))
			{
			const CBTextFileType type = CBGetPrefsManager()->GetFileType(itsFileName);
			if (CBIncludeInCMakeSource(type))
				{
				src->Append(" ");
				*src += itsFileName;

				JString complName;
				if (CBGetDocumentManager()->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), false) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
					{
					const CBRelPathCSF::PathType pathType =
						CBRelPathCSF::CalcPathType(itsFileName);
					complName = CBRelPathCSF::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->Append(" ");
					*hdr += complName;
					}
				}
			else if (CBIncludeInCMakeHeader(type))
				{
				hdr->Append(" ");
				*hdr += itsFileName;
				}
			}
		else
			{
			invalidList->Append(const_cast<CBFileNodeBase*>(this));
			}
		}

	CBProjectNode::BuildCMakeData(src, hdr, invalidList);
}

/******************************************************************************
 IncludedInQMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
CBFileNodeBase::IncludedInQMakeData()
	const
{
	const CBTextFileType type = CBGetPrefsManager()->GetFileType(itsFileName);
	return CBIncludeInQMakeSource(type) || CBIncludeInQMakeHeader(type);
}

/******************************************************************************
 BuildQMakeData (virtual)

 ******************************************************************************/

void
CBFileNodeBase::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	if (IncludedInQMakeData())
		{
		JString fullName;
		if (GetFullName(&fullName))
			{
			const CBTextFileType type = CBGetPrefsManager()->GetFileType(itsFileName);
			if (CBIncludeInQMakeSource(type))
				{
				src->Append(" ");
				*src += itsFileName;

				JString complName;
				if (CBGetDocumentManager()->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), false) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
					{
					const CBRelPathCSF::PathType pathType =
						CBRelPathCSF::CalcPathType(itsFileName);
					complName = CBRelPathCSF::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->Append(" ");
					*hdr += complName;
					}
				}
			else if (CBIncludeInQMakeHeader(type))
				{
				hdr->Append(" ");
				*hdr += itsFileName;
				}
			}
		else
			{
			invalidList->Append(const_cast<CBFileNodeBase*>(this));
			}
		}

	CBProjectNode::BuildQMakeData(src, hdr, invalidList);
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
CBFileNodeBase::Print
	(
	JString* text
	)
	const
{
	*text += "  ";
	*text += itsFileName;
	*text += "\n";

	CBProjectNode::Print(text);
}

/******************************************************************************
 FileRenamed (virtual)

 ******************************************************************************/

void
CBFileNodeBase::FileRenamed
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	const CBRelPathCSF::PathType type = CBRelPathCSF::CalcPathType(itsFileName);
	if (type == CBRelPathCSF::kAbsolutePath && itsFileName == origFullName)
		{
		SetFileName(newFullName);
		}
	else if (type == CBRelPathCSF::kHomeRelative)
		{
		JString s;
		const bool ok = JExpandHomeDirShortcut(itsFileName, &s);
		assert( ok );
		if (s == origFullName)
			{
			s = CBRelPathCSF::ConvertToRelativePath(newFullName, JString::empty, type);
			SetFileName(s);
			}
		}
	else
		{
		assert( type == CBRelPathCSF::kProjectRelative );

		const auto* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
		assert( projTree != nullptr );

		const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
		JString s               = JConvertToRelativePath(origFullName, basePath);
		if (itsFileName == s)
			{
			s = CBRelPathCSF::ConvertToRelativePath(newFullName, basePath, type);
			SetFileName(s);
			}
		}

	CBProjectNode::FileRenamed(origFullName, newFullName);
}

/******************************************************************************
 ShowFileLocation (virtual)

 ******************************************************************************/

void
CBFileNodeBase::ShowFileLocation()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
		{
		(JXGetWebBrowser())->ShowFileLocation(fullName);
		}
	else
		{
		ReportNotFound();
		}
}

/******************************************************************************
 ReportNotFound (protected)

 ******************************************************************************/

void
CBFileNodeBase::ReportNotFound()
	const
{
	const JUtf8Byte* map[] =
		{
		"name", itsFileName.GetBytes()
		};
	const JString msg = JGetString("FileNotFound::CBFileNodeBase", map, sizeof(map));
	JGetUserNotification()->ReportError(msg);
}

/******************************************************************************
 New (static)

	Creates the appropriate node, based on the file name.

 ******************************************************************************/

CBFileNodeBase*
CBFileNodeBase::New
	(
	CBProjectTree*	tree,
	const JString&	fileName
	)
{
	CBFileNodeBase* node = nullptr;

	const CBTextFileType type = CBGetPrefsManager()->GetFileType(fileName);
	if (CBIsLibrary(type))
		{
		node = jnew CBLibraryNode(tree, fileName);
		}
	else
		{
		node = jnew CBFileNode(tree, fileName);
		}
	assert( node != nullptr );

	return node;
}
