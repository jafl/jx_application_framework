/******************************************************************************
 CBEmulator.h

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEmulator
#define _H_CBEmulator

#include <jTypes.h>

class JTEKeyHandler;
class CBTextEditor;

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum CBEmulator
{
	kCBNoEmulator = 0,
	kCBVIEmulator,			// = kCBLastEmulator	// special

	// When you add new types, be sure to increment the prefs version!

	kCBFirstEmulator = kCBNoEmulator,
	kCBLastEmulator  = kCBVIEmulator
};

const JSize kCBEmulatorCount = kCBLastEmulator+1;

JBoolean	CBInstallEmulator(const CBEmulator type, CBTextEditor* editor,
							  JTEKeyHandler** handler);

std::istream& operator>>(std::istream& input, CBEmulator& type);
std::ostream& operator<<(std::ostream& output, const CBEmulator type);

#endif
