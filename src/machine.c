#include "emulator.h"

enum exit_reason_t machine_step(machine_t* m) {
    while (TRUE) {
        m->state.exit_reason = NONE;
        exec_block_interp(&m->state);
        assert(m->state.exit_reason != NONE);

        if (m->state.exit_reason == INDIRECT_JMP ||
            m->state.exit_reason == DIRECT_JMP) {
            m->state.pc = m->state.reenter_pc;
            continue;  // reservation for JIT
        }
        break;
    }

    m->state.pc = m->state.reenter_pc;
    assert(m->state.exit_reason == ECALL);
    return ECALL;
}

/**
 * 加载可执行程序
 */
void machine_load_program(machine_t* m, char* prog) {
    int fd = open(prog, O_RDONLY);
    if (fd == -1) {
        Fatal(strerror(errno));
    }

    mmu_load_elf(&m->mmu, fd);
    close(fd);

    m->state.pc = (u64)m->mmu.entry;
}