#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


inline unsigned int get_signature(const unsigned char *buffer, size_t size) __attribute__((always_inline));
unsigned int compute_signature(const unsigned char *buffer, size_t size)
{
    size_t n;
    
    unsigned int signature = 0;
    for (n = 0; n < size; n++)
    {
        //printf("%02X, %02X\n", mem[n], disk[n]);

        signature += buffer[n];
    }

    return signature;
}



unsigned int extract_signature(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if ( fd == -1)
    {
        printf("error reading file: %s\n", filename);
        return 0; // error reading file. to unknown status
    }

    size_t size = lseek(fd, 0, SEEK_END);
    if ( size < sizeof(int)) 
    {
        close(fd);
            return 0;
    }

    unsigned char buffer[sizeof(int)] = {0};
    lseek(fd, size - sizeof(int), SEEK_SET);


    //read(fd, buf, 4);
    unsigned int signature = 0;
    if (read(fd, &signature, sizeof(unsigned int)) != sizeof(unsigned int))
    {
        close(fd);
        return 0;
    }

    close(fd);
    printf("extracted signature = %d\n", signature);
    return signature;
}


void  append_signature(const char *filename, unsigned int signature)
{
    int nfd = open(filename, O_WRONLY);
    lseek(nfd, 0, SEEK_END);

    write(nfd, (void*) &signature, sizeof(signature));

    close(nfd);

    printf("written  signature = %d\n", signature);

    // check
    extract_signature(filename);
}
