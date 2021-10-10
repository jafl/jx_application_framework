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

	~DataTableDir() override;

private:

	JArray<JIndex>* itsData;

private:

	void BuildWindow();
};

#endif
