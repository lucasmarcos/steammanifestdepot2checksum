#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	FILE *f;
	uint32_t magic, total_length;
	uint8_t *manifest;

	f = fopen(argv[1], "rb");

	while (fread(&magic, sizeof(uint32_t), 1, f)) {
		if (magic == 1911953360) {
			fread(&total_length, sizeof(uint32_t), 1, f);

			manifest = (uint8_t *) malloc(total_length * sizeof(uint8_t));
			fread(manifest, sizeof(uint8_t), total_length, f);
			fclose(f);

			f = fopen("out", "wb");
			fwrite(manifest, sizeof(uint8_t), total_length, f);
			fclose(f);

			free(manifest);

			break;
		}
	}

	return 0;
}
