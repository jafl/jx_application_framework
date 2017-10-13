/******************************************************************************
 GXChooseFileImportDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GXChooseFileImportDialog.h"
#include "GLPlotApp.h"
#include "GXDataDocument.h"
#include "GLPrefsMgr.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JXColormap.h>

#include <JPtrArray-JString.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

const JSize  kFileByteCount				= 1000;
const JIndex kFileModulePrefsVersionID 	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GXChooseFileImportDialog::GXChooseFileImportDialog
	(
	GXDataDocument*	supervisor,
	const JString&	filename
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	JPrefObject(GetPrefsMgr(), kFileModulePrefsID),
	itsDir(supervisor)
{
	BuildWindow(filename);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXChooseFileImportDialog::~GXChooseFileImportDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXChooseFileImportDialog::BuildWindow
	(
	const JString& filename
	)
{
	
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 330,230, "");
	assert( window != NULL );

	JXStaticText* errorMessage =
		jnew JXStaticText(JGetString("errorMessage::GXChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 250,40);
	assert( errorMessage != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GXChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,200, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GXChooseFileImportDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GXChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,200, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GXChooseFileImportDialog::shortcuts::JXLayout"));

	itsReloadButton =
		jnew JXTextButton(JGetString("itsReloadButton::GXChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,200, 70,20);
	assert( itsReloadButton != NULL );

	itsFilterMenu =
		jnew JXTextMenu(JGetString("itsFilterMenu::GXChooseFileImportDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,160, 70,30);
	assert( itsFilterMenu != NULL );

	JXScrollbarSet* textScrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 310,100);
	assert( textScrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Choose Import Filter");
	window->PlaceAsDialogWindow();
	UseModalPlacement(kJTrue);
	SetButtons(okButton, cancelButton);

	const JSize dirModCount = itsDir->GetInternalModuleCount();
	for (JSize i = 1; i <= dirModCount; i++)
		{
		itsFilterMenu->AppendItem(itsDir->GetInternalModuleName(i));
		}

	JPtrArray<JString>* names = (GLGetApplication())->GetImportModules();
	
	const JSize strCount = names->GetElementCount();
	
	for (JSize i = 1; i <= strCount; i++)
		{
		itsFilterMenu->AppendItem(*(names->NthElement(i)));
		}

	itsFilterIndex = 1;
	
	itsFilterMenu->SetToPopupChoice(kJTrue, itsFilterIndex);
	itsFilterMenu->SetUpdateAction(JXMenu::kDisableNone);	
	ListenTo(itsFilterMenu);
	ListenTo(itsReloadButton);

	std::ifstream is(filename);
	JString text;
	text.Read(is, kFileByteCount);

	itsFileText = 
		jnew JXStaticText(text, kJFalse, kJFalse, 
			textScrollbarSet, textScrollbarSet->GetScrollEnclosure(), 
			JXWidget::kHElastic, JXWidget::kVElastic, 10,60, 310,90);
	assert(itsFileText != NULL);
	itsFileText->FitToEnclosure();
}

/******************************************************************************
 Receive 

 ******************************************************************************/

void
GXChooseFileImportDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsFilterMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
			itsFilterMenu->AppendItem(*(names->NthElement(i)));
			}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(kJTrue, itsFilterIndex);
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
GXChooseFileImportDialog::GetFilterIndex()
{
	return itsFilterIndex;
}

/******************************************************************************
 GetFileText (public)

 ******************************************************************************/

const JString&
GXChooseFileImportDialog::GetFileText()
{
	return itsFileText->GetText();
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
GXChooseFileImportDialog::ReadPrefs
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
			itsFilterMenu->SetToPopupChoice(kJTrue, itsFilterIndex);
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
GXChooseFileImportDialog::WritePrefs
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
