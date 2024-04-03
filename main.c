#include "emulator.h"

int main(int argc, char* argv[]) {
    struct Emulator emulator;
    init_emulator(&emulator, argc, argv);
    run_emulator(&emulator);
    free_emulator(&emulator);
    return 0;
}
