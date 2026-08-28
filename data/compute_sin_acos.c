/* 
 * Creates a LUT for sin(arccos(x)) used to draw circles.
 * 
 * Compile with:
 *   gcc -o compute_sin_acos compute_sin_acos.c -lm
 *
 * License: The Fuck AI License, which is WTFPL except training AI is not allowed.
 * 
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


void main(int argc, char **argv)
{
    const char *dstname = "luts/sin_acos.cpp";
    if (argc > 1) {
        dstname = argv[1];
    }
    FILE *dstfile = fopen(dstname, "w");
    if (!dstfile) {
        fprintf(stderr, "cannot open %s\n", dstname);
        exit(1);
    }
    
    fprintf(dstfile, "#include <stdint.h>\n\n");
    fprintf(dstfile, "int32_t DATA_SIN_ACOS[1024] = {\n");
    for (int i=0; i<1024; ++i) {
        int32_t v = sin(acos((float)i / 1024)) * 0x10000L;

        fprintf(dstfile, "%d,", v);
        if (i > 0 && (i+1) % 16 == 0) {
            fprintf(dstfile, "\n");
        }
    }
    fprintf(dstfile, "\n};\n");
    fprintf(dstfile, "\nint DATA_SIN_ACOS_LEN = 1024;\n");
    fclose(dstfile);
}
