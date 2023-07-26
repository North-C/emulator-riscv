#include "emulator.h"
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