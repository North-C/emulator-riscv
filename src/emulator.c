#include "emulator.h"

#include <stdio.h>

int main(int argc, char** argv) {
    assert(argc > 1);
    machine_t machine = {0};
    machine_load_program(&machine, argv[1]);

    while (TRUE) {
        enum exit_reason_t reason = machine_step(&machine);
        assert(reason == ECALL);
    }
    return 0;
}