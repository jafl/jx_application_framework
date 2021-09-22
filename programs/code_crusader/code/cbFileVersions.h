/******************************************************************************
 cbFileVersions.h

	Version information for unstructured data files

	Copyright © 1996-2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbFileVersions
#define _H_cbFileVersions

#include <jTypes.h>

const JFileVersion kCurrentPrefsFileVersion = 68;

// version 68:
//	Added support for D & Go source files.
// version 67:
//	Added support for Java properties files.
// version 66:
//	Added support for INI files.
// version 65:
//	Simplified sort of FileTypeInfo.
//	Added "core.*" file type to execute "medic -c $f".
// version 64:
//	Switches to TrueType monospace font.
// version 63:
//	Upgrade to ctags 5.8.
// version 62:
//	Added external png file type.
// version 61:
//	Update to CBSearchTextDialog prefs (73).
// version 60:
//	CBTextDocument saves kTabInsertsSpacesIndex setting override.
// version 59:
//	Added new file types and symbols supported by ctags 5.7.
// version 58:
//	Added XML to CBTextFileType.
// version 57:
//	Default character action for XML: xml-auto-close
// version 56:
//	Added JSP to CBTextFileType.
// version 55:
//	Removed CBDocumentManager::itsUpdateSymbolDBAfterBuildFlag.
// version 54:
//	Added ctags "PHP interface" type.
// version 53:
//	Re-sorted file type list so longer items are checked first.
// version 52:
// 	Added Ant to CBTextFileType.
// version 51:
//	Added CBProjectDocument::warnOpenOldVersion to kCBStaticGlobalID.
// version 50:
// 	Added JavaScript to CBTextFileType.
// version 49:
//	Added C#, Erlang, SML to CBTextFileType.
//	Added ctags "ASP variable" type.
//	Added ctags "Vim autocommand group" type.
//	Added CBCSharpStyler, CBPythonStyler, CBEiffelStyler.
// version 48:
//	Added ctags "PHP define" type.
// version 47:
//	Added CBAddFilesFilterDialog::itsFilterStr to kCBStaticGlobalID.
// version 46:
//	Added CBBuildManager::ShouldRebuildMakefileDaily() to kCBStaticGlobalID.
// version 45:
//	Cleaned out data for id 14 and 15, since they were unused.
// version 44:
//	Added Beta, Lua, SLang, SQL, Vera, Verilog to CBTextFileType.
// version 43:
//	Removed RaiseBefore* and BeepAfter* from kCBStaticGlobalID.
// version 42:
//	Added DoubleSpaceCompilerOutput to kCBStaticGlobalID.
// version 41:
//	Added Bison to CBTextFileType.
// version 40:
//	Added scriptPath to FileTypeInfo.
// version 39:
//	Added C shell to CBTextFileType.
// version 38:
//	Added Lex to CBTextFileType.
// version 37:
//	Added RaiseBeforeMake/Compile/Run to kCBStaticGlobalID.
//	Added kRightMarginColorIndex to CBPrefsManager color list.
// version 36:
//	Added REXX and Ruby to CBTextFileType.
// version 35:
//	Added Make to CBTextFileType.
// version 34:
//	Added ASP to CBTextFileType.
//	Added various symbol types to CBCtagsUser::Type.
// version 33:
//	Added PHP to CBTextFileType.
// version 32:
//	Added Java Archive to CBTextFileType.
// version 31:
//	Added new file types supported by ctags 4.0.
// version 30:
//	Added Modula-2 and Modula-3 to CBTextFileType.
// version 29:
//	Pref ID zero is no longer valid.
// version 28:
//	CBHTMLStyler adds ?php and JavaScript styles, if not already defined.
// version 27:
//	Moved kCBViewHTMLCmdsID (18) to JXWebBrowser in JX shared prefs.
//	Converted kCBViewInHTMLBrowserSuffixID (19) to External file types.
//	Removed kCBURLPrefixID (20).
// version 26:
//	Added editCmd to FileTypeInfo.
// version 25:
//	JXHelpManager (kCBHelpID = 27) moved to JX shared prefs.
//	JTextEditor::CopyWhenSelect  moved to JX shared prefs. (kCBStaticGlobalID)
//	JXTEBase::UseWindowsHomeEnd  moved to JX shared prefs. (kCBStaticGlobalID)
//	JXTEBase::CaretFollowsScroll moved to JX shared prefs. (kCBStaticGlobalID)
// version 24:
//	Added file types .s .S .r .p
// version 23:
//	Replaced kCBSymbolDialogID (47) with kCBSymbolWindSizeID (47).
// version 22:
//	Incremented CBCStyler version.
// version 21:
//	Adds JXTEBase::windowsHomeEnd and CBTextEditor::scrollCaret to kCBStaticGlobalID.
// version 20:
//	Adds CBCompileDialog::beepAfterMake,beepAfterCompile to kCBStaticGlobalID.
// version 19:
//	Renamed kCBGlobalTextID to kCBStaticGlobalID.
//	Adds CBProjectDocument::reopenFiles, CBProjectTable::dropAction to kCBStaticGlobalID.
// version 18:
//	Adds file suffixes for Eiffel, FORTRAN, and Java.
// version 17:
//	Stores JXFileDocument::itsAskOKToCloseFlag in kCBGlobalTextID.
// version 16:
//	JXHelpManager setup version incremented.
// version 15:
//	Converted variable marker in HTML commands from % to $
// version 14:
//	Resorted FileTypeInfo to put content regexes first.
// version 13:
//	Added C, C++, UNIX script, E-mail, and Outline CRM rule lists.
//	Added CRM id and complement suffix to FileTypeInfo.
//	Added content regexes to file type list.
// version 12:
//	Moved 20000 -> 0, 20004 -> 1
//	Converted from suffix lists.
//		Check CBPrefsManager::ConvertFromSuffixLists() for details.
// version 11:
//	Incremented to keep new CBSearchTextDialog prefs from being
//		destroyed by older version of Code Crusader.
// version 10:
//	Incremented version to avoid incompatibility with Code Medic.
// version 9:
//	Added misc file type info
//	Moved 10003 -> 10004, 10002 -> 10003, 20003 -> 20004
// version 8:
//	Removed id=1 (PS print setup for tree)
//	Removed id=8 (EPS print setup for tree)
// version 7:
//	Moved id=4 (print plain text) to id=16 to make space for
//		recreation of external editor cmds
// version 6:
//	Collected suffixes (2,3,16,17) to (10000,10001,10002,10003)
// version 5:
//	Added default font (kCBDefFontID)
// version 4:
//	Upgrades EPS setup to XEPS setup
//	Added print plain text settings for CBTextEditor
// version 3:
//	Removed id=4 (cmds for using external editor)
// version 2:
//	Removed '&' from external editor commands
// version 1:
//	Stores "local text editing" flag in kViewFileCmdID

const JFileVersion kCurrentProjectFileVersion = 89;

// version 89:
//	Added Java method prototype (from interface).
// version 88:
//	Added support for D & Go.
//	Removed CBFnListDirector.
//	Removed functions from CBClass.
// version 87:
//	Removed show flags from CBTree, because all loners moved to the bottom.
// version 86:
//	CBJavaTreeScanner:
//		* If parent not found in import list, assume package.
//		* Detect inner classes.
// version 85:
//	Added CBPHPTreeDirector.
// version 84:
//	CBTextDocument: removed kCharSetIndex setting.
// version 83:
//	Upgrade to ctags 5.8.
// version 82:
//	CBTextDocument saves kTabInsertsSpacesIndex setting override.
// version 81:
//	Added new file types and symbols supported by ctags 5.7.
// version 80:
//	CBFileListTable stores itsLastUniqueID.
// version 79:
//	Added ctags "PHP interface" type.
// version 78:
//	Fixed bug in ctags PHP parser.
// version 77:
// 	Parses JavaScript functions inside PHP files.
// version 76:
// 	Added Ant file type.
// version 75:
//	Fixed Java superclass parsing.
// version 74:
//	Moved CBSymbolTypeList to prefs file.
// version 73:
// 	Added JavaScript file type.
// version 72:
//	Converted item counts into "keepGoing" booleans, to allow CVS merge.
// version 71:
//	CBProjectDocument saves non-essential information in .jup and .jst, for CVS users.
// version 70:
//	CBTextDocument saves kCharSetIndex setting override.
// version 69:
//	CBTextDocument saves kTabWidthIndex setting override.
// version 68:
//	CBTextDocument saves settings overrides.
// version 67:
//	Added support for CMake.
// version 66:
//	Added C#, Erlang, SML file types.
//	Added ctags "ASP variable" type.
//	Added ctags "Vim autocommand group" type.
// version 65:
//	Added ctags "PHP define" type.
// version 64:
//	Added CBBuildManager::itsSubProjectBuildCmd.
// version 63:
//	Added SymbolInfo::signature.
//	Added Beta, Lua, SLang, SQL, Vera, Verilog file types.
// version 62:
//	CBBuildManager replaces CBCompileDialog and CBRunDialog.
//	Added CBCommandManager.
// version 61:
//	Added Bison file type.
// version 60:
//	Added C shell file type.
// version 59:
//	Added Lex file type.
// version 58:
//	C++ inheritance parser correctly handles declarations within namespaces.
// version 57:
//	Converted CBProjectDocument to use MakefileMethod.
//	CBCompileDialog stores qmake file mod times.
// version 56:
//	Added REXX and Ruby file types.
// version 55:
//	All symbols from Makefiles have file scope.
// version 54:
//	Added flag for marking fully qualified, file scoped symbols.
// version 53:
//	Added new file types and symbols supported by ctags 4.0.
// version 52:
//	Added Java and Qt inheritance and access types to CBClass.
//	Added StreamOut() to CBJavaClass.
// version 51:
//	Changed sorting function for CBSymbolList.
// version 50:
//	CBFileListDirector stores itsActiveFlag.
// version 49:
//	CBClass stores itsIsTemplateFlag.
// version 48:
//	Added CBJavaTreeDirector.
// version 47:
//	CBSymbolDirector stores itsActiveFlag.
// version 46:
//	Reparse all to remove "::name" and add "file:name".
// version 45:
//	Added CBSymbolTypeList.
// version 44:
//	Includes C function prototypes and class members in symbol database.
// version 43:
//	CBTree::itsClassesByFull is sorted case sensitive.
// version 42:
//	Moved file list, symbol list, and C++ class tree to separate .jst file.
// version 41:
//	Added CBSymbolDirector.
// version 40:
//	CBTree no longer stores file names and mod times.
//	CBClass stores file ID instead of file name.
//	Added CBFileListTable.
// version 39:
//	CBRunDialog stores useOutputDoc flag.
//	CBLibraryNode stores itsIncludeInDepListFlag.
//	CBProjectDocument stores itsDepListExpr.
// version 38:
//	CBLibraryNode stores itsShouldBuildFlag.
// version 37:
//	CBProjectDocument stores previous file path.
// version 36:
//	CBProjectDocument stores itsPrintName;
// version 35:
//	CBProjectDocument stores its*WriteMakeFilesFlag and itsTargetName.
// version 34:
//	Stores CBProjectTable setup.
//	CBGroupNode stores open flag.
// version 33:
//	CBProjectDocument stores window geometry.
// version 32:
//	CBProjectDocument stores file tree.
// version 31:
//	CBTreeDirector stores IsActive().
// version 30:
//	Removed type value (=1) from data written by CBClass.
// version 29:
//	CBCompileDialog stores "double space output" flag.
//	CBTextDocument saves PT & PS printing file names.
//	JXHelpManager setup version incremented.
// version 28:
//	CBProjectDocument stores CBCPreprocessor.
// version 27:
//	CBCompileDialog stores makemake file info.
// version 26:
//	Converted variable marker in Compile and Run dialogs from % to $
// version 25:
//	C++ parser strips non-ID characters (e.g. *) from function names,
//		so everything needs to be reparsed.
// version 24:
//	CBProjectDocument stores its window geometry.
//	CBFnListDirector stores scrollbar setup.
// version 23:
//	CBCompile/RunDialog stores window geometry in setup.
// version 22:
//	CBTextDocument and CBTreeWidget use Read/WriteScrollSetup().
// version 21:
//	CBTree saves recurse flag for each directory.
// version 20:
//	CBProjectDocument saves CBFileListDirector setup.
// version 19:
//	CBTree saves itsNeedsMinimizeMILinksFlag.
// version 18:
//	CBTree saves itsVisibleByGeom (as indicies).
//	CBTextDocument and CBTreeWidget save scrolltabs.
// version 17:
//	CBProjectDocument saves setup of each CBFnListDirector.
//	CBProjectDocument saves printer setup in project file.
// version 16:
//	Changes debugging command to "medic -f +%l %f"
// version 15:
//	C++ parser correctly handles inline constructors with initializer
//		lists, so everything needs to be reparsed.
// version 14:
//	CBCompileDialog stores make depend settings.
// version 13:
//	CBTree stores itsShowLoneStructsFlag.
// version 12:
//	CBTree stores itsShowLoneClassesFlag.
// version 11:
//	CBClass stores itsCollapsedFlag in place of itsWasVisibleFlag.
//	CBClass stores itsHas*ChildrenFlag.
// version 10:
//	CBTreeWidget stores scroll position.
//	CBTree stores itsShowEnumsFlag.
//	CBClass stores itsVisibleFlag, itsWasVisibleFlag, itsIsSelectedFlag.
// version 9:
//	CBXTree stores copy of header suffix list.
// version 8:
//	Stores path history menus from CBCompileDialog and CBRunDialog
// version 7:
//	Eliminated CBTree::itsClassNamesSortedFlag
// version 6:
//	Stores CBClass::itsNameStem, CBClass::itsFrame
//	All files in CBTree must be reparsed
// version 5:
//	Stores settings from CBRunDialog
// version 4:
//	All files in CBTree must be reparsed
// version 3:
//	Stores settings from CBCompileDialog
// version 2:
//	Stores configuration of open text editors
// version 1:
//	Stores CBClass::itsDeclType.
//	No longer stores CBClass::itsFirstFoundParent

const JFileVersion kCurrentProjectTmplVersion = 5;

// version 5:
//	Adjustments for non-essential data moved to .jup and .jst (projVers=71).
// version 4:
//	Added support for CMake.
// version 3:
//	Moved itsMakefileMethod,itsTargetName,itsDepListExpr to CBBuildManager::WriteTemplate()
//	Removed CBRunDialog.
// version 2:
//	Converted to CBProjectDocument::MakefileMethod.
//	CBCompileDialog saves qmake files.
//	CBCompileDialog saves Makefile if nothing else exists.
// version 1:
//	Doesn't save Make.files, Make.header if they don't exist.

const JFileVersion kCurrentProjectWizardVersion = 0;

#endif
