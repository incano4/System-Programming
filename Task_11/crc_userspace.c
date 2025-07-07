#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define CRC_MAX_LEN	256
#define CRC_IOCTL_SET	_IOW('d', 0, char[CRC_MAX_LEN])
#define CRC_IOCTL_GET	_IOR('d', 1, uint32_t)

int main(int argc, char *argv[]) {
    int fd;
    int ret;

    if (argc < 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return -1;
    }

    fd = open("/dev/crc", O_RDWR);
    if (fd < 0) {
        printf("Failed to open device");
        return -1;
    }

    ret = ioctl(fd, CRC_IOCTL_SET, argv[1]);
    if (ret < 0) {
        printf("CRC_IOCTL_SET failed\n");
        close(fd);
        return -1;
    }

    uint32_t crc = 0;
    ret = ioctl(fd, CRC_IOCTL_GET, &crc);
    if (ret < 0) {
        printf("CRC_IOCTL_GET failed\n");
        close(fd);
        return -1;
    }

    printf("crc32 for <%s> is %x\n", argv[1], crc);

    close(fd);
    return 0;
}