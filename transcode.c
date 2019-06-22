#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>

int UTF8toUnicode(char *input, char *output)
{
	// char inbuf[BUFSIZ] = {0};
	// char outbuf[BUFSIZ] = {0};
	size_t insize = 0;
	char *wrptr = (char *)output;
	int result = 0;
	iconv_t cd;

	//const char * car_no = "12KO5";
	//sprintf(inbuf, "[n1][v1][s3][z1]%s%s", "冀A", car_no);

	printf("[UTF8toUnicode] %s\n", input);

	cd = iconv_open ("UNICODELITTLE", "UTF-8");
	if (cd == (iconv_t) -1)
    {
      /* Something went wrong.  */
      if (errno == EINVAL)
        error (0, 0, "conversion from '%s' to wchar_t not available",
               "UTF-8");
      else
        perror ("iconv_open");

      /* Terminate the output string.  */
      *output = L'\0';

      return -1;
    } else {
		
		printf("iconv_open ok\n");
	}
	
	//printf("inbuf[%s]\n", inbuf);
	
	int avail = 1;
	while (avail > 0)
    {
      //size_t nread;
      size_t nconv;
      char *inptr = (char *)input;

      insize = strlen(inptr);
	  avail = 2*insize;
	
      /* Do the conversion.  */
	  printf("insize = [%d], avail = [%d]\n", insize, avail);
      nconv = iconv (cd, &inptr, &insize, &wrptr, &avail);
	  printf("nconv = [%d], insize = [%d], avail = [%d]\n", nconv, insize, avail);
      if (nconv == (size_t) -1)
        {
          /* Not everything went right.  It might only be
             an unfinished byte sequence at the end of the
             buffer.  Or it is a real problem.  */
			 printf("%s\n", strerror(errno));
          if (errno == EINVAL)
            /* This is harmless.  Simply move the unused
               bytes to the beginning of the buffer so that
               they can be used in the next round.  */
            memmove (input, inptr, insize);
          else
            {
              /* It is a real problem.  Maybe we ran out of
                 space in the output buffer or we have invalid
                 input.  In any case back the file pointer to
                 the position of the last processed byte.  */
              //lseek (fd, -insize, SEEK_CUR);
              result = -1;
              break;
            }
        }
		break;
    }

	return result;
	
	
}