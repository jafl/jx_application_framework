/******************************************************************************
 EditTableDir.h

	Interface for the EditTableDir class

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#ifndef _H_EditTableDir
#define _H_EditTableDir

#include <jx-af/jx/JXWindowDirector.h>


class EditTableDir : public JXWindowDirector
{
public:

	EditTableDir(JXDirector* supervisor);

	~EditTableDir() override;

private:

	JArray<JIndex>* itsData;

private:

	void BuildWindow();
};

#endif
