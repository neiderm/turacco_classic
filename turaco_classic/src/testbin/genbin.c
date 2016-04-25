// genbin.c

//
//  generates the test binary file.
//
//  December 3, 1998
//  jerry@mail.csh.rit.edu

#include <stdio.h>
#include <string.h>

int main(void)
{
    FILE *fp;
    char buf[512];
    int count;

    fp = fopen("test.bin", "wb");
    if (fp)
    {
        for(count=0; count <256 ; count++)
	{
	    memset(buf, count, 10);
	    fwrite (buf, 1, 8, fp);
	}

	memset(buf, 0x00, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x01, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x02, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x04, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x08, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x10, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x20, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x40, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x80, 10); fwrite (buf, 1, 8, fp);
	memset(buf, 0x00, 10); fwrite (buf, 1, 8, fp);

        for(count=0; count <=256 ; count++)
	    buf[count] = count;
	fwrite (buf, 1, 256, fp);

	fclose(fp);
	printf("Created test.bin\n");
    } else {
	printf("Can't open file\n");
    }
}
