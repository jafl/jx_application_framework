/******************************************************************************
 CBDirList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDirList
#define _H_CBDirList

#include <JContainer.h>
#include <JPtrArray-JString.h>

struct CBDirInfo
{
	JString*	path;
	bool	recurse;
	JIndex		projIndex;	// for use by CBApp::CollectSearchPaths()

	CBDirInfo()
		:
		path(nullptr), recurse(true), projIndex(0)
	{ };

	CBDirInfo(JString* p, const bool r)
		:
		path(p), recurse(r), projIndex(0)
	{ };

	static JListT::CompareResult
		ComparePathNames(const CBDirInfo& i1, const CBDirInfo& i2);
	static JListT::CompareResult
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

	bool	GetFullPath(const JIndex index, JString* fullPath,
							bool* recurse) const;
	bool	GetTruePath(const JIndex index, JString* truePath,
							bool* recurse) const;

	const JString&	GetPath(const JIndex index, bool* recurse) const;
	void			AddPath(const JString& path, const bool recurse);
	void			RemoveAll();

	const JString&	GetBasePath() const;
	void			SetBasePath(const JString& path);

	bool		Contains(const JString& path) const;

	void	ReadDirectories(std::istream& input, const JFileVersion vers);
	void	WriteDirectories(std::ostream& output) const;

private:

	CBDirInfoList*	itsDirList;
	JString			itsBasePath;

private:

	void	CBDirListX();
	void	ReadDirectory(std::istream& input, const JFileVersion vers);
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
