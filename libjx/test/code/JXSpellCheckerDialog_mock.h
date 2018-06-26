/******************************************************************************
 JXSpellCheckerDialog.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSpellCheckerDialog
#define _H_JXSpellCheckerDialog

#include <JStyledText.h>

class JXTEBase;
class JXSpellChecker;

class JXSpellCheckerDialog
{
public:

	JXSpellCheckerDialog(JXSpellChecker* checker,
						 JXTEBase* editor, const JStyledText::TextRange& range);

	virtual ~JXSpellCheckerDialog();

	void	Check();
};

#endif
