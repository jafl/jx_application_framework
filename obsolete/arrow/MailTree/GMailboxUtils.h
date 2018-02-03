/******************************************************************************
 GMailboxUtils.h

	Utility functions for dealing with files.

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMailboxUtils
#define _H_GMailboxUtils

#include <jTypes.h>

class JString;
class JDirEntry;

JString	JGetFileRoot(const JCharacter* file);
JString	JGetFileSuffix(const JCharacter* file);
JString	JGetURLFromFile(const JCharacter* file);
JBoolean	JGetFileFromURL(const JCharacter* url, JString* file);
void		JAbsoluteToRelativePath(const JCharacter* relativeTo, JString* path);
void		JRelativeToAbsolutePath(const JCharacter* relativeTo, JString* path);
JBoolean	MatchesCookie(const JCharacter* cookie, const JDirEntry& entry);

#endif
