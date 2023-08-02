#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "reg.h"
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

#define unreachable()                   \
    {                                   \
        fprintf(stderr, "unreachable"); \
        assert(0);                      \
    }

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
 * decode.c
 */
enum insn_type_t {
    insn_lb,
    insn_lh,
    insn_lw,
    insn_ld,
    insn_lbu,
    insn_lhu,
    insn_lwu,
    insn_fence,
    insn_fence_i,
    insn_addi,
    insn_slli,
    insn_slti,
    insn_sltiu,
    insn_xori,
    insn_srli,
    insn_srai,
    insn_ori,
    insn_andi,
    insn_auipc,
    insn_addiw,
    insn_slliw,
    insn_srliw,
    insn_sraiw,
    insn_sb,
    insn_sh,
    insn_sw,
    insn_sd,
    insn_add,
    insn_sll,
    insn_slt,
    insn_sltu,
    insn_xor,
    insn_srl,
    insn_or,
    insn_and,
    insn_mul,
    insn_mulh,
    insn_mulhsu,
    insn_mulhu,
    insn_div,
    insn_divu,
    insn_rem,
    insn_remu,
    insn_sub,
    insn_sra,
    insn_lui,
    insn_addw,
    insn_sllw,
    insn_srlw,
    insn_mulw,
    insn_divw,
    insn_divuw,
    insn_remw,
    insn_remuw,
    insn_subw,
    insn_sraw,
    insn_beq,
    insn_bne,
    insn_blt,
    insn_bge,
    insn_bltu,
    insn_bgeu,
    insn_jalr,
    insn_jal,
    insn_ecall,
    insn_csrrc,
    insn_csrrci,
    insn_csrrs,
    insn_csrrsi,
    insn_csrrw,
    insn_csrrwi,
    insn_flw,
    insn_fsw,
    insn_fmadd_s,
    insn_fmsub_s,
    insn_fnmsub_s,
    insn_fnmadd_s,
    insn_fadd_s,
    insn_fsub_s,
    insn_fmul_s,
    insn_fdiv_s,
    insn_fsqrt_s,
    insn_fsgnj_s,
    insn_fsgnjn_s,
    insn_fsgnjx_s,
    insn_fmin_s,
    insn_fmax_s,
    insn_fcvt_w_s,
    insn_fcvt_wu_s,
    insn_fmv_x_w,
    insn_feq_s,
    insn_flt_s,
    insn_fle_s,
    insn_fclass_s,
    insn_fcvt_s_w,
    insn_fcvt_s_wu,
    insn_fmv_w_x,
    insn_fcvt_l_s,
    insn_fcvt_lu_s,
    insn_fcvt_s_l,
    insn_fcvt_s_lu,
    insn_fld,
    insn_fsd,
    insn_fmadd_d,
    insn_fmsub_d,
    insn_fnmsub_d,
    insn_fnmadd_d,
    insn_fadd_d,
    insn_fsub_d,
    insn_fmul_d,
    insn_fdiv_d,
    insn_fsqrt_d,
    insn_fsgnj_d,
    insn_fsgnjn_d,
    insn_fsgnjx_d,
    insn_fmin_d,
    insn_fmax_d,
    insn_fcvt_s_d,
    insn_fcvt_d_s,
    insn_feq_d,
    insn_flt_d,
    insn_fle_d,
    insn_fclass_d,
    insn_fcvt_w_d,
    insn_fcvt_wu_d,
    insn_fcvt_d_w,
    insn_fcvt_d_wu,
    insn_fcvt_l_d,
    insn_fcvt_lu_d,
    insn_fmv_x_d,
    insn_fcvt_d_l,
    insn_fcvt_d_lu,
    insn_fmv_d_x,
    nums_insns,
};  // all implemented instructions

typedef struct {
    i8 rd;
    i8 rs1;
    i8 rs2;
    i8 rs3;                 // 扩展时可能用到
    i32 imm;
    i16 csr;                // 状态控制指令
    enum insn_type_t type;  // 指令类型
    BOOL rvc;               // 是否为rvc 指令
    BOOL continu;           // 是否继续执行
} insn_t;

/**
 * mmu.c
 */
typedef struct {
    u64 entry;
    u64 host_alloc;
    u64 base;         // guest 内存的基地址
    u64 guest_alloc;  // 后续guest使用的内存堆顶
} mmu_t;
void mmu_load_elf(mmu_t *, int);

/**
 * state.c
 */
enum exit_reason_t {
    NONE,
    DIRECT_JMP,
    INDIRECT_JMP,
    ECALL,
};

typedef struct {
    enum exit_reason_t exit_reason;  // 跳出原因
    u64 gp_regs[32];                 // 寄存器位置
    u64 pc;                          // 程序执行的位置
} state_t;

/**
 * machine.c
 */
typedef struct {
    state_t state;
    mmu_t mmu;
} machine_t;

enum exit_reason_t machine_step(machine_t *);
void machine_load_program(machine_t *, char *);

void exec_block_interp(state_t *);

void insn_decode(insn_t *, u32);

#endif