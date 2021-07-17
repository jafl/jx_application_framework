/******************************************************************************
 cbmUtil.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbmUtil
#define _H_cbmUtil

#include "CBTextFileType.h"
#include <JPtrArray-JString.h>

class JString;
class JTextEditor;
class JXTEBase;
class JXInputField;
class JXKeyModifiers;
class CBFnMenuUpdater;

CBFnMenuUpdater*	CBMGetFnMenuUpdater();

void	CBMParseEditorOptions(const JString& fullName, const JString& text,
							  bool* setTabWidth, JSize* tabWidth,
							  bool* setTabMode, bool* tabInsertsSpaces,
							  bool* setAutoIndent, bool* autoIndent);
void		CBMScrollForDefinition(JXTEBase* te, const CBLanguage lang);
void		CBMSelectLines(JTextEditor* te, const JIndexRange& lineRange);
void		CBMBalanceFromSelection(JXTEBase* te, const CBLanguage lang);
bool	CBMIsCharacterInWord(const JUtf8Character& c);

	// for use by dialogs

void	CBMGetStringList(JXInputField* inputField, JPtrArray<JString>* list);
void	CBMGetSuffixList(JXInputField* inputField, JPtrArray<JString>* list);
void	CBMSetStringList(JXInputField* inputField, const JPtrArray<JString>& list);

#if defined CODE_CRUSADER

	#include "CBPrefsManager.h"
	#include "CBDocumentManager.h"

	CBDocumentManager*	CBMGetDocumentManager();
	CBPrefsManager*		CBMGetPrefsManager();

	typedef CBPrefsManager	CBMPrefsManager;

	void	CBMWriteSharedPrefs(const bool replace);

#elif defined CODE_MEDIC

	#include "CMPrefsManager.h"

	class JSTStyler;

	class CMSourceWindowManager
	{
	public:

		void	StylerChanged(JSTStyler* styler) {};
	};

	CMSourceWindowManager*	CBMGetDocumentManager();
	CMPrefsManager*			CBMGetPrefsManager();

	typedef CMPrefsManager	CBMPrefsManager;

	bool	CBMReadSharedPrefs(JString* fontName, JSize* fontSize,
								   JSize* tabCharCount,
								   bool* sortFnNames, bool* includeNS,
								   bool* packFnNames,
								   bool* openComplFileOnTop,
								   const JSize colorCount, JRGB colorList[],
								   JPtrArray<JString>* cSourceSuffixList,
								   JPtrArray<JString>* cHeaderSuffixList,
								   JPtrArray<JString>* fortranSuffixList,
								   JPtrArray<JString>* javaSuffixList,
								   JPtrArray<JString>* phpSuffixList,
								   JPtrArray<JString>* dSuffixList,
								   JPtrArray<JString>* goSuffixList);

#endif

/******************************************************************************
 Functions for balancing

 ******************************************************************************/

bool	CBMBalanceForward(const CBLanguage lang, JStringIterator* iter, JUtf8Character* c);
bool	CBMBalanceBackward(const CBLanguage lang, JStringIterator* iter, JUtf8Character* c);

inline bool
CBMIsOpenGroup
	(
	const CBLanguage		lang,
	const JUtf8Character&	c
	)
{
	return c == '(' || c == '{' || c == '[';
}

inline bool
CBMIsCloseGroup
	(
	const CBLanguage		lang,
	const JUtf8Character&	c
	)
{
	return c == ')' || c == '}' || c == ']';
}

inline bool
CBMIsMatchingPair
	(
	const CBLanguage		lang,
	const JUtf8Character&	openChar,
	const JUtf8Character&	closeChar
	)
{
	return (openChar == '(' && closeChar == ')') ||
				(openChar == '{' && closeChar == '}') ||
				(openChar == '[' && closeChar == ']') ||
				((lang == kCBHTMLLang || lang == kCBXMLLang) &&
				 openChar == '<' && closeChar == '>');
}

#endif
