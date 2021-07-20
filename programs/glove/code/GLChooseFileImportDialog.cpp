/******************************************************************************
 GLChooseFileImportDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLChooseFileImportDialog.h"
#include "GLPlotApp.h"
#include "GLDataDocument.h"
#include "GLPrefsMgr.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColorManager.h>

#include <JPtrArray-JString.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

const JSize  kFileByteCount				= 1000;
const JIndex kFileModulePrefsVersionID 	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLChooseFileImportDialog::GLChooseFileImportDialog
	(
	GLDataDocument*	supervisor,
	const JString&	filename
	)
	:
	JXDialogDirector(supervisor, true),
	JPrefObject(GLGetPrefsMgr(), kFileModulePrefsID),
	itsDir(supervisor)
{
	BuildWindow(filename);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLChooseFileImportDialog::~GLChooseFileImportDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLChooseFileImportDialog::BuildWindow
	(
	const JString& filename
	)
{
	
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,230, JString::empty);
	assert( window != nullptr );

	auto* errorMessage =
		jnew JXStaticText(JGetString("errorMessage::GLChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 250,40);
	assert( errorMessage != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,200, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLChooseFileImportDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,200, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::GLChooseFileImportDialog::shortcuts::JXLayout"));

	itsReloadButton =
		jnew JXTextButton(JGetString("itsReloadButton::GLChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,200, 70,20);
	assert( itsReloadButton != nullptr );

	itsFilterMenu =
		jnew JXTextMenu(JGetString("itsFilterMenu::GLChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,160, 70,30);
	assert( itsFilterMenu != nullptr );

	auto* textScrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 310,100);
	assert( textScrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLChooseFileImportDialog"));
	window->PlaceAsDialogWindow();
	UseModalPlacement(true);
	SetButtons(okButton, cancelButton);

	const JSize dirModCount = itsDir->GetInternalModuleCount();
	for (JIndex i=1; i<=dirModCount; i++)
		{
		itsFilterMenu->AppendItem(itsDir->GetInternalModuleName(i));
		}

	JPtrArray<JString>* names = (GLGetApplication())->GetImportModules();
	
	const JSize strCount = names->GetElementCount();
	
	for (JIndex i=1; i<=strCount; i++)
		{
		itsFilterMenu->AppendItem(*(names->GetElement(i)));
		}

	itsFilterIndex = 1;
	
	itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
	itsFilterMenu->SetUpdateAction(JXMenu::kDisableNone);	
	ListenTo(itsFilterMenu);
	ListenTo(itsReloadButton);

	std::ifstream is(filename.GetBytes());
	JString text;
	text.Read(is, kFileByteCount);

	itsFileText = 
		jnew JXStaticText(text, false, false, false,
			textScrollbarSet, textScrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic, 10,60, 310,90);
	assert(itsFileText != nullptr);
	itsFileText->FitToEnclosure();
}

/******************************************************************************
 Receive 

 ******************************************************************************/

void
GLChooseFileImportDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsFilterMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsFilterIndex = selection->GetIndex();
		}
		
	else if (sender == itsReloadButton && message.Is(JXButton::kPushed))
		{
		(GLGetApplication())->ReloadImportModules();
		itsFilterMenu->RemoveAllItems();

		const JSize dirModCount = itsDir->GetInternalModuleCount();
		for (JSize i = 1; i <= dirModCount; i++)
			{
			itsFilterMenu->AppendItem(itsDir->GetInternalModuleName(i));
			}

		JPtrArray<JString>* names = (GLGetApplication())->GetImportModules();
		const JSize strCount = names->GetElementCount();
		for (JSize i = 1; i <= strCount; i++)
			{
			itsFilterMenu->AppendItem(*(names->GetElement(i)));
			}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
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
GLChooseFileImportDialog::GetFilterIndex()
{
	return itsFilterIndex;
}

/******************************************************************************
 GetFileText (public)

 ******************************************************************************/

const JString&
GLChooseFileImportDialog::GetFileText()
{
	return itsFileText->GetText()->GetText();
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
GLChooseFileImportDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JIndex id;
	input >> id;
	if (id <= kFileModulePrefsVersionID)
		{
		input >> id;
		if (id <= itsFilterMenu->GetItemCount())
			{
			itsFilterIndex = id;
			itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
			}
		JPoint loc;
		input >> loc;
		JCoordinate w;
		JCoordinate h;
		input >> w;
		input >> h;
		GetWindow()->SetSize(w, h);
		GetWindow()->Place(loc.x, loc.y);
		}
}

/******************************************************************************
 WritePrefs (protected)

 ******************************************************************************/

void
GLChooseFileImportDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kFileModulePrefsVersionID << ' ';
	output << itsFilterIndex << ' ';
	output << GetWindow()->GetDesktopLocation() << ' ';
	output << GetWindow()->GetFrameWidth() << ' ';
	output << GetWindow()->GetFrameHeight() << ' ';
}
