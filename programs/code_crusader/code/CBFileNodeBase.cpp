/******************************************************************************
 CBFileNodeBase.cpp

	BASE CLASS = CBProjectNode

	Copyright © 1999 John Lindal. All rights reserved.

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

// string ID's

static const JCharacter* kFileNotFoundID = "FileNotFound::CBFileNodeBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileNodeBase::CBFileNodeBase
	(
	CBProjectTree*			tree,
	const CBProjectNodeType	type,
	const JCharacter*		fileName
	)
	:
	CBProjectNode(tree, type, "", kJFalse),
	itsFileName(fileName)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);
	SetName(name);
}

CBFileNodeBase::CBFileNodeBase
	(
	istream&				input,
	const JFileVersion		vers,
	CBProjectNode*			parent,
	const CBProjectNodeType	type
	)
	:
	CBProjectNode(input, vers, parent, type, kJFalse)
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
	ostream& output
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
	const JCharacter* fileName
	)
{
	if (fileName != itsFileName)
		{
		itsFileName = fileName;

		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		const JBoolean nameChanged = JI2B( name != GetName() );
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

	Returns kJTrue if this file can be found.

 ******************************************************************************/

JBoolean
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

	Returns kJTrue if the specified file can be found.

 ******************************************************************************/

JBoolean
CBFileNodeBase::GetFullName
	(
	const JCharacter*	fileName,
	JString*			fullName
	)
	const
{
	const CBProjectTree* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
	assert( projTree != NULL );

	const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
	return JI2B(!JStringEmpty(fileName) &&
				JConvertToAbsolutePath(fileName, basePath, fullName));
}

/******************************************************************************
 FindFile1 (virtual protected)

 ******************************************************************************/

JBoolean
CBFileNodeBase::FindFile1
	(
	const JCharacter*	fullName,
	CBProjectNode**		node
	)
{
	JString name;
	if (GetFullName(&name) &&
		JSameDirEntry(name, fullName))
		{
		*node = this;
		return kJTrue;
		}
	else
		{
		*node = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

JBoolean
CBFileNodeBase::IncludedInMakefile()
	const
{
	const CBTextFileType type = (CBGetPrefsManager())->GetFileType(itsFileName);
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
		if (invalidList == NULL || GetFullName(&fullName))
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

JBoolean
CBFileNodeBase::IncludedInCMakeData()
	const
{
	const CBTextFileType type = (CBGetPrefsManager())->GetFileType(itsFileName);
	return JI2B(CBIncludeInCMakeSource(type) || CBIncludeInCMakeHeader(type));
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
			const CBTextFileType type = (CBGetPrefsManager())->GetFileType(itsFileName);
			if (CBIncludeInCMakeSource(type))
				{
				src->AppendCharacter(' ');
				*src += itsFileName;

				JString complName;
				if ((CBGetDocumentManager())->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), kJFalse) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
					{
					const CBRelPathCSF::PathType pathType =
						CBRelPathCSF::CalcPathType(itsFileName);
					complName = CBRelPathCSF::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->AppendCharacter(' ');
					*hdr += complName;
					}
				}
			else if (CBIncludeInCMakeHeader(type))
				{
				hdr->AppendCharacter(' ');
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

JBoolean
CBFileNodeBase::IncludedInQMakeData()
	const
{
	const CBTextFileType type = (CBGetPrefsManager())->GetFileType(itsFileName);
	return JI2B(CBIncludeInQMakeSource(type) || CBIncludeInQMakeHeader(type));
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
			const CBTextFileType type = (CBGetPrefsManager())->GetFileType(itsFileName);
			if (CBIncludeInQMakeSource(type))
				{
				src->AppendCharacter(' ');
				*src += itsFileName;

				JString complName;
				if ((CBGetDocumentManager())->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), kJFalse) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
					{
					const CBRelPathCSF::PathType pathType =
						CBRelPathCSF::CalcPathType(itsFileName);
					complName = CBRelPathCSF::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->AppendCharacter(' ');
					*hdr += complName;
					}
				}
			else if (CBIncludeInQMakeHeader(type))
				{
				hdr->AppendCharacter(' ');
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
	const JCharacter* origFullName,
	const JCharacter* newFullName
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
		const JBoolean ok = JExpandHomeDirShortcut(itsFileName, &s);
		assert( ok );
		if (s == origFullName)
			{
			s = CBRelPathCSF::ConvertToRelativePath(newFullName, NULL, type);
			SetFileName(s);
			}
		}
	else
		{
		assert( type == CBRelPathCSF::kProjectRelative );

		const CBProjectTree* projTree = dynamic_cast<const CBProjectTree*>(GetTree());
		assert( projTree != NULL );

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
	const JCharacter* map[] =
		{
		"name", itsFileName.GetCString()
		};
	const JString msg = JGetString(kFileNotFoundID, map, sizeof(map));
	(JGetUserNotification())->ReportError(msg);
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
	CBFileNodeBase* node = NULL;

	const CBTextFileType type = (CBGetPrefsManager())->GetFileType(fileName);
	if (CBIsLibrary(type))
		{
		node = new CBLibraryNode(tree, fileName);
		}
	else
		{
		node = new CBFileNode(tree, fileName);
		}
	assert( node != NULL );

	return node;
}
