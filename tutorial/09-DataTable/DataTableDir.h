/******************************************************************************
 DataTableDir.h

	Interface for the DataTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_DataTableDir
#define _H_DataTableDir

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
