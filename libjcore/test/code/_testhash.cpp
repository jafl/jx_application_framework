/******************************************************************************
 testhash.cpp

	Test JHash parameter code.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <unistd.h>
#include <JStrValue.h>
#include <JMMRecord.h>
#include <JHashRecord.tmpl>
#include <jAssert.h>

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "\nTest system parameters relevant to JHash" << std::endl;

	std::cout << "\n   system page size: " << getpagesize() << std::endl;

	std::cout << std::endl;

	JMMRecord scratch1;
	scratch1.PrintLayout();

	JHashRecord< JStrValue<int> > scratch2;
	scratch2.PrintLayout();

	return 0;
}