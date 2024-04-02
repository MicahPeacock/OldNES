#include "emulator.h"

int main(int argc, char* argv[]) {
    struct Emulator emulator;
    init_emulator(&emulator, argc, argv);
    free_emulator(&emulator);
    return 0;
}
