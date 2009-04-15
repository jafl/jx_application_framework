#define JTemplateType double
#include <JCoreStdInc.h>
#include <JDynamicHistogram.tmpls>
#include <JMinMax.tmpls>
#undef JTemplateType

#define JTemplateType JArray<double>
#define JPtrArrayCopy
#include <JPtrArray.tmpls>
#undef JPtrArrayCopy
#undef JTemplateType
