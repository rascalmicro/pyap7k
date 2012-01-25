
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>

static const char *device = "/dev/i2c-0";
static const char *output = "Hello World???";
static char output_flag = 0;
static char target = 0x13;
static char read_count = 16;

void print_usage(const char *prog)
{
   printf("Usage: %s [-Dtwr]\n", prog);
   puts("  -D --device   device to use (default /dev/i2c-0)\n"
        "  -t --target   Slave Adress (default 0x13)\n"
        "  -w --write    Data to transfer <'string'>\n"
        "  -r --read     Read from Slave <count> (default 16)\n");
   exit(1);
}

void parse_opts(int argc, char *argv[])
{
   while (1) {
      static const struct option lopts[] = {
         { "device",  1, 0, 'D' },
         { "target",  1, 0, 't' },
         { "write",   1, 0, 'w' },
	 { "read",    1, 0, 'r' },
         { NULL, 0, 0, 0 },
      };
      int c;

      c = getopt_long(argc, argv, "D:t:w:r", lopts, NULL);

      if (c == -1)
         break;

      switch (c) {
      case 'D':
         device = optarg;
         break;
      case 't':
         target = atoi(optarg);
         break;
      case 'w':
         output = optarg;
	 output_flag = 1;
	 break;
      case 'r':
         read_count   = atoi(optarg);
         break;
      default:
         print_usage(argv[0]);
         break;
      }
   }
}

int main (int argc, char *argv[]) {
    int device_file;
    unsigned char buffer[128] = {0};
    int i, j;

    parse_opts(argc, argv);

    if ((device_file = open(device,O_RDWR)) < 0) {
        perror("I2C open");
        return -1;
    }

    if (ioctl(device_file,I2C_SLAVE,target) < 0) {
        perror("ioctl I2C_SLAVE");
        return -1;
    }

    if (output_flag == 1)
      if (write(device_file,output,strlen(output)) != strlen(output)) {
        perror("write");
        return -1;
      }

    if (read_count)
    {
      if (read(device_file, buffer, read_count) != read_count) {
        perror("read");
        return -1;
      } else {
      // Write out received data
        for (i=0; i<80; i++) {
            if (!(i % 8))
                printf("\n%02x:", i);
            printf(" %02x ", buffer[i]);

            if (!((i+1) % 8)  ) {
                printf(" | ");
                for (j=0;j<8;j++) {
                    if (isprint( (int)buffer[i-7+j]) )
                        printf( "%c", buffer[i-7+j] );
                    else
                        printf(".");
                }
            }
        }
        printf("\n");
      }
    }
} 

