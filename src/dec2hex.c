#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int dec2hex_validate_dec(char *dec) {
		
	for(int i = 0; i < (int) strlen(dec); i++) {
		if(dec[i] <= '9' && dec[i] >= '0')
			continue;
		return 0;
	}

	return 1;
}

int main(int argc, char *argv[]) {
	
	if(argc != 2 || !dec2hex_validate_dec(argv[1])) {
		printf("Usage: dec2hex <decimal>\n Eg: dec2hex 4096\n");
		return 0;
	}
	
	printf("Warning: We don't check for overflows.\n");

	unsigned long long dec = atoi(argv[1]);

	printf("0x%llx\n", dec);
	
	return 0;
}
