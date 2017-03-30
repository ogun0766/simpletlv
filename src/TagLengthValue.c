#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TagLengthValue.h"

typedef struct __TLV
{
    uint32_t tagLength;
} TLV;

typedef struct __TLV_CONTAINER
{
    uint32_t tlvCount;
} TLV_CONTAINER;

void initTlv (TLV** tlv, uint32_t length)
{
    uint32_t len = 0;
    uint8_t* buffer = NULL;
    if (NULL == tlv) return;

    len = (length & 0x00FFFFFF);
    buffer = malloc(sizeof(uint32_t) + sizeof(uint8_t) * len);
    *tlv = (TLV*) buffer;
    (*tlv)->tagLength = len;
}

void destroyTlv (TLV** tlv)
{
    if (NULL == tlv) return;
    if (NULL == *tlv) return;

    free(*tlv);
    *tlv = NULL;
}

void setTag (uint8_t tag, TLV* tlv)
{
    uint32_t t = 0;
    if (NULL == tlv) return;

    t = ((uint32_t) tag) << 24;
    tlv->tagLength &= 0x00FFFFFF;
    tlv->tagLength |= t;
}

void setLength (uint32_t length, TLV** tlv)
{
    uint8_t tag = 0;
    if (NULL == tlv) return;

    tag = getTag(*tlv);

    if (NULL != *tlv)
    {
        free(*tlv);
        *tlv = NULL;
    }

    initTlv(tlv, length);
    setTag(tag, *tlv);
}

void setTagLength (uint8_t tag, uint32_t length, TLV** tlv)
{
    if (NULL == tlv) return;
    if (NULL != *tlv)
    {
        free(*tlv);
        *tlv = NULL;
    }

    initTlv(tlv, length);
    setTag(tag, *tlv);
}

void setValue (const void* value, TLV* tlv)
{
    uint32_t length = 0;
    uint8_t* p = NULL;
    if (NULL == tlv) return;

    if ((length = getLength(tlv)) <= 0) return;

    p = (uint8_t*) tlv;
    p += (sizeof(uint8_t) * 4);
    memcpy(p, value, sizeof(uint8_t) * length);
}

void setTlvs (uint32_t tlvCount, const TLV* tlvs[], TLV** tlv)
{
    uint32_t len = 0;
    uint32_t i = 0;
    uint8_t* p = NULL;

    if (tlvCount <= 0 || tlvs == NULL || tlv == NULL || *tlv == NULL) return;

    for (i = 0; i < tlvCount; i++)
    {
        len += (getLength(tlvs[i]) + sizeof(uint32_t));
    }

    if (len > getLength(*tlv))
    {
        setLength(len, tlv);
    }

    p = (((uint8_t*) *tlv) + sizeof(uint32_t));
    for (i = 0; i < tlvCount; i++)
    {
        uint32_t l = sizeof(uint32_t) + getLength(tlvs[i]) * sizeof(uint8_t);
        memcpy(p, tlvs[i], l);
        p += l;
    }
}

void setValueWithLength (const void* value, uint32_t length, TLV** tlv)
{
    if (NULL == tlv) return;

    setLength(length, tlv);
    setValue(value, *tlv);
}

uint8_t getTag (const TLV* tlv)
{
    if (NULL == tlv) return 0;

    return (uint8_t)((tlv->tagLength & 0xFF000000) >> 24);
}

uint32_t getLength (const TLV* tlv)
{
    if (NULL == tlv) return 0;

    return (tlv->tagLength & 0x00FFFFFF);
}

uint32_t getTagLength (const TLV* tlv)
{
    if (NULL == tlv) return 0;

    return tlv->tagLength;
}

void* getValue (const TLV* tlv)
{
    uint8_t* p = NULL;
    if (NULL == tlv) return NULL;

    p = (uint8_t*) tlv;
    p += (sizeof(uint8_t) * 4);
    return p;
}

void* getTlvFromBuffer (const void* buffer, uint32_t* bufferLength, TLV** tlv)
{
    uint8_t* ret = NULL;
    uint32_t len = 0;

    if (*bufferLength <= 0) return NULL;

    *tlv = (TLV*) buffer;
    len = getLength(*tlv);

    if (*bufferLength - (len + sizeof(uint32_t)) >= sizeof(uint32_t))
    {
        ret = ((uint8_t*) buffer) + sizeof(uint32_t) + sizeof(uint8_t) * len;
        *bufferLength = *bufferLength - (len + sizeof(uint32_t));
    }

    return ret;
}

uint8_t getUINT8Value (const TLV* tlv)
{
    uint8_t ret = 0;

    if (NULL == tlv) return ret;
    if (getTag(tlv) != TLV_PRIMITIVE_UINT8) return ret;
    if (getLength(tlv) != sizeof(uint8_t)) return ret;

    ret = *((uint8_t*) getValue(tlv));

    return ret;
}

uint16_t getUINT16Value (const TLV* tlv)
{
    uint16_t ret = 0;

    if (NULL == tlv) return ret;
    if (getTag(tlv) != TLV_PRIMITIVE_UINT16) return ret;
    if (getLength(tlv) != sizeof(uint16_t)) return ret;

    ret = *((uint16_t*) getValue(tlv));

    return ret;
}

uint32_t getUINT32Value (const TLV* tlv)
{
    uint32_t ret = 0;

    if (NULL == tlv) return ret;
    if (getTag(tlv) != TLV_PRIMITIVE_UINT32) return ret;
    if (getLength(tlv) != sizeof(uint32_t)) return ret;

    ret = *((uint32_t*) getValue(tlv));

    return ret;
}

uint64_t getUINT64Value (const TLV* tlv)
{
    uint64_t ret = 0;

    if (NULL == tlv) return ret;
    if (getTag(tlv) != TLV_PRIMITIVE_UINT64) return ret;
    if (getLength(tlv) != sizeof(uint64_t)) return ret;

    ret = *((uint64_t*) getValue(tlv));

    return ret;
}

char* getStringValue (const TLV* tlv)
{
    if (NULL == tlv) return NULL;
    if (getTag(tlv) != TLV_PRIMITIVE_STRING) return NULL;
    if (getLength(tlv) <= 0) return NULL;

    return (char*) getValue(tlv);
}

static uint32_t getTlvArrayLength (uint32_t tlvCount, const TLV* tlv[])
{
    uint32_t i = 0;
    uint32_t len = 0;

    if (tlvCount <= 0) return 0;
    if (tlv == NULL) return 0;

    for (i = 0; i < tlvCount; i++)
    {
        len += (getLength(tlv[i]) + sizeof(uint32_t));
    }

    return len;
}

void createTlvContainerToBuffer (uint32_t tlvCount, const TLV* tlv[],
        uint8_t* buffer, size_t bufferLength)
{
    uint32_t len = getTlvArrayLength(tlvCount, tlv);
    TLV_CONTAINER* tc = (TLV_CONTAINER*) buffer;
    TLV* p = NULL;
    uint32_t i = 0;

    if (bufferLength < len + sizeof(uint32_t)) return;

    tc->tlvCount = tlvCount;
    p = (TLV*) (buffer + sizeof(uint32_t));

    len = 0;
    for (i = 0; i < tlvCount; i++)
    {
        p->tagLength = tlv[i]->tagLength;
        len = getLength(p);
        p = (TLV*) (((uint8_t*) p) +sizeof(uint32_t));
        memcpy(p, ((uint8_t*)tlv[i]) + sizeof(uint32_t), len * sizeof(uint8_t));
        p = (TLV*) (((uint8_t*) p) + (len * sizeof(uint8_t)));
    }
}

void createTlvContainer (uint32_t tlvCount, const TLV* tlv[], TLV_CONTAINER** container)
{
    uint32_t len = 0;
    uint8_t* buffer = NULL;
    TLV* p = NULL;

    if (tlvCount <= 0) return;
    if (tlv == NULL) return;
    if (container == NULL) return;

    len = getTlvArrayLength(tlvCount, tlv);

    len += sizeof(uint32_t);

    buffer = (uint8_t*) malloc(sizeof(uint8_t) * len);
    if (buffer == NULL) return;

    *container = (TLV_CONTAINER*) buffer;

    createTlvContainerToBuffer(tlvCount, tlv, buffer, len);
}

void destroyTlvContainer (TLV_CONTAINER** container)
{
    if (NULL == container) return;
    if (NULL == *container) return;

    free(*container);
    *container = NULL;
}

uint32_t getTlvCount (const TLV_CONTAINER* container)
{
    if (NULL == container) return 0;

    return container->tlvCount;
}

uint32_t getTlvContainerLength (const TLV_CONTAINER* container)
{
    uint32_t len = 0;
    uint32_t i = 0;
    TLV* p = NULL;

    len += sizeof(uint32_t);

    p = (TLV*) (((uint8_t*) container) + sizeof(uint32_t));

    for (i = 0; i < container->tlvCount; i++)
    {
        len += sizeof(uint32_t);
        uint32_t l = getLength(p);
        len += l;
        p = (TLV*) (((uint8_t*)p) + (sizeof(uint32_t) + (sizeof(uint8_t) * l)));
    }

    return len;
}

TLV* getTlv (uint32_t index, const TLV_CONTAINER* container)
{
    TLV* p = NULL;
    uint32_t i = 0;

    if (container == NULL) return NULL;
    if (index >= container->tlvCount) return NULL;

    p = (TLV*) (((uint8_t*) container) + sizeof(uint32_t));
    for (i = 0; i < container->tlvCount; i++)
    {
        if (i == index) return p;
        p = (TLV*) (((uint8_t*)p) + (sizeof(uint32_t) + (sizeof(uint8_t) * getLength(p))));
    }

    return NULL;
}
