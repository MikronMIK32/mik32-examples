#include <mcu_utils.h>

static uint32_t miState;
int32_t rand() {
	miState ^= (miState << 13);
	miState ^= (miState >> 17);
	miState ^= (miState << 15);

	return (miState * 1332534557) & 0x7FFFFFFF;
}

void srand(uint32_t seed) {
// a zero seed will not work!
	if (seed == 0) {
		seed = 0x55aaff00;
	}

	miState = seed;
}
