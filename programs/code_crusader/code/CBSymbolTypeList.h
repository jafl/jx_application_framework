/******************************************************************************
 CBSymbolTypeList.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSymbolTypeList
#define _H_CBSymbolTypeList

#include "CBSymbolList.h"
#include <JPrefObject.h>

class JXImage;
class JXColormap;

class CBSymbolTypeList : public JContainer, public JPrefObject
{
public:

	CBSymbolTypeList(JXColormap* colormap);

	virtual ~CBSymbolTypeList();

//	JBoolean	IsVisible(const CBSymbolList::Type type) const;
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
		JBoolean			visible;	// not used
		JFontStyle			style;
		JXImage*			icon;		// can be NULL; not owned

		SymbolTypeInfo()
			:
			type(CBSymbolList::kUnknownST), lang(kCBOtherLang),
			visible(kJTrue), icon(NULL)
		{ };

		SymbolTypeInfo(const CBSymbolList::Type t, const CBLanguage l,
					   const JFontStyle& s, JXImage* i)
			:
			type(t), lang(l), visible(kJTrue), style(s), icon(i)
		{ };
	};

private:

	JArray<SymbolTypeInfo>*	itsSymbolTypeList;

	JXColormap*	itsColormap;		// not owned
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

	void	CreateSymTypeList();
	void	CreateIcons();
	void	DeleteIcons();

	JIndex	FindType(const CBSymbolList::Type type) const;

	// not allowed

	CBSymbolTypeList(const CBSymbolTypeList& source);
	const CBSymbolTypeList& operator=(const CBSymbolTypeList& source);

public:

	// JBroadcaster messages

	static const JCharacter* kVisibilityChanged;
	static const JCharacter* kStyleChanged;

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
inline JBoolean
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

	image can return NULL.

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
