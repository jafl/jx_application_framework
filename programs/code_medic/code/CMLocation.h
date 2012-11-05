/******************************************************************************
 CMLocation.h

	Copyright © 2001 John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_CMLocation
#define _H_CMLocation

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JString.h>
#include <JFileID.h>

class CMLocation
{
	friend JBoolean operator==(const CMLocation& lhs, const CMLocation& rhs);
	friend JBoolean operator!=(const CMLocation& lhs, const CMLocation& rhs);

public:

	CMLocation();
	CMLocation(const JCharacter* fileName, const JIndex lineNumber);
	CMLocation(const CMLocation& source);

	~CMLocation();

	const CMLocation& operator=(const CMLocation& source);

	const JString&	GetFileName() const;
	const JFileID&	GetFileID() const;
	void			SetFileName(const JCharacter* fileName);

	JIndex	GetLineNumber() const;
	void	SetLineNumber(const JIndex lineNumber);

	// not included in comparators

	const JString&	GetFunctionName() const;
	void			SetFunctionName(const JCharacter* fnName);

	const JString&	GetMemoryAddress() const;
	void			SetMemoryAddress(const JCharacter* addr);

private:

	JString	itsFileName;
	JFileID	itsFileID;
	JIndex	itsLineNumber;

	JString	itsFunctionName;
	JString	itsMemoryAddress;
};


/******************************************************************************
 File name

 *****************************************************************************/

inline const JString&
CMLocation::GetFileName() const
{
	return itsFileName;
}

inline void
CMLocation::SetFileName
	(
	const JCharacter* fileName
	)
{
	itsFileName = fileName;
	itsFileID.SetFileName(fileName);
}

/******************************************************************************
 GetFileID

 *****************************************************************************/

inline const JFileID&
CMLocation::GetFileID() const
{
	return itsFileID;
}

/******************************************************************************
 Line number

 *****************************************************************************/

inline JIndex
CMLocation::GetLineNumber() const
{
	return itsLineNumber;
}

inline void
CMLocation::SetLineNumber
	(
	const JIndex lineNumber
	)
{
	itsLineNumber = lineNumber;
}

/******************************************************************************
 Function name

 *****************************************************************************/

inline const JString&
CMLocation::GetFunctionName() const
{
	return itsFunctionName;
}

inline void
CMLocation::SetFunctionName
	(
	const JCharacter* fnName
	)
{
	itsFunctionName = fnName;
}

/******************************************************************************
 Memory address

 *****************************************************************************/

inline const JString&
CMLocation::GetMemoryAddress() const
{
	return itsMemoryAddress;
}

inline void
CMLocation::SetMemoryAddress
	(
	const JCharacter* addr
	)
{
	itsMemoryAddress = addr;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

inline JBoolean
operator!=
	(
	const CMLocation& lhs,
	const CMLocation& rhs
	)
{
	return JNegate(lhs == rhs);
}

#endif
