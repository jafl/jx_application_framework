/******************************************************************************
 JColorList.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JColorList
#define _H_JColorList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JArray.h>
#include <jColor.h>

class JColormap;

class JColorList : public JContainer
{
public:

	JColorList(JColormap* colormap);
	JColorList(const JColorList& source);

	virtual ~JColorList();

	JColormap*	GetColormap() const;

	JColorIndex	Add(const JRGB& color);
	JColorIndex	Add(const JHSV& color);
	JColorIndex	Add(const JCharacter* name);
	void		Add(const JColorIndex colorIndex);
	void		Remove(const JColorIndex colorIndex);
	void		RemoveAll();

private:

	JColormap*				itsColormap;	// not owned
	JArray<JColorIndex>*	itsColorList;	// can be NULL

private:

	void	CreateColorList();

	// not allowed

	const JColorList& operator=(const JColorList& source);
};


/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JColormap*
JColorList::GetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 Add

 ******************************************************************************/

inline JColorIndex
JColorList::Add
	(
	const JHSV& color
	)
{
	return Add(JRGB(color));
}

#endif
