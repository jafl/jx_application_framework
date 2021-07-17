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

	bool	CanCreateMenu(const CBTextFileType type) const;

	void		UpdateMenu(const JString& fileName, const CBTextFileType fileType,
						   const bool sort, const bool includeNS,
						   const bool pack,
						   JXTextMenu* menu, JArray<JIndex>* lineIndexList,
						   JArray<CBLanguage>* lineLangList);

	bool	WillSortFnNames() const;
	void		ShouldSortFnNames(const bool sort);

	bool	WillIncludeNamespace() const;
	void		ShouldIncludeNamespace(const bool includeNS);

	bool	WillPackFnNames() const;
	void		ShouldPackFnNames(const bool pack);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	bool	itsSortFlag;
	bool	itsIncludeNSFlag;
	bool	itsPackFlag;

private:

	void	ReadFunctionList(std::istream& input, const CBLanguage lang,
							 const bool sort, const bool includeNS,
							 JPtrArray<JString>* fnNameList,
							 JArray<JIndex>* lineIndexList,
							 JArray<CBLanguage>* lineLangList);

	// not allowed

	CBFnMenuUpdater(const CBFnMenuUpdater& source);
	const CBFnMenuUpdater& operator=(const CBFnMenuUpdater& source);
};


/******************************************************************************
 CanCreateMenu

 ******************************************************************************/

inline bool
CBFnMenuUpdater::CanCreateMenu
	(
	const CBTextFileType type
	)
	const
{
	return IsActive() && IsParsedForFunctionMenu(type);
}

/******************************************************************************
 Sorting

	This flag determines the default setting used by CBFunctionMenu.

 ******************************************************************************/

inline bool
CBFnMenuUpdater::WillSortFnNames()
	const
{
	return itsSortFlag;
}

inline void
CBFnMenuUpdater::ShouldSortFnNames
	(
	const bool sort
	)
{
	itsSortFlag = sort;
}

/******************************************************************************
 Namespace

	This flag determines the default setting used by CBFunctionMenu.

 ******************************************************************************/

inline bool
CBFnMenuUpdater::WillIncludeNamespace()
	const
{
	return itsIncludeNSFlag;
}

inline void
CBFnMenuUpdater::ShouldIncludeNamespace
	(
	const bool includeNS
	)
{
	itsIncludeNSFlag = includeNS;
}

/******************************************************************************
 Packing

	Use a smaller font size and pack the menu tightly.

 ******************************************************************************/

inline bool
CBFnMenuUpdater::WillPackFnNames()
	const
{
	return itsPackFlag;
}

inline void
CBFnMenuUpdater::ShouldPackFnNames
	(
	const bool pack
	)
{
	itsPackFlag = pack;
}

#endif
