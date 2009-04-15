#define JTemplateType int
#include <JStrValue.tmpls>
#undef JTemplateType

#include <JStrValue.h>
#define JTemplateType JStrValue<int>
#include <JHashTable.tmpls>
