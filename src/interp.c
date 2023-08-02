#include "emulator.h"

typedef void(func_t)(state_t *, insn_t *);

static void func_empty(state_t *state, insn_t *insn) {}

static func_t *funcs[] = {
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty, func_empty, func_empty, func_empty, func_empty, func_empty,
    func_empty,
};

void exec_block_interp(state_t *state) {
    static insn_t insn = {0};

    while (TRUE) {
        // 取码
        u32 data = *(u32 *)TO_HOST(state->pc);
        // 译码
        insn_decode(&insn, data);
        // 执行
        funcs[insn.type](state, &insn);
        state->gp_regs[zero] = 0;  // 每次执行后将zero置为0

        if (insn.continu) break;

        state->pc += insn.rvc ? 2 : 4;
    }
}
