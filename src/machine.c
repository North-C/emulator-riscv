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

void machine_setup(machine_t* m, int argc, char* argv[]) {
    // alloc space for stack
    size_t stacksz = 32 * 1024 * 1024;  // 32 MB
    u64 stack = mmu_alloc(&m->mmu, stacksz);
    m->state.gp_regs[sp] = stack + stacksz;

    m->state.gp_regs[sp] -= 8;  // auxv
    m->state.gp_regs[sp] -= 8;  // envp
    m->state.gp_regs[sp] -= 8;  // argv end

    u64 args = argc - 1;
    for (int i = args; i > 0; i--) {
        size_t len = strlen(argv[i]);
        u64 addr = mmu_alloc(&m->mmu, len + 1);  // 保存 '\0'
        mmu_write(addr, (u8*)argv[i], len);      // program name
        m->state.gp_regs[sp] -= 8;               // argv[i]
        mmu_write(m->state.gp_regs[sp], (u8*)&addr, sizeof(u64));
    }

    m->state.gp_regs[sp] -= 8;  // argc
    mmu_write(m->state.gp_regs[sp], (u8*)&args, sizeof(u64));
}
