#include <GAddressBookEntry.h>
#define JTemplateType GAddressBookEntry

#include <JStringPtrMap.tmpls>

#define JTemplateName JStrValue
#include <instantiate_template.h>
#undef JTemplateName

#undef JTemplateType
#define JTemplateType JStrValue<GAddressBookEntry*>
#include <JHashTable.tmpls>
