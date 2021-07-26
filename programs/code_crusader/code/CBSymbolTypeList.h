/******************************************************************************
 CBSymbolTypeList.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolTypeList
#define _H_CBSymbolTypeList

#include "CBSymbolList.h"
#include <JPrefObject.h>

class JXDisplay;
class JXImage;

class CBSymbolTypeList : public JContainer, public JPrefObject
{
public:

	CBSymbolTypeList(JXDisplay* display);

	virtual ~CBSymbolTypeList();

//	bool		IsVisible(const CBSymbolList::Type type) const;
	CBLanguage	GetLanguage(const CBSymbolList::Type type) const;
	JFontStyle	GetStyle(const CBSymbolList::Type type,
						 const JXImage** image) const;

	static void	SkipSetup(std::istream& input, const JFileVersion vers);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	struct SymbolTypeInfo
	{
		CBSymbolList::Type	type;
		CBLanguage			lang;
		bool			visible;	// not used
		JFontStyle			style;
		JXImage*			icon;		// can be nullptr; not owned

		SymbolTypeInfo()
			:
			type(CBSymbolList::kUnknownST), lang(kCBOtherLang),
			visible(true), icon(nullptr)
		{ };

		SymbolTypeInfo(const CBSymbolList::Type t, const CBLanguage l,
					   const JFontStyle& s, JXImage* i)
			:
			type(t), lang(l), visible(true), style(s), icon(i)
		{ };
	};

private:

	JArray<SymbolTypeInfo>*	itsSymbolTypeList;

	JXImage*	itsCClassIcon;
	JXImage*	itsCStructIcon;
	JXImage*	itsCEnumIcon;
	JXImage*	itsCUnionIcon;
	JXImage*	itsCMacroIcon;
	JXImage*	itsCTypedefIcon;
	JXImage*	itsCNamespaceIcon;
	JXImage*	itsCSharpClassIcon;
	JXImage*	itsCSharpEventIcon;
	JXImage*	itsCSharpInterfaceIcon;
	JXImage*	itsCSharpNamespaceIcon;
	JXImage*	itsCSharpStructIcon;
	JXImage*	itsEiffelClassIcon;
	JXImage*	itsFortranCommonBlockIcon;
	JXImage*	itsJavaClassIcon;
	JXImage*	itsJavaInterfaceIcon;
	JXImage*	itsJavaPackageIcon;
	JXImage*	itsJavaScriptClassIcon;
	JXImage*	itsAssemblyLabelIcon;
	JXImage*	itsPythonClassIcon;
	JXImage*	itsPHPClassIcon;
	JXImage*	itsPHPInterfaceIcon;
	JXImage*	itsBisonNontermDefIcon;
	JXImage*	itsBisonNontermDeclIcon;
	JXImage*	itsBisonTermDeclIcon;
	JXImage*	itsPrototypeIcon;
	JXImage*	itsFunctionIcon;
	JXImage*	itsVariableIcon;
	JXImage*	itsMemberIcon;

private:

	void	CreateSymTypeList(JXDisplay* display);
	void	LoadIcons(JXDisplay* display);

	JIndex	FindType(const CBSymbolList::Type type) const;

	// not allowed

	CBSymbolTypeList(const CBSymbolTypeList& source);
	const CBSymbolTypeList& operator=(const CBSymbolTypeList& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kVisibilityChanged;
	static const JUtf8Byte* kStyleChanged;

	class VisibilityChanged : public JBroadcaster::Message
		{
		public:

			VisibilityChanged()
				:
				JBroadcaster::Message(kVisibilityChanged)
				{ };
		};

	class StyleChanged : public JBroadcaster::Message
		{
		public:

			StyleChanged()
				:
				JBroadcaster::Message(kStyleChanged)
				{ };
		};
};


/******************************************************************************
 IsVisible

 ******************************************************************************/
/*
inline bool
CBSymbolTypeList::IsVisible
	(
	const CBSymbolList::Type type
	)
	const
{
	const JIndex i = FindType(type);
	return (itsSymbolTypeList->GetElement(i)).visible;
}
*/
/******************************************************************************
 GetLanguage

 ******************************************************************************/

inline CBLanguage
CBSymbolTypeList::GetLanguage
	(
	const CBSymbolList::Type type
	)
	const
{
	const JIndex i = FindType(type);
	return (itsSymbolTypeList->GetElement(i)).lang;
}

/******************************************************************************
 GetStyle

	image can return nullptr.

 ******************************************************************************/

inline JFontStyle
CBSymbolTypeList::GetStyle
	(
	const CBSymbolList::Type	type,
	const JXImage**				icon
	)
	const
{
	const JIndex i            = FindType(type);
	const SymbolTypeInfo info = itsSymbolTypeList->GetElement(i);

	*icon = info.icon;
	return info.style;
}

#endif
