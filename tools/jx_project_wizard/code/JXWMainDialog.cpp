/******************************************************************************
 JXWMainDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXWMainDialog.h"
#include "jxwGlobals.h"
#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXPathInput.h>
#include <JXTextCheckbox.h>
#include <JXPathHistoryMenu.h>
#include <JXHelpManager.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JDirInfo.h>
#include <jProcessUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

static const JString kDefaultVersion("1.0.0", JString::kNoCopy);
static const JString kDefaultURLText("http://", JString::kNoCopy);
static const JString kDefaultProjDir(JX_PATH "programs/my_project/", JString::kNoCopy);
static const JString kDefaultTemplateDir(JX_PATH "programs/jx_project_wizard/app_template/", JString::kNoCopy);
static const JString kDefaultOpenCmd("jcc $f", JString::kNoCopy);

static const JString kFilePrefixTag("_pre_", JString::kNoCopy);
static const JString kBinaryPrefixTag("_Binary_", JString::kNoCopy);

const JSize kFilePrefixTagLength   = 5;
const JSize kBinaryPrefixTagLength = 8;

static const JUtf8Byte* kTag[] =
{
	"<pre>",
	"<Program>",
	"<Version>",
	"<Author>",
	"<Company>",
	"<Year>",
	"<EMail>",
	"<URL>",
	"<MDI_DEF>",
	"<Binary>",
	"<PROJ_DIR>",
	"<JX_PATH>"
};

const JSize kPrefixTagLength = 5;

enum
{
	kPrefix,
	kProgram,
	kVersion,
	kAuthor,
	kCompany,
	kYear,
	kEmail,
	kURL,
	kMDI,
	kBinary,
	kProjDir,
	kJXPath,

	kTagCount
};

const JFileVersion kCurrentPrefsVersion	= 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWMainDialog::JXWMainDialog
	(
	JXDirector*		supervisor,
	const JSize		argc,
	char**			argv
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(JXWGetPrefsManager(), kJXWMainDlogID)
{
	BuildWindow(argc, argv);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWMainDialog::~JXWMainDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXWMainDialog::BuildWindow
	(
	const JSize		argc,
	char**			argv
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,370, JString::empty);
	assert( window != nullptr );

	itsCreateButton =
		jnew JXTextButton(JGetString("itsCreateButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,340, 60,20);
	assert( itsCreateButton != nullptr );
	itsCreateButton->SetShortcuts(JGetString("itsCreateButton::JXWMainDialog::shortcuts::JXLayout"));

	itsQuitButton =
		jnew JXTextButton(JGetString("itsQuitButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,340, 60,20);
	assert( itsQuitButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 225,340, 60,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXWMainDialog::shortcuts::JXLayout"));

	JXStaticText* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 120,20);
	assert( nameLabel != nullptr );
	nameLabel->SetToLabel();

	JXStaticText* versionLabel =
		jnew JXStaticText(JGetString("versionLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 120,20);
	assert( versionLabel != nullptr );
	versionLabel->SetToLabel();

	itsProgramName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 350,20);
	assert( itsProgramName != nullptr );

	itsProgramVersion =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,40, 350,20);
	assert( itsProgramVersion != nullptr );

	JXStaticText* authorLabel =
		jnew JXStaticText(JGetString("authorLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 120,20);
	assert( authorLabel != nullptr );
	authorLabel->SetToLabel();

	itsDevName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,70, 350,20);
	assert( itsDevName != nullptr );

	JXStaticText* emailLabel =
		jnew JXStaticText(JGetString("emailLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,90, 120,20);
	assert( emailLabel != nullptr );
	emailLabel->SetToLabel();

	itsDevEmail =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,90, 350,20);
	assert( itsDevEmail != nullptr );

	JXStaticText* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 120,20);
	assert( urlLabel != nullptr );
	urlLabel->SetToLabel();

	JXStaticText* srcPrefixLabel =
		jnew JXStaticText(JGetString("srcPrefixLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,180, 120,20);
	assert( srcPrefixLabel != nullptr );
	srcPrefixLabel->SetToLabel();

	itsNeedsMDICB =
		jnew JXTextCheckbox(JGetString("itsNeedsMDICB::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,210, 130,20);
	assert( itsNeedsMDICB != nullptr );
	itsNeedsMDICB->SetShortcuts(JGetString("itsNeedsMDICB::JXWMainDialog::shortcuts::JXLayout"));

	JXStaticText* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,240, 120,20);
	assert( directoryLabel != nullptr );
	directoryLabel->SetToLabel();

	itsChooseProjDirButton =
		jnew JXTextButton(JGetString("itsChooseProjDirButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,240, 60,20);
	assert( itsChooseProjDirButton != nullptr );

	JXStaticText* templateLabel =
		jnew JXStaticText(JGetString("templateLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,270, 120,20);
	assert( templateLabel != nullptr );
	templateLabel->SetToLabel();

	itsChooseTmplDirButton =
		jnew JXTextButton(JGetString("itsChooseTmplDirButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,270, 60,20);
	assert( itsChooseTmplDirButton != nullptr );

	JXStaticText* programNameLabel =
		jnew JXStaticText(JGetString("programNameLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,160, 120,20);
	assert( programNameLabel != nullptr );
	programNameLabel->SetToLabel();

	JXStaticText* companyLabel =
		jnew JXStaticText(JGetString("companyLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 120,20);
	assert( companyLabel != nullptr );
	companyLabel->SetToLabel();

	itsCompanyName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,110, 350,20);
	assert( itsCompanyName != nullptr );

	itsDevURL =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,130, 350,20);
	assert( itsDevURL != nullptr );

	itsBinaryName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,160, 350,20);
	assert( itsBinaryName != nullptr );

	itsSrcPrefix =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,180, 350,20);
	assert( itsSrcPrefix != nullptr );

	itsProjectDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,240, 290,20);
	assert( itsProjectDir != nullptr );

	itsTemplateDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,270, 260,20);
	assert( itsTemplateDir != nullptr );

	itsTmplDirHistory =
		jnew JXPathHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 450,270, 30,20);
	assert( itsTmplDirHistory != nullptr );

	JXStaticText* openCmdLabel =
		jnew JXStaticText(JGetString("openCmdLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,300, 120,20);
	assert( openCmdLabel != nullptr );
	openCmdLabel->SetToLabel();

	itsOpenCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,300, 350,20);
	assert( itsOpenCmd != nullptr );

// end JXLayout

	const JUtf8Byte* map[] =
		{
		"vers", JXWGetVersionNumberStr()
		};
	const JString title = JGetString("WindowTitle::JXWMainDialog", map, sizeof(map));
	window->SetTitle(title);
	window->ShouldFocusWhenShow(true);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsCreateButton);
	ListenTo(itsQuitButton);
	ListenTo(itsHelpButton);
	ListenTo(itsChooseProjDirButton);
	ListenTo(itsChooseTmplDirButton);
	ListenTo(itsTmplDirHistory);

	itsProgramVersion->GetText()->SetText(kDefaultVersion);
	itsDevURL->GetText()->SetText(kDefaultURLText);
	itsNeedsMDICB->SetState(true);

	itsProjectDir->ShouldAllowInvalidPath();
	itsProjectDir->ShouldRequireWritable();
	itsProjectDir->GetText()->SetText(kDefaultProjDir);

	itsTemplateDir->ShouldAllowInvalidPath();
	itsTemplateDir->GetText()->SetText(kDefaultTemplateDir);

	itsOpenCmd->GetText()->SetText(kDefaultOpenCmd);

	JPrefObject::ReadPrefs();

	// after read prefs to overwrite itsProjectDir

	if (argc == 3)
		{
		itsProjectDir->GetText()->SetText(JString(argv[1], JString::kNoCopy));
		itsProgramName->GetText()->SetText(JString(argv[2], JString::kNoCopy));
		itsBinaryName->GetText()->SetText(JString(argv[2], JString::kNoCopy));
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXWMainDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCreateButton && message.Is(JXButton::kPushed))
		{
		if (WriteTemplate())
			{
			JPrefObject::WritePrefs();
			JXGetApplication()->Quit();
			}
		}
	else if (sender == itsQuitButton && message.Is(JXButton::kPushed))
		{
		JXGetApplication()->Quit();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("JXWMainHelp");
		}

	else if (sender == itsChooseProjDirButton && message.Is(JXButton::kPushed))
		{
		itsProjectDir->ChoosePath(JString::empty);
		}
	else if (sender == itsChooseTmplDirButton && message.Is(JXButton::kPushed))
		{
		itsTemplateDir->ChoosePath(JString::empty);
		}

	else if (sender == itsTmplDirHistory && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsTemplateDir->GetText()->SetText(
			itsTmplDirHistory->JXTextMenu::GetItemText(selection->GetIndex()));
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 WriteTemplate (private)

 ******************************************************************************/

bool
JXWMainDialog::WriteTemplate()
{
	// validate inputs -- itsOpenCmd can be empty

	JXInputField* input[] =
		{
		itsProgramName, itsProgramVersion,
		itsDevName, itsDevEmail, itsDevURL,
		itsBinaryName, itsSrcPrefix
		};
	const JSize inputCount = sizeof(input)/sizeof(JXInputField*);

	for (JUnsignedOffset i=0; i<inputCount; i++)
		{
		input[i]->SetIsRequired(true);
		const bool ok = input[i]->InputValid();
		input[i]->SetIsRequired(false);
		if (!ok)
			{
			input[i]->Focus();
			return false;
			}
		}

	// can't have whitespace in source prefix

	if ((itsSrcPrefix->GetText()->GetText()).Contains(" "))
		{
		JGetUserNotification()->ReportError(JGetString("SrcPrefixError::JXWMainDialog"));
		itsSrcPrefix->Focus();
		return false;
		}

	// check template directory

	itsTemplateDir->ShouldAllowInvalidPath(false);
	bool tmplDirOK = itsTemplateDir->InputValid();
	itsTemplateDir->ShouldAllowInvalidPath(true);
	if (!tmplDirOK)
		{
		itsTemplateDir->Focus();
		return false;
		}

	JString templateDir;
	tmplDirOK = itsTemplateDir->GetPath(&templateDir);
	assert( tmplDirOK );

	const JString testFile = JCombinePathAndName(templateDir, JString("_Binary_.fd", JString::kNoCopy));
	if (!JFileExists(testFile))
		{
		JGetUserNotification()->ReportError(JGetString("InvalidTmplDirError::JXWMainDialog"));
		itsTemplateDir->Focus();
		return false;
		}

	// try to create project directory if it doesn't exist

	JString projectDir;
	if (JIsRelativePath(itsProjectDir->GetText()->GetText()) ||
		!JExpandHomeDirShortcut(itsProjectDir->GetText()->GetText(), &projectDir))
		{
		itsProjectDir->ShouldAllowInvalidPath(false);
		itsProjectDir->InputValid();
		itsProjectDir->ShouldAllowInvalidPath(true);
		itsProjectDir->Focus();
		return false;
		}

	const JError err = JCreateDirectory(projectDir);
	if (!err.OK())
		{
		JGetStringManager()->ReportError("UnableToCreateProjDir::JXWMainDialog", err);
		return false;
		}

	// warnings should be after all errors so user doesn't have to answer yes
	// after fixing each error

		// nothing to warn about at the moment

	// generate code

	tm ts;
	const time_t t = time(nullptr);
	gmtime_r(&t, &ts);
	const JString currentYear((JUInt64) 1900 + ts.tm_year);

	JString f = projectDir, p, dirName;
	JStripTrailingDirSeparator(&f);
	JSplitPathAndName(f, &p, &dirName);

	JString jxPath(JX_PATH, JString::kNoCopy);
	if (projectDir.BeginsWith(jxPath))
		{
		jxPath = JConvertToRelativePath(jxPath, projectDir);
		JStripTrailingDirSeparator(&jxPath);
		}

	JPtrArray<JString> value(JPtrArrayT::kDeleteAll);
	value.Append(itsSrcPrefix->GetText()->GetText());
	value.Append(itsProgramName->GetText()->GetText());
	value.Append(itsProgramVersion->GetText()->GetText());
	value.Append(itsDevName->GetText()->GetText());
	value.Append(itsCompanyName->GetText()->GetText());
	value.Append(currentYear);
	value.Append(itsDevEmail->GetText()->GetText());
	value.Append(itsDevURL->GetText()->GetText());
	value.Append(itsNeedsMDICB->IsChecked() ? JString("-DUSE_MDI", JString::kNoCopy) : JString::empty);
	value.Append(itsBinaryName->GetText()->GetText());
	value.Append(dirName);
	value.Append(jxPath);

	CopyAndAdjustTemplateFiles(templateDir, projectDir, value);

	// open the project

	if (!itsOpenCmd->GetText()->IsEmpty())
		{
		JString projFile = itsBinaryName->GetText()->GetText() + ".jcc";
		projFile         = JCombinePathAndName(projectDir, projFile);
		projFile         = JPrepArgForExec(projFile);

		const JUtf8Byte* map[] =
			{
			"f", projFile.GetBytes()
			};
		JString cmd = itsOpenCmd->GetText()->GetText();
		JGetStringManager()->Replace(&cmd, map, sizeof(map));

		pid_t pid;
		JExecute(cmd, &pid);
		}

	// save template directory

	itsTmplDirHistory->AddString(templateDir);
	return true;
}

/******************************************************************************
 CopyAndAdjustTemplateFiles (private, recursive)

 ******************************************************************************/

void
JXWMainDialog::CopyAndAdjustTemplateFiles
	(
	const JString&				sourceDir,
	const JString&				targetDir,
	const JPtrArray<JString>&	value
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(sourceDir, &info))
		{
		return;
		}
	info->SetWildcardFilter(JString("*~ #*#", JString::kNoCopy), true);

	const JSize prefixLength = value.GetElement(kPrefix)->GetCharacterCount();

	const JSize count = info->GetEntryCount();
	JString prefix;
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);
		if (entry.IsDirectory() && !JIsVCSDirectory(entry.GetName()))
			{
			const JString& name = entry.GetName();
			const JString d1    = JCombinePathAndName(sourceDir, name);
			const JString d2    = JCombinePathAndName(targetDir, name);
			const JError err    = JCreateDirectory(d2);
			if (err.OK())
				{
				CopyAndAdjustTemplateFiles(d1, d2, value);
				}
			else
				{
				err.ReportIfError();
				}
			}
		else if (entry.IsFile())
			{
			JString name = entry.GetName();
			if (name.BeginsWith(kFilePrefixTag, JString::kIgnoreCase))
				{
				JStringIterator iter(&name);
				iter.SkipNext();
				iter.BeginMatch();
				iter.SkipNext(kFilePrefixTagLength-2);
				prefix = iter.FinishMatch().GetString();

				iter.RemoveLastMatch();
				iter.RemoveNext();
				iter.Invalidate();

				name.Prepend(*value.GetElement(kPrefix));
				name.MatchCase(prefix, JCharacterRange(1, prefix.GetCharacterCount()),
							   JCharacterRange(1, prefixLength));
				}
			else if (name.BeginsWith(kBinaryPrefixTag))
				{
				JStringIterator iter(&name);
				iter.BeginMatch();
				iter.SkipNext(kBinaryPrefixTagLength);
				iter.FinishMatch();
				iter.ReplaceLastMatch(*value.GetElement(kBinary));
				}

			JString data;
			JReadFile(entry.GetFullName(), &data);

			for (JUnsignedOffset j=0; j<kTagCount; j++)
				{
				const bool isPrefix = j == kPrefix;

				JStringIterator iter(&data);
				JIndex fIndex = 1;
				while (iter.Next(kTag[j], !isPrefix))
					{
					if (isPrefix)
						{
						prefix = data.GetSubstring(fIndex+1, fIndex+kPrefixTagLength-2);
						}
					data.ReplaceSubstring(fIndex, fIndex + strlen(kTag[j])-1,
										  value[j]);
					if (isPrefix)
						{
						data.MatchCase(prefix, JCharacterRange(1, prefix.GetLength()),
									   JCharacterRange(fIndex, fIndex+prefixLength-1));
						}
					}
				}

			const JString target = JCombinePathAndName(targetDir, name);
			std::ofstream os(target.GetBytes());
			data.Print(os);
			}
		}

	jdelete info;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
JXWMainDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentPrefsVersion)
		{
		GetWindow()->ReadGeometry(input);

		JString s;
		input >> s;
		itsProgramVersion->GetText()->SetText(s);

		input >> s;
		itsDevName->GetText()->SetText(s);

		input >> s;
		itsDevEmail->GetText()->SetText(s);

		input >> s;
		itsCompanyName->GetText()->SetText(s);

		input >> s;
		itsDevURL->GetText()->SetText(s);

		bool checked;
		input >> JBoolFromString(checked);
		itsNeedsMDICB->SetState(checked);

		input >> s;
		if (!s.IsEmpty())
			{
			UpdatePath(&s);
			itsProjectDir->GetText()->SetText(s);
			}

		input >> s;
		if (!s.IsEmpty())
			{
			UpdatePath(&s);
			itsTemplateDir->GetText()->SetText(s);
			}

		itsTmplDirHistory->ReadSetup(input);

		input >> s;
		itsOpenCmd->GetText()->SetText(s);
		}
}

/******************************************************************************
 UpdatePath (private)

 ******************************************************************************/

static const JRegex jxVersPattern = "JX-([0-9.]+)";

void
JXWMainDialog::UpdatePath
	(
	JString* s
	)
{
	JStringIterator iter(s);
	const JString v = "JX-" + JString(JXWGetVersionNumberStr(), JString::kNoCopy);
	while (iter.Next(jxVersPattern))
		{
		iter.ReplaceLastMatch(v);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

static const JRegex hostPattern = "[^/]+://[^/]+/";
static const JRegex projPattern = ".+/JX-[0-9.]+/+programs/";

void
JXWMainDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsProgramVersion->GetText()->GetText();
	output << ' ' << itsDevName->GetText()->GetText();
	output << ' ' << itsDevEmail->GetText()->GetText();
	output << ' ' << itsCompanyName->GetText()->GetText();

	// save host name from url

	output << ' ';
	const JStringMatch m1 = hostPattern.Match(itsDevURL->GetText()->GetText(), JRegex::kIgnoreSubmatches);
	if (!m1.IsEmpty())
		{
		output << m1.GetString();
		}
	else
		{
		output << JString(kDefaultURLText, JString::kNoCopy);
		}

	output << ' ' << JBoolToString(itsNeedsMDICB->IsChecked());

	// save ".../JX-<vers>/programs/" from project directory

	output << ' ';
	const JStringMatch m2 = projPattern.Match(itsProjectDir->GetText()->GetText(), JRegex::kIgnoreSubmatches);
	if (!m2.IsEmpty())
		{
		output << m2.GetString();
		}
	else
		{
		output << JString();
		}

	output << ' ' << itsTemplateDir->GetText()->GetText();

	output << ' ';
	itsTmplDirHistory->WriteSetup(output);

	output << ' ' << itsOpenCmd->GetText()->GetText();
	output << ' ';
}
