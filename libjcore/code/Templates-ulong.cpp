#define JTemplateType unsigned long
#include <JCoreStdInc.h>
#include <JStringMap.tmpls>
#include <JDynamicHistogram.tmpls>
#include <JMinMax.tmpls>
#undef JTemplateType

#define JTemplateType JArray<unsigned long>
#define JPtrArrayCopy
#include <JPtrArray.tmpls>
#undef JPtrArrayCopy
#undef JTemplateType

#define JTemplateType unsigned long,JArray<unsigned long>
#include <JStack.tmpls>
#undef JTemplateType

#define JTemplateType JStrValue<unsigned long>
#include <JHashTable.tmpls>
#undef JTemplateType
