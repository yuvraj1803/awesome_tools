/*
	Author: Yuvraj Sakshith <ysakshith@gmail.com>
	
*/

#include <stdio.h>
#include <string.h>

int hex2dec_validate_hex(char *hex) {
	if(strlen(hex) < 3 || hex[0] != '0' || hex[1] != 'x')
		return 0;
	
	for(int i = 2; i < (int) strlen(hex); i++) {
		if((hex[i] <= '9' && hex[i] >= '0') || (hex[i] <= 'f' && hex[i] >= 'a'))
			continue;
		return 0;
	}

	// Check if provided hex fits into the underlying architecture's register size.
	// Right now random hex sizes are not supported.

	// +2 because we need to count '0x' as well
	if(strlen(hex) > (8 + 2) && sizeof(void*) == 4) {
		printf("32-bit machine underlying. Please provide hex only as big as 32-bits\n");
		return 0;
	}	

	if(strlen(hex) > (16 + 2) && sizeof(void*) == 8) {
		printf("64-bit machine underlying. Please provide hex only as big as 64-bits\n");
		return 0;
	}

	return 1;
}

int main(int argc, char *argv[]) {
	
	if(argc != 2 || !hex2dec_validate_hex(argv[1])) {
		printf("Usage: hex2dec <hexadecimal>\n Eg: hex2dec 0x4000\n");
		goto exit;
	}

	unsigned long long dec = 0;
	unsigned long long multiplier = 1;

	for(int i = strlen(argv[1]) - 1; i >= 2; i--) {
		if (argv[1][i] <= '9' && argv[1][i] >= '0') {
			dec += (argv[1][i] - '0') * multiplier;
		} else {
			dec += ((argv[1][i] - 'a') + 10) * multiplier;
		}
		multiplier <<= 4;
	}

	printf("%llu\n", dec);
exit:
	return 0;
}
