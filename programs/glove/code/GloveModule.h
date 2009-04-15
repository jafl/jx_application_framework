#ifndef _H_GloveImport
#define _H_GloveImport

enum
{
//	File import/export module values
	kGloveDataError = 0,
	kGloveMatrixDataFormat,
	kGloveRaggedDataFormat,
	
//  General module values
	kGloveOK,
	kGloveFail,
	kGloveFinished,

//	Data module values
	kGloveRequiresData,
	kGloveRequiresNoData,
	kGloveDataDump,
	kGloveDataControl,
	
//  Fit module values
	kGloveXY,
	kGloveXYdY,
	kGloveXYdX,
	kGloveXYdYdX
};

#endif

