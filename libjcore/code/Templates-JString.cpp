#include <JCoreStdInc.h>
#include <JString.h>

#define JTemplateType JString

#define JPtrArrayCopy
#include <JPtrArray.tmpls>
#undef JPtrArrayCopy

#define JStringPtrMapCopy
#include <JStringPtrMap.tmpls>
#undef JStringPtrMapCopy

#undef JTemplateType

#define JTemplateDataType    JString
#define JTemplateStorageType JArray<JString*>

#define JPtrStackCopy
#include <JPtrStack.tmpls>
#undef JPtrStackCopy

#define JPtrQueueCopy
#include <JPtrQueue.tmpls>
#undef JPtrQueueCopy

#undef JTemplateDataType
#undef JTemplateStorageType

#define JTemplateType JStrValue<JString*>
#include <JHashTable.tmpls>
#undef JTemplateType

#define JTemplateType JString*
#include <JAliasArray.tmpls>
#undef JTemplateType
