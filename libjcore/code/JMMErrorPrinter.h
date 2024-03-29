/******************************************************************************
 JMMErrorPrinter.h

	Interface for the JMMErrorPrinter class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_JMMErrorPrinter
#define _H_JMMErrorPrinter

// Superclass Header
#include "JMMMonitor.h"

#include "jTypes.h"

class JMMErrorPrinter : public JMMMonitor
{
public:

	JMMErrorPrinter();
	~JMMErrorPrinter() override;

	bool GetPrintErrors() const;
	void    SetPrintErrors(const bool print);

protected:

	void HandleObjectDeletedAsArray(const JMMRecord& record) override;
	void HandleArrayDeletedAsObject(const JMMRecord& record) override;

	void HandleUnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
								   const bool isArray) override;
	void HandleMultipleDeletion(const JMMRecord& thisRecord,
								const JUtf8Byte* file, const JUInt32 line,
								const bool isArray) override;

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord) override;

private:

	bool itsPrintErrorsFlag;
};

/******************************************************************************
 GetPrintErrors

 *****************************************************************************/

inline bool
JMMErrorPrinter::GetPrintErrors() const
{
	return itsPrintErrorsFlag;
}

/******************************************************************************
 SetPrintErrors

	Sets whether error notifications will be printed.  Overrides the
	JMM_NO_PRINT_ERRORS environment variable.

 *****************************************************************************/

inline void
JMMErrorPrinter::SetPrintErrors
	(
	const bool print
	)
{
	itsPrintErrorsFlag = print;
}

#endif
