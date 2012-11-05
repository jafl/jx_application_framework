/******************************************************************************
 CBDirList.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDirList
#define _H_CBDirList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JPtrArray-JString.h>

struct CBDirInfo
{
	JString*	path;
	JBoolean	recurse;
	JIndex		projIndex;	// for use by CBApp::CollectSearchPaths()

	CBDirInfo()
		:
		path(NULL), recurse(kJTrue), projIndex(0)
	{ };

	CBDirInfo(JString* p, const JBoolean r)
		:
		path(p), recurse(r), projIndex(0)
	{ };

	static JOrderedSetT::CompareResult
		ComparePathNames(const CBDirInfo& i1, const CBDirInfo& i2);
	static JOrderedSetT::CompareResult
		CompareProjIndex(const CBDirInfo& i1, const CBDirInfo& i2);
};

class CBDirInfoList : public JArray<CBDirInfo>
{
public:

	void	DeleteAll();
};

class CBDirList : public JContainer
{
public:

	CBDirList();
	CBDirList(const CBDirList& source);

	virtual ~CBDirList();

	const CBDirList& operator=(const CBDirList& source);

	JBoolean	GetFullPath(const JIndex index, JString* fullPath,
							JBoolean* recurse) const;
	JBoolean	GetTruePath(const JIndex index, JString* truePath,
							JBoolean* recurse) const;

	const JString&	GetPath(const JIndex index, JBoolean* recurse) const;
	void			AddPath(const JCharacter* path, const JBoolean recurse);
	void			RemoveAll();

	const JString&	GetBasePath() const;
	void			SetBasePath(const JCharacter* path);

	JBoolean		Contains(const JCharacter* path) const;

	void	ReadDirectories(istream& input, const JFileVersion vers);
	void	WriteDirectories(ostream& output) const;

private:

	CBDirInfoList*	itsDirList;
	JString			itsBasePath;

private:

	void	CBDirListX();
	void	ReadDirectory(istream& input, const JFileVersion vers);
	void	CopyPaths(const CBDirList& source);
};


// global functions for working with CBDirList

int operator==(const CBDirList& l1, const CBDirList& l2);


/******************************************************************************
 RemoveAll

 ******************************************************************************/

inline void
CBDirList::RemoveAll()
{
	itsDirList->DeleteAll();
}

/******************************************************************************
 GetBasePath

 ******************************************************************************/

inline const JString&
CBDirList::GetBasePath()
	const
{
	return itsBasePath;
}

/******************************************************************************
 operator!=

 ******************************************************************************/

inline int
operator!=
	(
	const CBDirList& l1,
	const CBDirList& l2
	)
{
	return !(l1 == l2);
}

#endif
