/******************************************************************************
 LayoutDocument.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_LayoutDocument
#define _H_LayoutDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXWidget.h>	// need sizing

class JXTextMenu;
class JXToolBar;
class LayoutContainer;
class BaseWidget;

class LayoutDocument : public JXFileDocument
{
public:

	static bool	Create(LayoutDocument** doc);
	static bool	Create(const JString& fullName);

	~LayoutDocument() override;

	const JString&	GetName() const override;

	void	SelectAllWidgets();
	void	ClearSelection();
	void	GetSelectedWidgets(JPtrArray<BaseWidget>* list) const;

protected:

	LayoutDocument(const JString& fullName, const bool onDisk);

	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

private:

	LayoutContainer*	itsLayoutContainer;
	mutable JString		itsDocName;		// so GetName() can return JString&

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsEditMenu;
	JXTextMenu*	itsPrefsMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();
	void	ReadFile(std::istream& input);
	void	SetLayoutSize(const JCoordinate w, const JCoordinate h);

	static void			ImportFDesignFile(std::istream& input);
	bool				ImportFDesignLayout(std::istream& input);
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

#endif
