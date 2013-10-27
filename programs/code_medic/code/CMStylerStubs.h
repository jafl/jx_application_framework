/******************************************************************************
 CMStylerStubs.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMStylerStubs
#define _H_CMStylerStubs

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"

class CBBourneShellStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBCSharpStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBCShellStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBEiffelStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBINIStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBJavaScriptStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBPerlStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBPythonStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBRubyStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

class CBTCLStyler
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();
};

#endif
