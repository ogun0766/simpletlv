#include <stdio.h>
#include <string.h>
#include "TagLengthValue.h"

void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

int main (int argc, char** argv)
{
    TLV* t = NULL;
    TLV* t2 = NULL;
    TLV* t3 = NULL;
    TLV* t4 = NULL;
    TLV* t5 = NULL;
    TLV* tarr[2];
    TLV_CONTAINER* tc = NULL;
    uint32_t test = 10;
    char s[32] = {0,};
    void* buf = NULL;
    uint32_t len = 0;

    initTlv(&t, sizeof(uint32_t));

    setTag(0x2, t);
    setValue(&test, t);

    test = 0;

    printf("tag : %d length : %d value : %p test : %d\n",
            getTag(t), getLength(t), getValue(t), test);

    test = (*(int*)getValue(t));
    printf("test = %d\n", test);

    strcpy(s, "hello!");
    s[sizeof("hello!")] = '\0';

    initTlv(&t2, sizeof(char) * 32);
    setTag(0x1, t2);
    setValue(s, t2);

    printf("tag : %d length : %d value : %s\n",
            getTag(t2), getLength(t2), (char*) getValue(t2));

    tarr[0] = t;
    tarr[1] = t2;
    createTlvContainer(2, tarr, &tc);
    hexDump("TLV", t, getLength(t) + sizeof(uint32_t));
    hexDump("TLV_CONTAINER", tc, getTlvContainerLength(tc));
    printf("count : %d len : %d\n", getTlvCount(tc), getTlvContainerLength(tc));
    t3 = getTlv(0, tc);
    printf("tag = %d length = %d\n", getTag(t3), getLength(t3));
    test = (*(int*)getValue(t3));
    printf("test = %d\n", test);
    t3 = getTlv(1, tc);
    printf("tag = %d length = %d\n", getTag(t3), getLength(t3));
    printf("value=%s\n", (char*)getValue(t3));

    initTlv(&t4, 0);
    setTag(0x3, t4);
    tarr[0] = t3;
    tarr[1] = t;
    setTlvs(2, tarr, &t4);
    hexDump("TLV", t4, getLength(t4) + sizeof(uint32_t));

    buf = getValue(t4);
    len = getLength(t4);

    while (buf != NULL)
    {
        printf("buf = %p len = %d\n", buf, len);
        buf = getTlvFromBuffer(buf, &len, &t5);
        printf("t5 tag=%d length=%d value=%p\n",
                getTag(t5), getLength(t5), getValue(t5));
    }

    return 0;
}
