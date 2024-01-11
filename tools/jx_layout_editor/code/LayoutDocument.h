/******************************************************************************
 LayoutDocument.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_LayoutDocument
#define _H_LayoutDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXWidget.h>	// need sizing

class JXMenuBar;
class JXTextMenu;
class JXToolBar;
class LayoutContainer;
class BaseWidget;

class LayoutDocument : public JXFileDocument
{
	friend class LayoutUndo;

public:

	static bool	Create(LayoutDocument** doc);
	static bool	Create(const JString& fullName);

	~LayoutDocument() override;

	const JString&	GetName() const override;
	const JString&	GetCodeTag() const;

	LayoutContainer*	GetLayoutContainer() const;

	void	UpdateSaveState();

	BaseWidget*	ReadWidget(std::istream& input, JXWidget* defaultEnclosure,
						   JPtrArray<JXWidget>* widgetList) const;
	static void	WriteWidget(std::ostream& output, const JXContainer* obj,
							JPtrArray<JXWidget>* widgetList);

	static bool	FindProjectRoot(const JString& path, JString* root);

protected:

	LayoutDocument(const JString& fullName, const bool onDisk);

	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

private:

	LayoutContainer*	itsLayout;
	mutable JString		itsDocName;		// so GetName() can return JString&

	JString	itsWindowTitle;
	JString	itsContainerName;
	JString	itsCodeTag;
	bool	itsAdjustContainerToFitFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsLayoutMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsGridMenu;

// begin JXLayout

	JXMenuBar* itsMenuBar;
	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();
	void	ReadFile(std::istream& input, const bool isUndoRedo = false);
	void	SetLayoutSize(const JCoordinate w, const JCoordinate h);
	void	SetDataReverted();

	bool	GenerateCode() const;
	bool	CopyBeforeCodeDelimiter(std::istream& input, std::ostream& output,
									JString* indent) const;
	bool	CopyAfterCodeDelimiter(std::istream& input, std::ostream& output) const;
	void	GenerateHeader(std::ostream& output, const JPtrArray<JString>& objTypes,
							const JPtrArray<JString>& objNames, const JString& indent) const;

	static bool	FindInputFile(JString* path, const JUtf8Byte* suffixList[]);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleLayoutMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);

	void	UpdateGridMenu();
	void	HandleGridMenu(const JIndex index);

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
};


/******************************************************************************
 GetLayoutContainer

 ******************************************************************************/

inline LayoutContainer*
LayoutDocument::GetLayoutContainer()
	const
{
	return itsLayout;
}

#endif
