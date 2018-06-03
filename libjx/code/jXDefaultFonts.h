/******************************************************************************
 jXDefaultFonts.cpp

	Copyright (C) 2018 John Lindal.

 ******************************************************************************/

static const JUtf8Byte* kMonospaceFontName =
	#ifdef _J_OSX
	"Menlo";
	#else
	"Bitstream Vera Sans Mono";
	#endif

static const JUtf8Byte* kArabicDefaultFontName =
	#ifdef _J_OSX
	"Times New Roman";	//"Arial Unicode MS";
	#else
	?;
	#endif

static const JUtf8Byte* kArmenianDefaultFontName =
	#ifdef _J_OSX
	"Mshtakan";
	#else
	?;
	#endif

static const JUtf8Byte* kCJKDefaultFontName =
	#ifdef _J_OSX
	"PingFang SC";	//"Arial Unicode MS";
	#else
	?;
	#endif

static const JUtf8Byte* kCyrillicDefaultFontName =
	#ifdef _J_OSX
	"Arial Unicode MS";
	#else
	?;
	#endif

static const JUtf8Byte* kGreekDefaultFontName =
	#ifdef _J_OSX
	"Arial Unicode MS";
	#else
	?;
	#endif

static const JUtf8Byte* kHebrewDefaultFontName =
	#ifdef _J_OSX
	"Arial Hebrew";	//"Arial Unicode MS";
	#else
	?;
	#endif

static const JUtf8Byte* kLatinDefaultFontName =
	#ifdef _J_OSX
	"Arial";
	#else
	"Helvetica";
	#endif
