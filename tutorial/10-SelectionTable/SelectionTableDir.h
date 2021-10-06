/******************************************************************************
 SelectionTableDir.h

	Interface for the SelectionTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_SelectionTableDir
#define _H_SelectionTableDir

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
};

#endif
