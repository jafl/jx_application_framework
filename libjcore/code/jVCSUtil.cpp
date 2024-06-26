/******************************************************************************
 jVCSUtil.cpp

	Routines to help with Version Control Systems.

	Copyright (C) 2005 John Lindal.

 ******************************************************************************/

#include "jVCSUtil.h"
#include "JRegex.h"
#include "JSimpleProcess.h"
#include "jFileUtil.h"
#include "jStreamUtil.h"
#include "jFStreamUtil.h"
#include "jXMLUtil.h"
#include "JStdError.h"
#include <libxml/parser.h>
#include "jGlobals.h"
#include "jAssert.h"

static const JString kGitDirName        (".git");
static const JString kSubversionDirName (".svn");
static const JString kSubversionFileName("entries");
static const JString kCVSDirName        ("CVS");
static const JString kSCCSDirName       ("SCCS");

static const JUtf8Byte* kDirName[] =
{
	kGitDirName.GetBytes(),
	kSubversionDirName.GetBytes(),
	kCVSDirName.GetBytes(),
	kSCCSDirName.GetBytes()
};

const JSize kDirCount = sizeof(kDirName) / sizeof(const JUtf8Byte*);

// error types

const JUtf8Byte* kJUnsupportedVCS = "JUnsupportedVCS";

// local

static bool	jSearchVCSRoot(const JString& path, const JString& vcsDirName,
						   JString* vcsRoot);

/******************************************************************************
 JIsVCSDirectory

 ******************************************************************************/

bool
JIsVCSDirectory
	(
	const JString& name
	)
{
	return (name == kGitDirName        ||
			name == kSubversionDirName ||
			name == kCVSDirName        ||
			name == kSCCSDirName);
}

/******************************************************************************
 JGetVCSDirectoryNames

 ******************************************************************************/

JSize
JGetVCSDirectoryNames
	(
	const JUtf8Byte*** dirNames
	)
{
	*dirNames = kDirName;
	return kDirCount;
}

/******************************************************************************
 JGetVCSType

 ******************************************************************************/

JVCSType
JGetVCSType
	(
	const JString& path
	)
{
	JString p = path, n;
	if (JFileExists(path) ||
		!JDirectoryExists(path))	// broken link
	{
		JSplitPathAndName(path, &p, &n);
	}

	// can't read newer versions

	JString vcsDir = JCombinePathAndName(p, kSubversionDirName);
	vcsDir         = JCombinePathAndName(vcsDir, kSubversionFileName);
	if (JFileExists(vcsDir))
	{
		return kJSVNType;
	}

	vcsDir = JCombinePathAndName(p, kCVSDirName);
	if (JDirectoryExists(vcsDir))
	{
		return kJCVSType;
	}

	vcsDir = JCombinePathAndName(p, kSCCSDirName);
	if (JDirectoryExists(vcsDir))
	{
		return kJSCCSType;
	}

	// check git & new svn last, since they need to search directory tree up to root

	if (JSearchGitRoot(p, &n))
	{
		return kJGitType;
	}
	else if (jSearchVCSRoot(p, kSubversionDirName, &n))
	{
		return kJSVNType;
	}

	return kJUnknownVCSType;
}

/******************************************************************************
 JIsManagedByVCS

 ******************************************************************************/

bool
JIsManagedByVCS
	(
	const JString&	fullName,
	JVCSType*		returnType
	)
{
	const JVCSType type = JGetVCSType(fullName);

	bool isManaged = false;
	if (type == kJSVNType)
	{
		JString path, name, entriesFileName;
		JSplitPathAndName(fullName, &path, &name);
		entriesFileName = JCombinePathAndName(path, kSubversionDirName);
		entriesFileName = JCombinePathAndName(entriesFileName, kSubversionFileName);

		ifstream input(entriesFileName.GetBytes());
		const JString version = JReadLine(input);
		if (version == "8" || version == "9" || version == "10")
		{
			const JString pattern = "\n\f\n" + name + "\n";
			JIgnoreUntil(input, pattern.GetBytes(), &isManaged);
		}
	}
	else if (type == kJGitType)
	{
		isManaged = true;	// TODO: ask git (until then, better safe than sorry)
	}

	if (returnType != nullptr)
	{
		*returnType = (isManaged ? type : kJUnknownVCSType);
	}
	return isManaged;
}

/******************************************************************************
 JEditVCS

 ******************************************************************************/

void
JEditVCS
	(
	const JString& fullName
	)
{
	if (JFileExists(fullName) && !JFileWritable(fullName))
	{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JString vcsDir = JCombinePathAndName(path, kCVSDirName);
		if (JDirectoryExists(vcsDir))
		{
			const JString cmd = "cd " + path + "; cvs edit " + name;
			if (system(cmd.GetBytes()) == -1)
			{
				std::cout << cmd << " failed" << std::endl;
			}
		}
	}
}

/******************************************************************************
 JRenameVCS

 ******************************************************************************/

static const JString kMoveFileCmd("mv");

JError
JRenameVCS
	(
	const JString& oldFullName,
	const JString& newFullName
	)
{
	if (!JNameUsed(oldFullName))
	{
		return JDirEntryDoesNotExist(oldFullName);
	}

	JString oldPath, newPath, name;
	JSplitPathAndName(newFullName, &newPath, &name);
	JSplitPathAndName(oldFullName, &oldPath, &name);	// must be second

	JGetTemporaryDirectoryChangeMutex().lock();

	const JString origPath = JGetCurrentDirectory();
	if (!JChangeDirectory(oldPath))
	{
		JGetTemporaryDirectoryChangeMutex().unlock();
		return JAccessDenied(oldPath);
	}

	JVCSType type1    = JGetVCSType(oldPath);
	JVCSType type2    = JGetVCSType(newPath);
	JError err        = JNoError();
	bool tryPlain = false;
	JString cmd;
	JProcess* p = nullptr;
	if (type1 != type2)
	{
		tryPlain = true;
	}
	else if (type1 == kJSVNType || type1 == kJGitType)
	{
		if (type1 == kJSVNType)
		{
			cmd  = "svn mv --force ";
			cmd += JPrepArgForExec(oldFullName);
			cmd += " ";
			cmd += JPrepArgForExec(newFullName);
		}
		else if (type1 == kJGitType)
		{
			cmd  = "git mv -f ";
			cmd += JPrepArgForExec(name);
			cmd += " ";
			cmd += JPrepArgForExec(newFullName);
		}

		err = JProcess::Create(&p, cmd);
		if (err.OK())
		{
			p->WaitUntilFinished();
		}

		if (p != nullptr && !p->SuccessfulFinish())
		{
			err      = JAccessDenied(oldFullName, newFullName);
			tryPlain = true;
		}
	}
	else if (type1 == kJUnknownVCSType)
	{
		tryPlain = true;
	}
	else
	{
		err = JUnsupportedVCS(oldFullName);
	}

	if (tryPlain && JProgramAvailable(kMoveFileCmd))
	{
		cmd  = kMoveFileCmd;
		cmd += " ";
		cmd += JPrepArgForExec(oldFullName);
		cmd += " ";
		cmd += JPrepArgForExec(newFullName);

		JSimpleProcess* p1;
		err = JSimpleProcess::Create(&p1, cmd);
		p   = p1;
		if (err.OK())
		{
			p->WaitUntilFinished();
			if (!p->SuccessfulFinish())
			{
				err = JAccessDenied(oldFullName, newFullName);
			}
		}
	}
	else if (tryPlain)
	{
		JRenameDirEntry(oldFullName, newFullName, &err);
	}

	jdelete p;
	JChangeDirectory(origPath);
	JGetTemporaryDirectoryChangeMutex().unlock();
	return err;
}

/******************************************************************************
 JRemoveVCS

	if !sync and p != nullptr, p *may* return a process.

 ******************************************************************************/

JError
JRemoveVCS
	(
	const JString&	fullName,
	const bool	sync,
	JProcess**		returnP
	)
{
	if (returnP != nullptr)
	{
		*returnP = nullptr;
	}

	if (!JNameUsed(fullName))
	{
		return JDirEntryDoesNotExist(fullName);
	}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	JGetTemporaryDirectoryChangeMutex().lock();

	const JString origPath = JGetCurrentDirectory();
	if (!JChangeDirectory(path))
	{
		JGetTemporaryDirectoryChangeMutex().unlock();
		return JAccessDenied(path);
	}

	JVCSType type     = JGetVCSType(path);
	JError err        = JNoError();
	bool tryPlain = false;
	JString cmd;
	JProcess* p = nullptr;
	if (type == kJSVNType || type == kJGitType)
	{
		const JUtf8Byte* binary = nullptr;
		if (type == kJSVNType)
		{
			binary = "svn rm --force ";
		}
		else if (type == kJGitType)
		{
			binary = "git rm -rf ";
		}

		cmd  = binary;
		cmd += JPrepArgForExec(name);
		err  = JProcess::Create(&p, cmd);
		if (err.OK())
		{
			p->WaitUntilFinished();
		}

		if (p != nullptr && !p->SuccessfulFinish())
		{
			err      = JAccessDenied(fullName);
			tryPlain = true;
		}
	}
	else if (type == kJUnknownVCSType)
	{
		tryPlain = true;
	}
	else
	{
		err = JUnsupportedVCS(fullName);
	}

	if (tryPlain && JKillDirectory(fullName, sync, returnP))
	{
		err = JNoError();
	}
	else if (tryPlain)
	{
		err = JAccessDenied(fullName);
	}

	jdelete p;
	JChangeDirectory(origPath);
	JGetTemporaryDirectoryChangeMutex().unlock();
	return err;
}

/******************************************************************************
 JGetVCSRepositoryPath

 ******************************************************************************/

static const JString kCVSName1("Root");
static const JString kCVSName2("Repository");

bool
JGetVCSRepositoryPath
	(
	const JString&	origPath,
	JString*		repoPath
	)
{
	JString path = origPath, name;
	if (JFileExists(origPath) ||
		!JDirectoryExists(origPath))	// broken link
	{
		JSplitPathAndName(origPath, &path, &name);
	}

	const JVCSType type = JGetVCSType(path);
	bool found      = false;
	if (type == kJCVSType)
	{
		const JString cvsPath = JCombinePathAndName(path, kCVSDirName);

		JString fullName = JCombinePathAndName(cvsPath, kCVSName1);
		JReadFile(fullName, repoPath);

		fullName = JCombinePathAndName(cvsPath, kCVSName2);
		JString repo;
		JReadFile(fullName, &repo);

		if (!repoPath->IsEmpty() && !repo.IsEmpty())
		{
			*repoPath = JCombinePathAndName(*repoPath, repo);
			found     = true;
		}
	}
	else if (type == kJSVNType)
	{
		int fromFD;
		const JError err = JExecute(origPath,
									"svn info --show-item url", nullptr,
									kJIgnoreConnection, nullptr,
									kJCreatePipe, &fromFD);
		if (err.OK())
		{
			JReadAll(fromFD, repoPath);
			repoPath->TrimWhitespace();
			found = ! repoPath->IsEmpty();
		}
	}

	if (found)
	{
		if (!name.IsEmpty())
		{
			*repoPath = JCombinePathAndName(*repoPath, name);
		}
		return true;
	}
	else
	{
		repoPath->Clear();
		return false;
	}
}

/******************************************************************************
 JGetSVNEntryType

	*** This function makes a synchronous call to the central repository!
	*** It is only useful for command line interaction.

	If the process succeeds, *type is the entry's "kind":  file or dir

	If the process fails, the error output is returned in *error.

 ******************************************************************************/

bool
JGetSVNEntryType
	(
	const JString&	url,
	JString*		type,
	JString*		error
	)
{
	type->Clear();
	error->Clear();

	JString cmd = "svn info --xml " + JPrepArgForExec(url);
	JProcess* p;
	int fromFD, errFD;
	JError err = JProcess::Create(&p, cmd, kJIgnoreConnection, nullptr,
								  kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
	if (!err.OK())
	{
		err.ReportIfError();
		return false;
	}

	p->WaitUntilFinished();
	if (p->SuccessfulFinish())
	{
		xmlDoc* doc = xmlReadFd(fromFD, nullptr, nullptr, XML_PARSE_NOBLANKS | XML_PARSE_NOCDATA);
		close(fromFD);
		if (doc != nullptr)
		{
			xmlNode* root = xmlDocGetRootElement(doc);

			if (root != nullptr &&
				strcmp((char*) root->name, "info") == 0 &&
				strcmp((char*) root->children->name, "entry") == 0)
			{
				*type = JGetXMLNodeAttr(root->children, "kind");
				return true;
			}
		}
	}

	JReadAll(errFD, error, true);
	return false;
}

/******************************************************************************
 JUpdateCVSIgnore

 ******************************************************************************/

static const JString kCVSIgnoreName(".cvsignore");

void
JUpdateCVSIgnore
	(
	const JString& ignoreFullName
	)
{
	JString path, name;
	JSplitPathAndName(ignoreFullName, &path, &name);
	const JString cvsFile = JCombinePathAndName(path, kCVSIgnoreName);

	if (!JFileExists(cvsFile) && JGetVCSType(path) != kJCVSType)
	{
		return;
	}

	JString cvsData;
	JReadFile(cvsFile, &cvsData);
	if (!cvsData.IsEmpty() && !cvsData.EndsWith("\n"))
	{
		cvsData += "\n";
	}

	name += "\n";
	if (!cvsData.Contains(name))
	{
		JEditVCS(cvsFile);
		cvsData += name;

		std::ofstream output(cvsFile.GetBytes());
		cvsData.Print(output);
	}
}

/******************************************************************************
 JGetGitDirectoryName

 ******************************************************************************/

const JString&
JGetGitDirectoryName()
{
	return kGitDirName;
}

/******************************************************************************
 JSearchGitRoot

	Search directory tree up to root.

 ******************************************************************************/

bool
JSearchGitRoot
	(
	const JString&	path,
	JString*		gitRoot
	)
{
	return jSearchVCSRoot(path, kGitDirName, gitRoot);
}

/******************************************************************************
 JUnsupportedVCS

 ******************************************************************************/

JUnsupportedVCS::JUnsupportedVCS
	(
	const JString& fullName
	)
	:
	JError(kJUnsupportedVCS)
{
	const JUtf8Byte* map[] =
	{
		"file", fullName.GetBytes()
	};
	SetMessage(map, sizeof(map));
}

/******************************************************************************
 jSearchVCSRoot

	Search directory tree up to root.

 ******************************************************************************/

static bool
jSearchVCSRoot
	(
	const JString&	path,
	const JString&	vcsDirName,
	JString*		vcsRoot
	)
{
	JString p = path, n;
	if (JFileExists(path) ||
		!JDirectoryExists(path))	// broken link
	{
		JSplitPathAndName(path, &p, &n);
	}

	do
	{
		n = JCombinePathAndName(p, vcsDirName);
		if (JDirectoryExists(n))
		{
			*vcsRoot = p;
			return true;
		}

		JSplitPathAndName(p, &p, &n);
	}
	while (!JIsRootDirectory(p));

	vcsRoot->Clear();
	return false;
}
