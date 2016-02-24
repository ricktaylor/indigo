
#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main( int argc, char* argv[])
{
    unsigned char *buf;
    char* ident;
    unsigned int param, i, file_size;
    unsigned int text_mode = 0;

    FILE *f_input, *f_output;

    if (argc < 4 || argc > 5) {
        fprintf(stderr, "Usage: %s binary_file output_file identifier\n", argv[0]);
        return -1;
    }

    param = 1;
    if (argv[1][0] == '-')
    {
    	if (strcmp(argv[1],"-t") == 0)
    		text_mode = 1;
    	else
    	{
    		fprintf(stderr, "Usage: %s [-t] binary_file output_file identifier\n", argv[0]);
    		return -1;
    	}

    	++param;
    }

    f_input = fopen(argv[param], "rb");
    if (f_input == NULL) {
        fprintf(stderr, "%s: can't open %s for reading\n", argv[0], argv[param]);
        return -1;
    }

    // Get the file length
    fseek(f_input, 0, SEEK_END);
    file_size = ftell(f_input);
    fseek(f_input, 0, SEEK_SET);

    buf = (unsigned char *)malloc(file_size);
    assert(buf);

    fread(buf, file_size, 1, f_input);
    fclose(f_input);

    f_output = fopen(argv[param+1], "w");
    if (f_output == NULL)
    {
        fprintf(stderr, "%s: can't open %s for writing\n", argv[0], argv[param+1]);
        return -1;
    }

    ident = argv[param+2];
    char* c = ident;
    for (; *c != '\0';++c)
    {
    	if (*c == '-')
    		*c = '_';
    }

    if (text_mode)
    {
    	fprintf (f_output, "const char s_%s[%i] = \n\t\"", ident, file_size);
		unsigned int need_comma = 0;
		for (i = 0; i < file_size; ++i)
		{
			if (buf[i] == '\n')
			{
				fprintf(f_output, "\\n\"");
				for (; buf[i] == '\n' && i < file_size; ++i)
					fprintf(f_output, "\t\n");

				fprintf(f_output, "\t");
				for (; buf[i] == '\t' && i < file_size; ++i)
					fprintf(f_output, "\t");

				fprintf(f_output, "\"");
				--i;
			}
			else if (buf[i] == '\t')
				fprintf(f_output, "\t");
			else if (buf[i] == '\"')
				fprintf(f_output, "\\\"");
			else if (buf[i] < 32 || buf[i] >= 127)
				fprintf(f_output, "\\x%.2x", buf[i]);
			else
				fprintf(f_output, "%c", buf[i]);
		}
		fprintf(f_output, "\";\n");
    }
    else
    {
    	fprintf (f_output, "const unsigned char s_%s[%i] = \n{", ident, file_size);
    	unsigned int need_comma = 0;
		for (i = 0; i < file_size; ++i)
		{
			if (need_comma) fprintf(f_output, ",");
			else need_comma = 1;
			if (( i % 16 ) == 0) fprintf(f_output, "\n\t");
			fprintf(f_output, "0x%.2x", buf[i]);
		}
		fprintf(f_output, "\n};\n");
    }

    fclose(f_output);

    return 0;
}
