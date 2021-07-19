/******************************************************************************
 CBDirList.cpp

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBDirList.h"
#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDirList::CBDirList()
	:
	JContainer()
{
	CBDirListX();

	AddPath(JString("./", JString::kNoCopy), true);
}

// private

void
CBDirList::CBDirListX()
{
	itsDirList = jnew CBDirInfoList;
	assert( itsDirList != nullptr );
	itsDirList->SetSortOrder(JListT::kSortAscending);
	itsDirList->SetCompareFunction(CBDirInfo::ComparePathNames);

	InstallCollection(itsDirList);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

CBDirList::CBDirList
	(
	const CBDirList& source
	)
	:
	JContainer(),
	itsBasePath(source.itsBasePath)
{
	CBDirListX();
	CopyPaths(source);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDirList::~CBDirList()
{
	itsDirList->DeleteAll();
	jdelete itsDirList;
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const CBDirList&
CBDirList::operator=
	(
	const CBDirList& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	JContainer::operator=(source);

	itsBasePath = source.itsBasePath;
	CopyPaths(source);

	return *this;
}

/******************************************************************************
 CopyPaths (private)

 ******************************************************************************/

void
CBDirList::CopyPaths
	(
	const CBDirList& source
	)
{
	itsDirList->DeleteAll();

	const JSize count = (source.itsDirList)->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CBDirInfo info = (source.itsDirList)->GetElement(i);
		AddPath(*(info.path), info.recurse);
		}
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
CBDirList::GetPath
	(
	const JIndex	index,
	bool*		recurse
	)
	const
{
	const CBDirInfo info = itsDirList->GetElement(index);
	*recurse             = info.recurse;
	return *(info.path);
}

/******************************************************************************
 GetFullPath

 ******************************************************************************/

bool
CBDirList::GetFullPath
	(
	const JIndex	index,
	JString*		fullPath,
	bool*		recurse
	)
	const
{
	const CBDirInfo info = itsDirList->GetElement(index);
	*recurse             = info.recurse;
	return JConvertToAbsolutePath(*(info.path), itsBasePath, fullPath);
}

/******************************************************************************
 GetTruePath

 ******************************************************************************/

bool
CBDirList::GetTruePath
	(
	const JIndex	index,
	JString*		truePath,
	bool*		recurse
	)
	const
{
	JString path;
	return GetFullPath(index, &path, recurse) &&
				JGetTrueName(path, truePath);
}

/******************************************************************************
 AddPath

 ******************************************************************************/

void
CBDirList::AddPath
	(
	const JString&	path,
	const bool	recurse
	)
{
	CBDirInfo info(jnew JString(path), recurse);
	assert( info.path != nullptr );
	itsDirList->InsertSorted(info);
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is stored.  It must be an absolute path.

 ******************************************************************************/

void
CBDirList::SetBasePath
	(
	const JString& path
	)
{
	assert( JIsAbsolutePath(path) );
	itsBasePath = path;
}

/******************************************************************************
 Contains

	Returns true if the given item is contained in our list of paths.

 ******************************************************************************/

bool
CBDirList::Contains
	(
	const JString& p
	)
	const
{
	if (p.IsEmpty())
		{
		return false;
		}

	JString path = p, name;
	if (path.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		JSplitPathAndName(p, &path, &name);
		}

	const JSize count = GetElementCount();
	JString truePath;
	bool recurse;
	for (JIndex i=1; i<=count; i++)
		{
		if (GetTruePath(i, &truePath, &recurse) &&
			(( recurse && path.BeginsWith(truePath)) ||
			 (!recurse && path == truePath)))
			{
			return true;
			}
		}

	return false;
}

/******************************************************************************
 ReadDirectories

 ******************************************************************************/

void
CBDirList::ReadDirectories
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	itsDirList->DeleteAll();

	if (vers >= 71)
		{
		input >> std::ws;
		JIgnoreLine(input);
		}

	if (vers <= 71)
		{
		JSize dirCount;
		input >> dirCount;

		for (JIndex i=1; i<=dirCount; i++)
			{
			ReadDirectory(input, vers);
			}
		}
	else
		{
		while (true)
			{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
				{
				break;
				}

			ReadDirectory(input, vers);
			}
		}
}

/******************************************************************************
 ReadDirectory (private)

 ******************************************************************************/

void
CBDirList::ReadDirectory
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	JString path;
	bool recurse = true;

	input >> path;
	if (vers >= 21)
		{
		input >> JBoolFromString(recurse);
		}
	AddPath(path, recurse);
}

/******************************************************************************
 WriteDirectories

 ******************************************************************************/

void
CBDirList::WriteDirectories
	(
	std::ostream& output
	)
	const
{
	output << "# search paths\n";

	const JSize dirCount = GetElementCount();
	for (JIndex i=1; i<=dirCount; i++)
		{
		output << JBoolToString(true);

		const CBDirInfo info = itsDirList->GetElement(i);
		output << ' ' << *(info.path);
		output << ' ' << JBoolToString(info.recurse);
		output << '\n';
		}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 operator==

	We know that CBDirList objects are always sorted.

 ******************************************************************************/

int
operator==
	(
	const CBDirList& l1,
	const CBDirList& l2
	)
{
	if (l1.GetBasePath() != l2.GetBasePath())
		{
		return false;
		}

	bool sameDirs = false;
	const JSize count = l1.GetElementCount();
	if (count == l2.GetElementCount())
		{
		sameDirs = true;
		for (JIndex i=1; i<=count; i++)
			{
			bool r1, r2;
			if (l1.GetPath(i, &r1) != l2.GetPath(i, &r2) ||
				r1 != r2)
				{
				sameDirs = false;
				break;
				}
			}
		}

	return sameDirs;
}

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
CBDirInfoList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		jdelete (GetElement(i)).path;
		}
	RemoveAll();
}

/******************************************************************************
 ComparePathNames (static)

	UNIX path names are case-sensitive.

 ******************************************************************************/

JListT::CompareResult
CBDirInfo::ComparePathNames
	(
	const CBDirInfo& i1,
	const CBDirInfo& i2
	)
{
	return JCompareStringsCaseSensitive(i1.path, i2.path);
}

/******************************************************************************
 CompareProjIndex (static)

 ******************************************************************************/

JListT::CompareResult
CBDirInfo::CompareProjIndex
	(
	const CBDirInfo& i1,
	const CBDirInfo& i2
	)
{
	if (i1.projIndex < i2.projIndex)
		{
		return JListT::kFirstLessSecond;
		}
	else if (i1.projIndex > i2.projIndex)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return ComparePathNames(i1, i2);
		}
}
