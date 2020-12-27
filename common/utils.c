#include <stdlib.h>

#include "utils.h"

char *
read_file(const char *file_name, int64_t *out_size)
{
	char *buffer, *ret = NULL;
	int64_t file_size;
	FILE *file = fopen(file_name, "r");
	if (!file) {
		pprint_error("Failed to open '%s' file\n", file_name);
		goto return_nullptr;
	}

	if (fseek(file, 0, SEEK_END)) {
		pprint_error("Fseek error while processing '%s' file\n", file_name);
		goto close_file;
	}

	file_size = ftell(file);
	if (file_size == -1L) {
		pprint_error("Ftell error while processing '%s' file\n", file_name);
		goto close_file;
	}

	rewind(file);

	buffer = malloc(sizeof(char *) * get_alignment((file_size + 1), 4));
	if (!buffer) {
		pprint_error("Failed to allocate %ld bytes to file buffer\n", file_size + 1);
		goto close_file;
	}

	if (fread(buffer, 1, file_size, file) < file_size) {
		pprint_error("Failed to read the file %s\n", file_name);
		free(buffer);
		goto close_file;
	}
	buffer[file_size] = '\0';

	ret = buffer;
	*out_size = file_size;

close_file:
	fclose(file);
return_nullptr:
	return ret;
}
