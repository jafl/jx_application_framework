/******************************************************************************
 DataTableDir.h

	Interface for the DataTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_DataTableDir
#define _H_DataTableDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JArray.h>


class DataTableDir : public JXWindowDirector
{
public:

	DataTableDir(JXDirector* supervisor);

	virtual ~DataTableDir();

private:

	JArray<JIndex>* itsData;

private:

	void BuildWindow();

	// not allowed

	DataTableDir(const DataTableDir& source);
	const DataTableDir& operator=(const DataTableDir& source);
};

#endif
