/*
 * Copyright (C) 2014 Raphael S. Carvalho <raphael.scarv@gmail.com>
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct gdt_desc
{
    uint16_t limit_0_15;                // 0:15
    uint16_t base_0_15;                 // 16:32
    uint8_t base_16_23;                 // 32:39
    struct access_byte {                // 40:47
        uint8_t access  : 1;
        uint8_t rw      : 1;
        uint8_t dc      : 1;
        uint8_t ex      : 1;
        uint8_t one     : 1;
        uint8_t privl   : 2;
        uint8_t pr      : 1;
    } ab;
    union {
        uint8_t limit_16_19 : 4;        // 48:51
        struct flags {
            uint8_t unused      : 4;
            uint8_t zero        : 1;    // 52:55
            uint8_t l           : 1;    // Used for x86-64 mode.
            uint8_t sz          : 1;
            uint8_t gran        : 1;
        } fl;
    };
    uint8_t base_24_31;                 // 56:63
};

static inline uint32_t build_base(uint16_t base_0_15, uint8_t base_16_23,
    uint8_t base_24_31) {
    return base_0_15 | (base_16_23 << 16) | (base_24_31 << 24);
}

static inline uint32_t build_limit(uint16_t limit_0_15, uint8_t limit_16_19)
{
    return limit_0_15 | ((limit_16_19 & 0xF) << 16);
}

static inline void dump_access_byte(struct access_byte ab)
{
    assert(ab.one == 1);
    printf("Accessed=%d, RW=%d, DC=%d<grows %s>, Exec=%d, "
        "DPL(ring level)=%d<%s>, Present=%d\n",
        ab.access, ab.rw,
        ab.dc, (!ab.dc) ? "up" : "down",
        ab.ex,
        ab.privl, (ab.privl != 3) ? "system" : "user",
        ab.pr);
}


static inline void dump_flags(struct flags fl)
{
    assert(fl.zero == 0);
    printf("L(x86-64 only)=%d, Size=%d<pm %d>, Granularity=%d\n",
        fl.l, fl.sz, (fl.sz) ? 32 : 16, fl.gran);
}

int main(int argc, char **argv)
{
    struct gdt_desc desc;
    uint64_t qword;
    uint32_t base, limit;

    assert(sizeof(struct gdt_desc) == 8);
    if (argc != 2) {
        printf("%s <gdt descriptor>\n", argv[0]);
        return -1;
    }

    // Copy 64-bit value into the descriptor structure.
    qword = strtoll(argv[1], NULL, 16);
    memcpy(&desc, &qword, sizeof(struct gdt_desc));

    printf("Segment descriptor details\n");
    printf("---------------------------\n");
    // Detect whether or not the descriptor is 64-bit.
    if (desc.fl.l) {
        printf("Mode: Long mode\n");
        printf("Base=0, Limit=ffffffffffffffff\n");
        if (desc.fl.sz) {
            printf("Warning: the flag Size should be unset for long mode!\n");
        }
    } else {
        printf("Mode: %d-bit Protected mode\n", (desc.fl.sz) ? 32 : 16);
        base = build_base(desc.base_0_15, desc.base_0_15, desc.base_24_31);
        limit = build_limit(desc.limit_0_15, desc.limit_16_19);
        printf("Base=%08x, Limit=%08x\n", base, limit);
    }
    printf("* Access byte:\n");
    dump_access_byte(desc.ab);
    printf("* Flags:\n");
    dump_flags(desc.fl);

    return 0;
}
