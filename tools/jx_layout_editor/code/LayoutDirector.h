/******************************************************************************
 LayoutDirector.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_LayoutDirector
#define _H_LayoutDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXWidget.h>	// need sizing
#include <jx-af/jcore/JString.h>

class JRect;
class JXTextMenu;
class JXToolBar;
class JXFlatRect;
class MainDocument;
class LayoutContainer;
class BaseWidget;

class LayoutDirector : public JXWindowDirector
{
public:

	LayoutDirector(MainDocument* doc, const JString& name);
	LayoutDirector(MainDocument* doc, const JString& name, std::istream& input);

	~LayoutDirector() override;

	bool	Close() override;

	const JString&	GetLayoutName() const;
	void			SetLayoutName(const JString& name);

	void	WriteLayout(std::ostream& output) const;
	bool	ImportFDesignLayout(std::istream& input);

	void	SelectAllWidgets();
	void	ClearSelection();
	void	GetSelectedWidgets(JPtrArray<BaseWidget>* list) const;

	void	DataModified();

private:

	MainDocument*		itsDoc;
	JString				itsLayoutName;
	LayoutContainer*	itsLayoutContainer;
	bool				itsModifiedFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsEditMenu;
	JXTextMenu*	itsPrefsMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle();

	void	ReadLayout(std::istream& input);
	void	SetLayoutSize(const JCoordinate w, const JCoordinate h);

	static JString		ReadFDesignString(std::istream& input, const JUtf8Byte* marker);
	static JCoordinate	ReadFDesignNumber(std::istream& input, const JUtf8Byte* marker);
	static void			ParseFDesignGravity(const JString& gravity,
											JXWidget::HSizingOption* hSizing,
											JXWidget::VSizingOption* vSizing);
	static JString		GetTempFDesignVarName(const JPtrArray<JString>& objNames);
	static bool			GetFDesignEnclosure(const JRect& frame,
											const JArray<JRect>& rectList,
											JIndex* enclIndex);
	static void			SplitFDesignClassNameAndArgs(const JString& str,
													 JString* name, JString* args);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
};


/******************************************************************************
 Layout name

 ******************************************************************************/

inline const JString&
LayoutDirector::GetLayoutName()
	const
{
	return itsLayoutName;
}

inline void
LayoutDirector::SetLayoutName
	(
	const JString& name
	)
{
	itsLayoutName = name;
	UpdateWindowTitle();
}

#endif
