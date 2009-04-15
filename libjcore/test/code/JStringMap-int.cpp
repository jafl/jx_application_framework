#define JTemplateType int
#include <JStringMap.tmpls>

#undef JTemplateType
#define JTemplateType JStrValue<int>
#include <JHashTable.tmpls>
