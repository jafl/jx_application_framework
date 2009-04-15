#define JTemplateType JCharacter
#include <JStringPtrMap.tmpls>

#undef JTemplateType
#define JTemplateType JStrValue<JCharacter*>
#include <JHashTable.tmpls>
