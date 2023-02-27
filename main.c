#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint64_t readlength(uint8_t *f, uint32_t *offset)
{
	uint64_t length = 0;
	uint64_t shift = 0;
	uint8_t value;

	do {
		value = f[*offset];
		(*offset)++;
		length |= (value & 0x7f) << shift;
		shift += 7;
	} while (value >> 7);

	return length;
}

void handledata(uint8_t *data, uint32_t len)
{
	uint32_t i = 0, j;

	uint8_t field;
	uint8_t type;
	uint32_t length;

	uint8_t *fileName = NULL;
	uint8_t *hash = NULL;
	uint8_t dir = 0;

	while (i < len) {
		field = data[i] >> 3;
		type = data[i] & 0x07;
		i++;

		length = readlength(data, &i);

		// printf("type %d field %d value %d\n", type, field, length);

		if (type == 2) {
			if (field == 1) {
				fileName = malloc(length * sizeof(uint8_t) + 1);
				memcpy(fileName, &data[i], length * sizeof(uint8_t));
				fileName[length] = '\0';
			} else if (field == 5) {
				hash = malloc(length * sizeof(uint8_t));
				memcpy(hash, &data[i], length * sizeof(uint8_t));
			}

			i += length;
		} else {
			if (field == 2) {
				dir = !length;
			}
		}
	}

	if (fileName && hash && !dir) {
		for (j = 0; j < 20; j++) {
			printf("%.2X", hash[j]);
		}

		printf(" *%s\n", fileName);
	}

	if (fileName) {
		free(fileName);
		fileName = NULL;
	}
	if (hash) {
		free(hash);
		hash = NULL;
	}
}


int main(int argc, char *argv[])
{
	FILE *f;
	uint32_t magic, total_length, length;
	uint8_t *manifest, *data;
	uint8_t field, type;

	f = fopen(argv[1], "rb");

	fread(&magic, sizeof(uint32_t), 1, f);
	fread(&total_length, sizeof(uint32_t), 1, f);

	manifest = (uint8_t *) malloc(total_length * sizeof(uint8_t));
	fread(manifest, sizeof(uint8_t), total_length, f);
	fclose(f);

	uint32_t i = 0;
	while (i < total_length) {
		field = manifest[i] >> 3;
		type = manifest[i] & 0x07;
		i++;
		length = readlength(manifest, &i);

		if (type != 2 || field != 1) break;

		data = malloc(length * sizeof(uint8_t));
		memcpy(data, &manifest[i], length * sizeof(uint8_t));
		handledata(data, length);
		free(data);

		i += length;
	}

	free(manifest);

	return 0;
}
