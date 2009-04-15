/******************************************************************************
 EditTableDir.h

	Interface for the EditTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_EditTableDir
#define _H_EditTableDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JArray.h>


class EditTableDir : public JXWindowDirector
{
public:

	EditTableDir(JXDirector* supervisor);

	virtual ~EditTableDir();

private:

	JArray<JIndex>* itsData;

private:

	void BuildWindow();

	// not allowed

	EditTableDir(const EditTableDir& source);
	const EditTableDir& operator=(const EditTableDir& source);
};

#endif
