#include "global.h"

s32 __osSiRawWriteIo(uintptr_t devAddr, u32 data) {
    if (__osSiDeviceBusy() != 0) {
        return -1;
    }

    *(u32*)(devAddr | 0xA0000000) = data;

    return 0;
}
