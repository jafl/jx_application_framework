/******************************************************************************
 GLFitModuleDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLFitModuleDialog.h"
#include "GLPlotApp.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JPtrArray.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLFitModuleDialog::GLFitModuleDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLFitModuleDialog::~GLFitModuleDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLFitModuleDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 270,100, JString::empty);
	assert( window != nullptr );

	auto* prompt =
		jnew JXStaticText(JGetString("prompt::GLFitModuleDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,10, 230,20);
	assert( prompt != nullptr );
	prompt->SetToLabel();

	itsFilterMenu =
		jnew JXTextMenu(JGetString("itsFilterMenu::GLFitModuleDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,40, 70,20);
	assert( itsFilterMenu != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLFitModuleDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,70, 70,20);
	assert( cancelButton != nullptr );

	itsOKButton =
		jnew JXTextButton(JGetString("itsOKButton::GLFitModuleDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,70, 70,20);
	assert( itsOKButton != nullptr );
	itsOKButton->SetShortcuts(JGetString("itsOKButton::GLFitModuleDialog::shortcuts::JXLayout"));

	itsReloadButton =
		jnew JXTextButton(JGetString("itsReloadButton::GLFitModuleDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 70,20);
	assert( itsReloadButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLFitModuleDialog"));
	SetButtons(itsOKButton, cancelButton);

	JPtrArray<JString>* names = GLGetApplication()->GetFitModules();
	for (auto* name : *names)
		{
		itsFilterMenu->AppendItem(*name);
		}

	itsFilterIndex = 1;

	itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
	itsFilterMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFilterMenu);
	if (names->IsEmpty())
		{
		itsFilterMenu->Deactivate();
		itsOKButton->Deactivate();
		}
	ListenTo(itsReloadButton);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GLFitModuleDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsFilterMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsFilterIndex = selection->GetIndex();
		}

	else if (sender == itsReloadButton && message.Is(JXButton::kPushed))
		{
		GLGetApplication()->ReloadFitModules();
		itsFilterMenu->RemoveAllItems();
		JPtrArray<JString>* names = GLGetApplication()->GetFitModules();
		for (auto* name : *names)
			{
			itsFilterMenu->AppendItem(*name);
			}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
		if (names->IsEmpty())
			{
			itsFilterMenu->Deactivate();
			itsOKButton->Deactivate();
			}
		else
			{
			itsFilterMenu->Activate();
			itsOKButton->Activate();
			}
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetFilterIndex

 ******************************************************************************/

JIndex
GLFitModuleDialog::GetFilterIndex()
{
	return itsFilterIndex;
}
