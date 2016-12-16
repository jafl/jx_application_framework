/******************************************************************************
 CBDirList.cpp

	Copyright (C) 1999 by John Lindal. All rights reserved.

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

	AddPath("./", kJTrue);
}

// private

void
CBDirList::CBDirListX()
{
	itsDirList = jnew CBDirInfoList;
	assert( itsDirList != NULL );
	itsDirList->SetSortOrder(JOrderedSetT::kSortAscending);
	itsDirList->SetCompareFunction(CBDirInfo::ComparePathNames);

	InstallOrderedSet(itsDirList);
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
	JBoolean*		recurse
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

JBoolean
CBDirList::GetFullPath
	(
	const JIndex	index,
	JString*		fullPath,
	JBoolean*		recurse
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

JBoolean
CBDirList::GetTruePath
	(
	const JIndex	index,
	JString*		truePath,
	JBoolean*		recurse
	)
	const
{
	JString path;
	return JI2B(GetFullPath(index, &path, recurse) &&
				JGetTrueName(path, truePath));
}

/******************************************************************************
 AddPath

 ******************************************************************************/

void
CBDirList::AddPath
	(
	const JCharacter*	path,
	const JBoolean		recurse
	)
{
	CBDirInfo info(jnew JString(path), recurse);
	assert( info.path != NULL );
	itsDirList->InsertSorted(info);
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is stored.  It must be an absolute path.

 ******************************************************************************/

void
CBDirList::SetBasePath
	(
	const JCharacter* path
	)
{
	assert( JIsAbsolutePath(path) );
	itsBasePath = path;
}

/******************************************************************************
 Contains

	Returns kJTrue if the given item is contained in our list of paths.

 ******************************************************************************/

JBoolean
CBDirList::Contains
	(
	const JCharacter* p
	)
	const
{
	if (JStringEmpty(p))
		{
		return kJFalse;
		}

	JString path = p, name;
	if (path.GetLastCharacter() != ACE_DIRECTORY_SEPARATOR_CHAR)
		{
		JSplitPathAndName(p, &path, &name);
		}

	const JSize count = GetElementCount();
	JString truePath;
	JBoolean recurse;
	for (JIndex i=1; i<=count; i++)
		{
		if (GetTruePath(i, &truePath, &recurse) &&
			(( recurse && path.BeginsWith(truePath)) ||
			 (!recurse && path == truePath)))
			{
			return kJTrue;
			}
		}

	return kJFalse;
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
		while (1)
			{
			JBoolean keepGoing;
			input >> keepGoing;
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
	JBoolean recurse = kJTrue;

	input >> path;
	if (vers >= 21)
		{
		input >> recurse;
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
		output << kJTrue;

		const CBDirInfo info = itsDirList->GetElement(i);
		output << ' ' << *(info.path);
		output << ' ' << info.recurse;
		output << '\n';
		}

	output << kJFalse << '\n';
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
		return kJFalse;
		}

	JBoolean sameDirs = kJFalse;
	const JSize count = l1.GetElementCount();
	if (count == l2.GetElementCount())
		{
		sameDirs = kJTrue;
		for (JIndex i=1; i<=count; i++)
			{
			JBoolean r1, r2;
			if (l1.GetPath(i, &r1) != l2.GetPath(i, &r2) ||
				r1 != r2)
				{
				sameDirs = kJFalse;
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

JOrderedSetT::CompareResult
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

JOrderedSetT::CompareResult
CBDirInfo::CompareProjIndex
	(
	const CBDirInfo& i1,
	const CBDirInfo& i2
	)
{
	if (i1.projIndex < i2.projIndex)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (i1.projIndex > i2.projIndex)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return ComparePathNames(i1, i2);
		}
}
