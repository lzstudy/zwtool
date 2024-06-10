#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
 
int main() {
    // 物理地址
    unsigned long phys_addr = 0x50000000;
    // 映射的大小
    size_t size = 1024 * 1024; // 1024 KB
    // 获取文件描述符
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    // 定位到特定的物理地址
    if (lseek(fd, phys_addr, SEEK_SET) == -1) {
        perror("lseek");
        return -1;
    }
    // 映射物理内存
    void *virt_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phys_addr);
    if (virt_addr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
 
    uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    // 使用映射后的地址
    printf("Memory mapped to %p\n", virt_addr);
    memcpy(virt_addr, data, sizeof(data));
 
    // 清理映射
    if (munmap(virt_addr, size) == -1) {
        perror("munmap");
        return -1;
    }
 
    close(fd);
    return 0;
}
