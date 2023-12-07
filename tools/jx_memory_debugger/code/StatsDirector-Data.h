// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_StatsDirector_Data
#define _H_StatsDirector_Data

static const JUtf8Byte* kDataMenuStr =
"* %i GetRecords::MDStatsDirector %l"
"|* %i SaveSnapshot::MDStatsDirector"
"|* %i DiffSnapshot::MDStatsDirector %l"
"|* %b %i ShowAppRecordsAction::MDStatsDirector"
"|* %b %i ShowBucket1RecordsAction::MDStatsDirector"
"|* %b %i ShowBucket2RecordsAction::MDStatsDirector"
"|* %b %i ShowBucket3RecordsAction::MDStatsDirector %l"
"|* %b %i ShowLibraryRecordsAction::MDStatsDirector %l"
"|* %b %i ShowInternalRecordsAction::MDStatsDirector"
"|* %b %i ShowUnknownRecordsAction::MDStatsDirector"
;

enum {
	kGetRecordsCmd=1,
	kSaveSnapshotCmd,
	kDiffSnapshotCmd,
	kShowAppRecordsCmd,
	kShowBucket1RecordsCmd,
	kShowBucket2RecordsCmd,
	kShowBucket3RecordsCmd,
	kShowLibraryRecordsCmd,
	kShowIntervalRecordsCmd,
	kShowUnknownRecordsCmd,
};


static void ConfigureDataMenu(JXTextMenu* menu, const int offset = 0) {
	if (menu->IsEmpty() && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#d", JString::kNoCopy));
	}
};

#endif