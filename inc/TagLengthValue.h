#ifndef __TAG_LENGTH_VALUE_H__
#define __TAG_LENGTH_VALUE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct __TLV;
struct __TLV_CONTAINER;

typedef struct __TLV TLV;
typedef struct __TLV_CONTAINER TLV_CONTAINER;

enum
{
    TLV_PRIMITIVE_UINT8 = 0x0,
    TLV_PRIMITIVE_UINT16 = 0x1,
    TLV_PRIMITIVE_UINT32 = 0x2,
    TLV_PRIMITIVE_UINT64 = 0x3,
    TLV_PRIMITIVE_STRING = 0x4,
    TLV_CONTAINER = 0xFF,
};

void initTlv (TLV** tlv, uint32_t length);

void destroyTlv (TLV** tlv);

void setTag (uint8_t tag, TLV* tlv);

void setLength (uint32_t length, TLV** tlv);

void setTagLength (uint8_t tag, uint32_t length, TLV** tlv);

void setValue (const void* value, TLV* tlv);

void setTlvs (uint32_t tlvCount, const TLV* tlvs[], TLV** tlv);

void setValueWithLength (const void* value, uint32_t length, TLV** tlv);

uint8_t getTag (const TLV* tlv);

uint32_t getLength (const TLV* tlv);

uint32_t getTagLength (const TLV* tlv);

void* getValue (const TLV* tlv);

void* getTlvFromBuffer (const void* buffer, uint32_t* bufferLength, TLV**tlv);

uint8_t getUINT8Value (const TLV* tlv);

uint16_t getUINT16Value (const TLV* tlv);

uint32_t getUINT32Value (const TLV* tlv);

uint64_t getUINT64Value (const TLV* tlv);

char* getStringValue (const TLV* tlv);

void createTlvContainerToBuffer (uint32_t tlvCount, const TLV* tlv[],
        uint8_t* buffer, size_t bufferLength);

void createTlvContainer (uint32_t tlvCount, const TLV* tlv[], TLV_CONTAINER** container);

void destroyTlvContainer (TLV_CONTAINER** container);

uint32_t getTlvCount (const TLV_CONTAINER* container);

uint32_t getTlvContainerLength (const TLV_CONTAINER* container);

TLV* getTlv (uint32_t index, const TLV_CONTAINER* container);

#ifdef __cplusplus
}
#endif

#endif
