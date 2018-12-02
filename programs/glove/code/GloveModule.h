#ifndef _H_GLModule
#define _H_GLModule

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

