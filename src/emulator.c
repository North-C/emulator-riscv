#include "emulator.h"

#include <stdio.h>

int main(int argc, char** argv) {
    assert(argc > 1);
    machine_t machine = {0};

    machine_load_program(&machine, argv[1]);
    machine_setup(&machine, argc, argv);  // BUG: segmentation fault

    while (TRUE) {
        enum exit_reason_t reason = machine_step(&machine);
        assert(reason == ECALL);

        u64 syscall = machine_get_gp_reg(&machine, a7);
        u64 ret = do_syscall(&machine, syscall);
        machine_set_gp_reg(&machine, a0, ret);
    }
    return 0;
}