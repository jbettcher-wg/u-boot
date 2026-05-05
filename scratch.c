#include <stdio.h>
#include <stdint.h>

#define CONFIG_SYS_FSL_CORES_PER_CLUSTER 4
#define CONFIG_SYS_FSL_THREADS_PER_CORE 2

uint32_t rlwinm(uint32_t val, uint32_t shift, uint32_t mask) {
    uint32_t rotated = (val << shift) | (val >> (32 - shift));
    return rotated & mask;
}

int main() {
    for (int pir = 0; pir < 32; pir++) {
        uint32_t r0 = pir;
        
        uint32_t r8 = rlwinm(r0, 29, 0x03);
        uint32_t r10 = r0 >> 5;
        uint32_t r11 = rlwinm(r0, 0, 0x07); // 29, 31 means bits 29 to 31. The mask is 0x07!
        // Wait, rlwinm r11, r0, 0, 29, 31.
        // SH=0. MB=29, ME=31. Bits 29 to 31 is mask 0x07.
        
        uint32_t r5 = (r10 * CONFIG_SYS_FSL_CORES_PER_CLUSTER) + r8;
        uint32_t r4 = (r5 * CONFIG_SYS_FSL_THREADS_PER_CORE) + r11;
        
        printf("PIR=%d: r8(core)=%d, r10(cluster)=%d, r11(thread)=%d -> index r5=%d\n", 
               pir, r8, r10, r11, r5);
    }
    return 0;
}
