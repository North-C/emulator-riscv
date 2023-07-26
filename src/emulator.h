#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "types.h"

#define fatalf(format, ...)                                                  \
    (fprintf(stderr, "file: %s:%d, function: %s --- " format "\n", __FILE__, \
             __LINE__, __FUNCTION__, __VA_ARGS__))

#define BUG(msg)           \
    {                      \
        fatalf("%s", msg); \
        exit(1);           \
    }

#define Fatal(msg)         \
    {                      \
        fatalf("%s", msg); \
        exit(1);           \
    }

#define Unreachable() (fatalf("unreachable"), __bulitin_unreadchable())

#define ROUNDDOWN(x, y) ((x) & ~(y - 1))
#define ROUNDUP(x, y) (((x) + (y - 1)) & ~(y - 1))

#define GUEST_MEMORY_OFFSET 0x088800000000ULL
#define TO_HOST(addr) (addr + GUEST_MEMORY_OFFSET)
#define TO_GUEST(addr) (addr - GUEST_MEMORY_OFFSET)

#define MAX(a, b)           \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        (void)(&_a == &_b); \
        _a > _b ? _a : _b;  \
    })

#define MIN(a, b)           \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        (void)(&_a == &_b); \
        _a < _b ? _a : _b;  \
    })

/**
 * mmu.c
 */
typedef struct {
    u64 entry;
    u64 host_alloc;
    u64 base;         // guest 内存的基地址
    u64 guest_alloc;  // 后续guest使用的内存堆顶
} mmu_t;
void mmu_load_elf(mmu_t*, int);

/**
 * state.c
 */
typedef struct {
    u64 gp_regs[32];  // 寄存器位置
    u64 pc;           // 程序执行的位置
} state_t;

/**
 * machine.c
 */
typedef struct {
    state_t state;
    mmu_t mmu;
} machine_t;
void machine_load_program(machine_t*, char*);

#endif