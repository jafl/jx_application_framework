/******************************************************************************
 CMLocation.h

	Copyright (C) 2001 John Lindal.

 *****************************************************************************/

#ifndef _H_CMLocation
#define _H_CMLocation

#include <JString.h>
#include <JFileID.h>

class CMLocation
{
	friend bool operator==(const CMLocation& lhs, const CMLocation& rhs);
	friend bool operator!=(const CMLocation& lhs, const CMLocation& rhs);

public:

	CMLocation();
	CMLocation(const JString& fileName, const JIndex lineNumber);
	CMLocation(const CMLocation& source);

	~CMLocation();

	const CMLocation& operator=(const CMLocation& source);

	const JString&	GetFileName() const;
	const JFileID&	GetFileID() const;
	void			SetFileName(const JString& fileName);

	JIndex	GetLineNumber() const;
	void	SetLineNumber(const JIndex lineNumber);

	// not included in comparators

	const JString&	GetFunctionName() const;
	void			SetFunctionName(const JString& fnName);

	const JString&	GetMemoryAddress() const;
	void			SetMemoryAddress(const JString& addr);

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
	const JString& fileName
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
	const JString& fnName
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
	const JString& addr
	)
{
	itsMemoryAddress = addr;
}

/******************************************************************************
 Comparison

 *****************************************************************************/

inline bool
operator!=
	(
	const CMLocation& lhs,
	const CMLocation& rhs
	)
{
	return lhs != rhs;
}

#endif
