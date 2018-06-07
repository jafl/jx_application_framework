/******************************************************************************
 jVCSUtil.h

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_jVCSUtil
#define _H_jVCSUtil

#include <JError.h>

class JString;
class JProcess;

enum JVCSType
{
	kJUnknownVCSType = 0,
	kJSCCSType,
	kJCVSType,
	kJSVNType,
	kJGitType
};

class JUnsupportedVCS : public JError
{
public:

	JUnsupportedVCS(const JString& fullName);
};

JBoolean	JIsVCSDirectory(const JString& name);
JSize		JGetVCSDirectoryNames(const JUtf8Byte*** dirNames);
JVCSType	JGetVCSType(const JString& path, const JBoolean deepInspection = kJFalse);
JBoolean	JIsManagedByVCS(const JString& fullName, JVCSType* returnType = nullptr);

void		JEditVCS(const JString& fullName);
JError		JRenameVCS(const JString& oldFullName, const JString& newFullName);
JError		JRemoveVCS(const JString& fullName, const JBoolean sync = kJTrue,
					   JProcess** p = nullptr);

JBoolean	JGetVCSRepositoryPath(const JString& path, JString* repoPath);

// SVN specific

JBoolean	JGetCurrentSVNRevision(const JString& fullName, JString* rev);
JBoolean	JGetSVNEntryType(const JString& url, JString* type, JString* error);

// CVS specific

void		JUpdateCVSIgnore(const JString& ignoreFullName);

// git specific

JBoolean	JSearchGitRoot(const JString& path, JString* gitRoot);

#endif
