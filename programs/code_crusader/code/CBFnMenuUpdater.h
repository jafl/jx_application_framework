/******************************************************************************
 CBFnMenuUpdater.h

	Copyright © 1999-2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFnMenuUpdater
#define _H_CBFnMenuUpdater

#include <JPrefObject.h>
#include "CBCtagsUser.h"

class JXTextMenu;

class CBFnMenuUpdater : public JPrefObject, public CBCtagsUser
{
public:

	CBFnMenuUpdater();

	virtual ~CBFnMenuUpdater();

	JBoolean	CanCreateMenu(const CBTextFileType type) const;

	CBLanguage	UpdateMenu(const JString& fileName, const CBTextFileType fileType,
						   const JBoolean sort, const JBoolean includeNS,
						   const JBoolean pack,
						   JXTextMenu* menu, JArray<JIndex>* lineIndexList);

	JBoolean	WillSortFnNames() const;
	void		ShouldSortFnNames(const JBoolean sort);

	JBoolean	WillIncludeNamespace() const;
	void		ShouldIncludeNamespace(const JBoolean includeNS);

	JBoolean	WillPackFnNames() const;
	void		ShouldPackFnNames(const JBoolean pack);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	JBoolean	itsSortFlag;
	JBoolean	itsIncludeNSFlag;
	JBoolean	itsPackFlag;

private:

	void	ReadFunctionList(std::istream& input, CBLanguage lang,
							 const JBoolean sort, const JBoolean includeNS,
							 JXTextMenu* menu, JArray<JIndex>* lineIndexList);

	// not allowed

	CBFnMenuUpdater(const CBFnMenuUpdater& source);
	const CBFnMenuUpdater& operator=(const CBFnMenuUpdater& source);
};


/******************************************************************************
 CanCreateMenu

 ******************************************************************************/

inline JBoolean
CBFnMenuUpdater::CanCreateMenu
	(
	const CBTextFileType type
	)
	const
{
	return JI2B(IsActive() && IsParsedForFunctionMenu(type));
}

/******************************************************************************
 Sorting

	This flag determines the default setting used by CBFunctionMenu.

 ******************************************************************************/

inline JBoolean
CBFnMenuUpdater::WillSortFnNames()
	const
{
	return itsSortFlag;
}

inline void
CBFnMenuUpdater::ShouldSortFnNames
	(
	const JBoolean sort
	)
{
	itsSortFlag = sort;
}

/******************************************************************************
 Namespace

	This flag determines the default setting used by CBFunctionMenu.

 ******************************************************************************/

inline JBoolean
CBFnMenuUpdater::WillIncludeNamespace()
	const
{
	return itsIncludeNSFlag;
}

inline void
CBFnMenuUpdater::ShouldIncludeNamespace
	(
	const JBoolean includeNS
	)
{
	itsIncludeNSFlag = includeNS;
}

/******************************************************************************
 Packing

	Use a smaller font size and pack the menu tightly.

 ******************************************************************************/

inline JBoolean
CBFnMenuUpdater::WillPackFnNames()
	const
{
	return itsPackFlag;
}

inline void
CBFnMenuUpdater::ShouldPackFnNames
	(
	const JBoolean pack
	)
{
	itsPackFlag = pack;
}

#endif
