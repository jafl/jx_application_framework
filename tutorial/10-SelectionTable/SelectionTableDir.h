/******************************************************************************
 SelectionTableDir.h

	Interface for the SelectionTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SelectionTableDir
#define _H_SelectionTableDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JArray.h>


class SelectionTableDir : public JXWindowDirector
{
public:

	SelectionTableDir(JXDirector* supervisor);

	virtual ~SelectionTableDir();

private:

	JArray<JIndex>* itsData;

private:

	void BuildWindow();

	// not allowed

	SelectionTableDir(const SelectionTableDir& source);
	const SelectionTableDir& operator=(const SelectionTableDir& source);
};

#endif
