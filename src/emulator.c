#include "emulator.h"

#include <stdio.h>

int main(int argc, char** argv) {
    assert(argc > 1);
    machine_t machine = {0};
    machine_load_program(&machine, argv[1]);

    printf("entry: 0x%llx\n", TO_HOST(machine.mmu.entry));
    printf("host_alloc: 0x%lx\n", machine.mmu.host_alloc);
    return 0;
}