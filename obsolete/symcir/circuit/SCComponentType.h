/******************************************************************************
 SCComponentType.h

	Defines all known types of components.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCComponentType
#define _H_SCComponentType

#include <jTypes.h>

// Do not change these values once they are assigned
// because they are stored in files

enum SCComponentType
{
	kResistor     = 0,
	kCapacitor    = 1,
	kInductor     = 2,
	kShortCircuit = 3,

	kVACSource = 10,
	kIACSource = 11,

	kVdepVSource = 20,
	kIdepISource = 21,
	kVdepISource = 22,
	kIdepVSource = 23
};

const JCharacter kResistorPrefix  = 'R';
const JCharacter kCapacitorPrefix = 'C';
const JCharacter kInductorPrefix  = 'L';

const JCharacter kVACSourcePrefix = 'V';
const JCharacter kIACSourcePrefix = 'I';

const JCharacter kVdepVSourcePrefix = 'E';
const JCharacter kIdepISourcePrefix = 'F';
const JCharacter kVdepISourcePrefix = 'G';
const JCharacter kIdepVSourcePrefix = 'H';

istream& operator>>(istream& input, SCComponentType& type);
ostream& operator<<(ostream& output, const SCComponentType type);

#endif
