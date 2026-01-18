#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define GPIO_CLK 45
#define GPIO_DAT 44
#define SYSFS_GPIO_DIR "/sys/class/gpio"

int sysfs_write_file(char *path, char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) 
    {
        printf("Error opening %s\n", path);
        return -1;
    }
    write(fd, value, strlen(value));
    close(fd);
    return 0;
}

void gpio_export(int gpio) {
    char buffer[50];
    char path[50];
    
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    if (access(path, F_OK) != -1) return; 

    int fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    sprintf(buffer, "%d", gpio);
    write(fd, buffer, strlen(buffer));
    close(fd);
}

void gpio_direction(int gpio, char *dir) {
    char path[50];
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
    sysfs_write_file(path, dir);
}

void gpio_write(int gpio, int value) {
    char path[50];
    sprintf(path, SYSFS_GPIO_DIR "/gpio%d/value", gpio);
    
    if (value) 
        sysfs_write_file(path, "1");
    else 
        sysfs_write_file(path, "0");
}

unsigned char gray(unsigned char byyte) {
    pid_t graybit[8]; 
    int status;
    unsigned char result = 0;

    for (int i = 0; i < 8; i++) {
        graybit[i] = fork();
        
        if (graybit[i] == 0) {
            int current_bit = (byyte >> i) & 1;
            int calculated_bit;
            
            if (i == 7) {
                calculated_bit = current_bit;
            } else {
                int left_bit = (byyte >> (i + 1)) & 1;
                calculated_bit = current_bit ^ left_bit;
            }

            exit(calculated_bit);
        }
    }

    for (int i = 0; i < 8; i++) {
        waitpid(graybit[i], &status, 0);
        if (WIFEXITED(status)) {
            result |= (WEXITSTATUS(status) << i);
        }
    }
    return result;
}

void driver_send_byte(unsigned char data) {
    for (int i = 7; i >= 0; i--) {
        gpio_write(GPIO_DAT, (data >> i) & 1);
        
        gpio_write(GPIO_CLK, 1);
        usleep(1000);
        gpio_write(GPIO_CLK, 0);
        usleep(1000);
    }
    gpio_write(GPIO_DAT, 0);
}

int main() {
    gpio_export(GPIO_CLK);
    gpio_export(GPIO_DAT);
    
    gpio_direction(GPIO_CLK, "out");
    gpio_direction(GPIO_DAT, "out");

    gpio_write(GPIO_CLK, 0);

    unsigned char signal_input = 0xF3; 
    
    unsigned char gray_code = gray(signal_input);
    
    printf("Transmitting Signal: 0x%X\n", gray_code);
    driver_send_byte(gray_code);
   
    return 0;
}
