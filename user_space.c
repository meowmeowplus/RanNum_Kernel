#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main()
{
    int randomNumber_from_kernel;
    int fd = open("/dev/randomNumber", O_RDONLY);
    
    if (fd < 0){
        perror("failed to open module");
        return EXIT_FAILURE;
    }

    read(fd, &randomNumber_from_kernel, sizeof(int));

    printf("Your random number is: %d\n", randomNumber_from_kernel);
    
    return EXIT_SUCCESS;
}