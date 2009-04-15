/******************************************************************************
 jVCSUtil.cpp

	Routines to help with Version Control Systems.

	Copyright © 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jVCSUtil.h>
#include <JRegex.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <strstream>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kGitDirName         = ".git";
static const JCharacter* kSubversionDirName  = ".svn";
static const JCharacter* kCVSDirName         = "CVS";
static const JCharacter* kSCCSDirName        = "SCCS";

// error types

const JCharacter* kJUnsupportedVCS = "JUnsupportedVCS";

// string ID's

static const JCharacter* kAskForceSVNMoveID   = "AskForceSVNMove::VCSUtil";
static const JCharacter* kAskPlainSVNMoveID   = "AskPlainSVNMove::VCSUtil";
static const JCharacter* kAskForceGitMoveID   = "AskForceGitMove::VCSUtil";
static const JCharacter* kAskPlainGitMoveID   = "AskPlainGitMove::VCSUtil";
static const JCharacter* kAskForceSVNRemoveID = "AskForceSVNRemove::VCSUtil";
static const JCharacter* kAskPlainSVNRemoveID = "AskPlainSVNRemove::VCSUtil";
static const JCharacter* kAskForceGitRemoveID = "AskForceGitRemove::VCSUtil";
static const JCharacter* kAskPlainGitRemoveID = "AskPlainGitRemove::VCSUtil";
static const JCharacter* kAskPlainCVSRemoveID = "AskPlainCVSRemove::VCSUtil";

/******************************************************************************
 JIsVCSDirectory

 ******************************************************************************/

JBoolean
JIsVCSDirectory
	(
	const JCharacter* name
	)
{
	return JI2B(strcmp(name, kGitDirName)        == 0 ||
				strcmp(name, kSubversionDirName) == 0 ||
				strcmp(name, kCVSDirName)        == 0 ||
				strcmp(name, kSCCSDirName)       == 0);
}

/******************************************************************************
 JGetVCSType

 ******************************************************************************/

JVCSType
JGetVCSType
	(
	const JCharacter* path
	)
{
	JString p = path, n;
	if (JFileExists(path))
		{
		JSplitPathAndName(path, &p, &n);
		}

	JString vcsDir = JCombinePathAndName(p, kSubversionDirName);
	if (JDirectoryExists(vcsDir))
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

	// check git last, since it needs to search directory tree up to root

	do
		{
		vcsDir = JCombinePathAndName(p, kGitDirName);
		if (JDirectoryExists(vcsDir))
			{
			return kJGitType;
			}

		JSplitPathAndName(p, &p, &n);
		}
		while (!JIsRootDirectory(p));

	return kJUnknownVCSType;
}

/******************************************************************************
 JEditVCS

 ******************************************************************************/

void
JEditVCS
	(
	const JCharacter* fullName
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
			system(cmd);
			}
		}
}

/******************************************************************************
 JRenameVCS

 ******************************************************************************/

JError
JRenameVCS
	(
	const JCharacter* oldFullName,
	const JCharacter* newFullName
	)
{
	if (!JNameUsed(oldFullName))
		{
		return JDirEntryDoesNotExist(oldFullName);
		}

	JString oldPath, newPath, name;
	JSplitPathAndName(newFullName, &newPath, &name);
	JSplitPathAndName(oldFullName, &oldPath, &name);	// must be second

	const JString origPath = JGetCurrentDirectory();
	if (JChangeDirectory(oldPath) != kJNoError)
		{
		return JAccessDenied(oldPath);
		}

	JVCSType type1    = JGetVCSType(oldPath);
	JVCSType type2    = JGetVCSType(newPath);
	JError err        = JNoError();
	JBoolean tryPlain = kJFalse;
	JString cmd;
	JSimpleProcess* p = NULL;
	if (type1 != type2)
		{
		tryPlain = kJTrue;
		}
	else if (type1 == kJSVNType || type1 == kJGitType)
		{
		const JCharacter *binary, *forceArg, *forceMsg, *plainMsg;
		if (type1 == kJSVNType)
			{
			binary   = "svn mv ";
			forceArg = " --force";
			forceMsg = kAskForceSVNMoveID;
			plainMsg = kAskPlainSVNMoveID;
			}
		else if (type1 == kJGitType)
			{
			binary   = "git mv ";
			forceArg = " -f";
			forceMsg = kAskForceGitMoveID;
			plainMsg = kAskPlainGitMoveID;
			}

		cmd  = binary;
		cmd += JPrepArgForExec(name);
		cmd += " ";
		cmd += JPrepArgForExec(newFullName);

		err = JSimpleProcess::Create(&p, cmd);
		if (err.OK())
			{
			p->WaitUntilFinished();
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err = JAccessDenied(oldFullName, newFullName);
			if ((JGetUserNotification())->AskUserYes(JGetString(forceMsg)))
				{
				delete p;

				cmd += forceArg;
				err = JSimpleProcess::Create(&p, cmd);
				if (err.OK())
					{
					p->WaitUntilFinished();
					}
				}
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err = JAccessDenied(oldFullName, newFullName);
			if ((JGetUserNotification())->AskUserYes(JGetString(plainMsg)))
				{
				tryPlain = kJTrue;
				}
			}
		}
	else if (type1 == kJUnknownVCSType)
		{
		tryPlain = kJTrue;
		}
	else
		{
		err = JUnsupportedVCS(oldFullName);
		}

	if (tryPlain && JProgramAvailable("mv"))
		{
		cmd  = "mv ";
		cmd += JPrepArgForExec(oldFullName);
		cmd += " ";
		cmd += JPrepArgForExec(newFullName);

		err = JSimpleProcess::Create(&p, cmd);
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
		err = JRenameDirEntry(oldFullName, newFullName);
		}

	delete p;
	JChangeDirectory(origPath);
	return err;
}

/******************************************************************************
 JRemoveVCS

 ******************************************************************************/

JError
JRemoveVCS
	(
	const JCharacter* fullName
	)
{
	if (!JNameUsed(fullName))
		{
		return JDirEntryDoesNotExist(fullName);
		}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JString origPath = JGetCurrentDirectory();
	if (JChangeDirectory(path) != kJNoError)
		{
		return JAccessDenied(path);
		}

	JVCSType type     = JGetVCSType(path);
	JError err        = JNoError();
	JBoolean tryPlain = kJFalse;
	JString cmd;
	JSimpleProcess* p = NULL;
	if (type == kJSVNType || type == kJGitType)
		{
		const JCharacter *binary, *forceArg, *forceMsg, *plainMsg;
		if (type == kJSVNType)
			{
			binary   = "svn rm ";
			forceArg = " --force";
			forceMsg = kAskForceSVNRemoveID;
			plainMsg = kAskPlainSVNRemoveID;
			}
		else if (type == kJGitType)
			{
			binary   = "git rm -r ";
			forceArg = " -f";
			forceMsg = kAskForceGitRemoveID;
			plainMsg = kAskPlainGitRemoveID;
			}

		cmd  = binary;
		cmd += JPrepArgForExec(name);

		err = JSimpleProcess::Create(&p, cmd);
		if (err.OK())
			{
			p->WaitUntilFinished();
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err = JAccessDenied(fullName);
			if ((JGetUserNotification())->AskUserYes(JGetString(forceMsg)))
				{
				delete p;

				cmd += forceArg;
				err = JSimpleProcess::Create(&p, cmd);
				if (err.OK())
					{
					p->WaitUntilFinished();
					}
				}
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err = JAccessDenied(fullName);
			if ((JGetUserNotification())->AskUserYes(JGetString(plainMsg)))
				{
				tryPlain = kJTrue;
				}
			}
		}
	else if (type == kJCVSType)
		{
		cmd  = "cvs remove -f ";
		cmd += JPrepArgForExec(name);

		err = JSimpleProcess::Create(&p, cmd);
		if (err.OK())
			{
			p->WaitUntilFinished();
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err = JAccessDenied(fullName);
			if ((JGetUserNotification())->AskUserYes(JGetString(kAskPlainCVSRemoveID)))
				{
				tryPlain = kJTrue;
				}
			}
		}
	else if (type == kJUnknownVCSType)
		{
		tryPlain = kJTrue;
		}
	else
		{
		err = JUnsupportedVCS(fullName);
		}

	if (tryPlain && JKillDirectory(fullName))
		{
		err = JNoError();
		}
	else if (tryPlain)
		{
		err = JAccessDenied(fullName);
		}

	delete p;
	JChangeDirectory(origPath);
	return err;
}

/******************************************************************************
 JGetVCSRepositoryPath

 ******************************************************************************/

static const JRegex svn4RepositoryPattern1 = "<entry[^>]+name=\"\"(.|\n)*?>";
static const JRegex svn4RepositoryPattern2 = "<entry[^>]+url=\"([^\"]+)\"";

JBoolean
JGetVCSRepositoryPath
	(
	const JCharacter*	path,
	JString*			repoPath
	)
{
	const JVCSType type = JGetVCSType(path);
	if (type == kJCVSType)
		{
		const JString cvsPath = JCombinePathAndName(path, kCVSDirName);

		JString fullName = JCombinePathAndName(cvsPath, "Root");
		JReadFile(fullName, repoPath);

		fullName = JCombinePathAndName(cvsPath, "Repository");
		JString repo;
		JReadFile(fullName, &repo);

		if (!repoPath->IsEmpty() && !repo.IsEmpty())
			{
			*repoPath = JCombinePathAndName(*repoPath, repo);
			return kJTrue;
			}
		}
	else if (type == kJSVNType)
		{
		JString entriesFileName, data;
		entriesFileName = JCombinePathAndName(path, kSubversionDirName);
		entriesFileName = JCombinePathAndName(entriesFileName, "entries");
		JReadFile(entriesFileName, &data);

		if (data.BeginsWith("<?xml"))
			{
			JIndexRange range;
			JArray<JIndexRange> matchList;
			if (svn4RepositoryPattern1.Match(data, &range) &&
				svn4RepositoryPattern2.MatchWithin(data, range, &matchList))
				{
				*repoPath = data.GetSubstring(matchList.GetElement(2));
				return kJTrue;
				}
			}
		else
			{
			std::istrstream input(data, data.GetLength());

			const JString version = JReadLine(input);
			if (version == "8" || version == "9")
				{
				JIgnoreLine(input);		// ???
				JIgnoreLine(input);		// dir
				JIgnoreLine(input);		// latest update version

				*repoPath = JReadLine(input);
				return JI2B(input.good());
				}
			}
		}

	repoPath->Clear();
	return kJFalse;
}

/******************************************************************************
 JGetCurrentSVNRevision

 ******************************************************************************/

static const JRegex svn4RevisionPattern = "<entry[^>]+committed-rev=\"([^\"]+)\"";

JBoolean
JGetCurrentSVNRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	JString path, name, entriesFileName, data, pattern;
	JSplitPathAndName(fullName, &path, &name);
	entriesFileName = JCombinePathAndName(path, ".svn");
	entriesFileName = JCombinePathAndName(entriesFileName, "entries");
	JReadFile(entriesFileName, &data);

	if (data.BeginsWith("<?xml"))
		{
		pattern = "<entry[^>]+name=\"" + JRegex::BackslashForLiteral(name) + "\"(.|\n)*?>";
		JRegex r(pattern);
		JIndexRange range;
		JArray<JIndexRange> matchList;
		if (r.Match(data, &range) &&
			svn4RevisionPattern.MatchWithin(data, range, &matchList))
			{
			*rev = data.GetSubstring(matchList.GetElement(2));
			return kJTrue;
			}
		}
	else
		{
		std::istrstream input(data, data.GetLength());

		const JString version = JReadLine(input);
		if (version == "8" || version == "9")
			{
			pattern = "\n\f\n" + name + "\n";
			JIgnoreUntil(input, pattern);

			JIgnoreLine(input);		// file
			JIgnoreLine(input);		// ???
			JIgnoreLine(input);		// ???
			JIgnoreLine(input);		// ???
			JIgnoreLine(input);		// ???
			JIgnoreLine(input);		// timestamp
			JIgnoreLine(input);		// hash
			JIgnoreLine(input);		// timestamp

			*rev = JReadLine(input);
			return JI2B(input.good());
			}
		}

	rev->Clear();
	return kJFalse;
}

/******************************************************************************
 JUpdateCVSIgnore

 ******************************************************************************/

void
JUpdateCVSIgnore
	(
	const JCharacter* ignoreFullName
	)
{
	JString path, name;
	JSplitPathAndName(ignoreFullName, &path, &name);
	const JString cvsFile = JCombinePathAndName(path, ".cvsignore");

	if (!JFileExists(cvsFile))
		{
		// We cannot just check for CVS, because they might be planning to
		// add it to CVS later.

		JString svnDir  = JCombinePathAndName(path, kSubversionDirName);
		JString sccsDir = JCombinePathAndName(path, kSCCSDirName);
		if (JDirectoryExists(svnDir) ||
			JDirectoryExists(sccsDir))
			{
			return;
			}
		}

	JString cvsData;
	JReadFile(cvsFile, &cvsData);

	name += "\n";
	if (!cvsData.Contains(name))
		{
		JEditVCS(cvsFile);

		// append new data

		if (!cvsData.IsEmpty() && !cvsData.EndsWith("\n"))
			{
			cvsData += "\n";
			}
		cvsData += name;

		// write result

		ofstream output(cvsFile);
		cvsData.Print(output);
		}
}

/******************************************************************************
 JUpdateCVSIgnore

 ******************************************************************************/

JUnsupportedVCS::JUnsupportedVCS
	(
	const JCharacter* fullName
	)
	:
	JError(kJUnsupportedVCS, "")
{
	const JCharacter* map[] =
		{
		"file", fullName
		};
	SetMessage(map, sizeof(map));
}
