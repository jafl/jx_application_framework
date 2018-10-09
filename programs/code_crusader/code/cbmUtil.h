/******************************************************************************
 cbmUtil.h

	Copyright (C) 1999 by John Lindal.

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
							  JBoolean* setTabWidth, JSize* tabWidth,
							  JBoolean* setTabMode, JBoolean* tabInsertsSpaces,
							  JBoolean* setAutoIndent, JBoolean* autoIndent);
void		CBMScrollForDefinition(JXTEBase* te, const CBLanguage lang);
void		CBMSelectLines(JTextEditor* te, const JIndexRange& origRange);
void		CBMBalanceFromSelection(JXTEBase* te, const CBLanguage lang);
JBoolean	CBMIsCharacterInWord(const JString& text, const JIndex charIndex);

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

	void	CBMWriteSharedPrefs(const JBoolean replace);

#elif defined CODE_MEDIC

	#include "CMPrefsManager.h"

	class JTEStyler;

	class CMSourceWindowManager
	{
	public:

		void	StylerChanged(JTEStyler* styler) {};
	};

	CMSourceWindowManager*	CBMGetDocumentManager();
	CMPrefsManager*			CBMGetPrefsManager();

	typedef CMPrefsManager	CBMPrefsManager;

	JBoolean	CBMReadSharedPrefs(JString* fontName, JSize* fontSize,
								   JSize* tabCharCount,
								   JBoolean* sortFnNames, JBoolean* includeNS,
								   JBoolean* packFnNames,
								   JBoolean* openComplFileOnTop,
								   const JSize colorCount, JRGB colorList[],
								   JPtrArray<JString>* cSourceSuffixList,
								   JPtrArray<JString>* cHeaderSuffixList,
								   JPtrArray<JString>* fortranSuffixList,
								   JPtrArray<JString>* javaSuffixList,
								   JPtrArray<JString>* phpSuffixList);

#endif

/******************************************************************************
 Functions for balancing

 ******************************************************************************/

JBoolean	CBMBalanceForward(const CBLanguage lang, const JString& str, JIndex* index);
JBoolean	CBMBalanceBackward(const CBLanguage lang, const JString& str, JIndex* index);

inline JBoolean
CBMIsOpenGroup
	(
	const CBLanguage	lang,
	const JCharacter	c
	)
{
	return JI2B( c == '(' || c == '{' || c == '[' ||
				 ((lang == kCBHTMLLang || lang == kCBXMLLang) && c == '<'));
}

inline JBoolean
CBMIsCloseGroup
	(
	const CBLanguage	lang,
	const JCharacter	c
	)
{
	return JI2B( c == ')' || c == '}' || c == ']' ||
				 ((lang == kCBHTMLLang || lang == kCBXMLLang) && c == '>'));
}

inline JBoolean
CBMIsMatchingPair
	(
	const CBLanguage	lang,
	const JCharacter	openChar,
	const JCharacter	closeChar
	)
{
	return JI2B((openChar == '(' && closeChar == ')') ||
				(openChar == '{' && closeChar == '}') ||
				(openChar == '[' && closeChar == ']') ||
				((lang == kCBHTMLLang || lang == kCBXMLLang) &&
				 openChar == '<' && closeChar == '>'));
}

#endif
