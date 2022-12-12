// Programmer for ARM over UART




#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/serial.h>
#include <unistd.h>


#undef USE_USB
#define IS_NATIVE

#include "linux.h"



//#define USE_USB
#ifdef USE_USB
#include "libusb.h"
#endif

#define BAUD B115200


// Pin masks
// Reset
#define CHIP_RESET_PIN (0x20)
// JTAG reset
#define JTAG_RESET_PIN (0x1)
// JTAG mode select
#define TMS_PIN (0x10)
// JTAG clock
#define TCK_PIN (0x8)
// JTAG data to chip
#define TDI_PIN (0x4)
// JTAG data from chip
#define TDO_PIN (0x2)


#define CAPTURE_FILE "terminal.cap"
#define VENDOR_ID 0x04d8
#define PRODUCT_ID 0x000d
#define PACKET_SIZE 64
#define RESET_DELAY 100
// Timeout in milliseconds
#define TIMEOUT 30000
#define RAM_SIZE 65536

#ifdef USE_USB
struct libusb_device_handle *devh;
unsigned char out_urb_data[PACKET_SIZE];
struct libusb_transfer *last_urb = 0;
#endif

FILE *capture = 0;



#ifdef USE_USB
static void out_callback(struct libusb_transfer *urb)
{
	last_urb = urb;
}
#endif





// Returns the FD of the serial port
static int init_serial(char *path, int baud)
{
	struct termios term;

	printf("init_serial %d: opening %s\n", __LINE__, path);
// Initialize serial port
	int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		printf("init_serial %d: path=%s: %s\n", __LINE__, path, strerror(errno));
		return -1;
	}
	
	if (tcgetattr(fd, &term))
	{
		printf("init_serial %d: path=%s %s\n", __LINE__, path, strerror(errno));
		close(fd);
		return -1;
	}


/*
 * printf("init_serial: %d path=%s iflag=0x%08x oflag=0x%08x cflag=0x%08x\n", 
 * __LINE__, 
 * path, 
 * term.c_iflag, 
 * term.c_oflag, 
 * term.c_cflag);
 */
	tcflush(fd, TCIOFLUSH);
	cfsetispeed(&term, baud);
	cfsetospeed(&term, baud);
//	term.c_iflag = IGNBRK;
	term.c_iflag = 0;
	term.c_oflag = 0;
	term.c_lflag = 0;
//	term.c_cflag &= ~(PARENB | PARODD | CRTSCTS | CSTOPB | CSIZE);
//	term.c_cflag |= CS8;
	term.c_cc[VTIME] = 1;
	term.c_cc[VMIN] = 1;
/*
 * printf("init_serial: %d path=%s iflag=0x%08x oflag=0x%08x cflag=0x%08x\n", 
 * __LINE__, 
 * path, 
 * term.c_iflag, 
 * term.c_oflag, 
 * term.c_cflag);
 */
	if(tcsetattr(fd, TCSANOW, &term))
	{
		printf("init_serial %d: path=%s %s\n", __LINE__, path, strerror(errno));
		close(fd);
		return -1;
	}
	printf("init_serial %d: opened %s\n", __LINE__, path);

	return fd;
}

void print_char(unsigned char c)
{
	fputc(c, capture);
	fflush(capture);
	
	printf("%c", c);
	fflush(stdout);
}

// Read a character
unsigned char read_char(int fd)
{
	unsigned char c;
	int result;
	do
	{
		result = read(fd, &c, 1);
	} while(result <= 0);
	return c;
}

// Send a character
void write_char(int fd, unsigned char c)
{
	int result;
	do
	{
		result = write(fd, &c, 1);
	} while(!result);
}

void write_buffer(int fd, unsigned char *data, int size)
{
	int result = 0;
	int bytes_sent = 0;
	do
	{
		bytes_sent = write(fd, data + result, size - result);
		if(bytes_sent > 0) result += bytes_sent;
	}while(result < size);
}



#ifdef USE_USB
static int init_usb()
{
	int result = 0;

	result = libusb_init(0);
	if(result < 0)
	{
		printf("init_usb: Couldn't initialize libusb\n");
		return 1;
	}

	devh = libusb_open_device_with_vid_pid(0, VENDOR_ID, PRODUCT_ID);
	if(!devh)
	{
		printf("init_usb: Couldn't find JTAG bit banger\n");
		return 1;
	}


	result = libusb_claim_interface(devh, 0);
	if(result < 0)
	{
		printf("init_usb: bit banger in use\n");
		return 1;
	}


 	return 0;
}

void close_usb()
{
	libusb_release_interface(devh, 0);
}


void write_usb(unsigned char lat, unsigned char tris)
{
	int result = 0;

	out_urb_data[0] = 1;
	out_urb_data[1] = 0;
	out_urb_data[2] = 0;
	out_urb_data[3] = tris;
	out_urb_data[4] = lat;
	
	struct libusb_transfer *out_urb = libusb_alloc_transfer(0);
	libusb_fill_bulk_transfer(out_urb,
		devh, 
		1 | LIBUSB_ENDPOINT_OUT,
		out_urb_data, 
		PACKET_SIZE, 
		out_callback,
		out_urb, 
		TIMEOUT);
	libusb_submit_transfer(out_urb);

//printf("write_usb %d\n", __LINE__);
	do
	{
// Output packet
		struct timeval tv;
		tv.tv_sec = TIMEOUT / 1000;
		tv.tv_usec = 0;
		result = libusb_handle_events_timeout(0, &tv);
//printf("write_usb %d %p %p\n", __LINE__, last_urb, out_urb);
	}while(last_urb != out_urb);
//printf("write_usb %d\n", __LINE__);

	libusb_free_transfer(out_urb);
}
#endif // USE_USB




void wait_uart(int serial_fd)
{
	char code[4] = { 0, 0, 0, 0 };
	while(1)
	{
		unsigned char c = read_char(serial_fd);
		code[0] = code[1];
		code[1] = code[2];
		code[2] = code[3];
		code[3] = c;


		print_char(c);
//printf("main %d: %c%c%c%c\n", __LINE__, code[0], code[1], code[2], code[3]);


		if(code[0] == 'd' &&
			code[1] == 'o' &&
			code[2] == 'n' &&
			code[3] == 'e')
		{
			return;
		}
	}
}


int64_t get_number(char *text)
{
	int64_t result;
	if(text[0] == '0' && 
		text[1] == 'x')
	{
		sscanf(text, "%lx", &result);
	}
	else
	{
		sscanf(text, "%ld", &result);
	}
	return result;
}



int main(int argc, char *argv[])
{
	int write_program = 0;
	int size = 0;
	unsigned char *data = 0;
	char *filename = 0;
	int start_address = PROGRAM_START;
	int i;

// Flash sectors
// 0: 0x00000000 (0x4000 16kB) not protected
// 1: 0x00004000 (0x4000 16kB) not protected
// 2: 0x00008000 (0x4000 16kB) not protected
// 3: 0x0000c000 (0x4000 16kB) not protected
// 4: 0x00010000 (0x10000 64kB) not protected
// 5: 0x00020000 (0x20000 128kB) not protected
// 6: 0x00040000 (0x20000 128kB) not protected
// 7: 0x00060000 (0x20000 128kB) not protected
// 8: 0x00080000 (0x20000 128kB) not protected
// 9: 0x000a0000 (0x20000 128kB) not protected
// 10: 0x000c0000 (0x20000 128kB) not protected
// 11: 0x000e0000 (0x20000 128kB) not protected

	int64_t flash_sectors[] =
	{
		0x8000000,
		0x8004000,
		0x8008000,
		0x800c000,
		0x8010000,
		0x8020000,
		0x8040000,
		0x8060000,
		0x8080000,
		0x80a0000,
		0x80c0000,
		0x80e0000
	};
	int total_sectors = sizeof(flash_sectors) / sizeof(unsigned int);

// Got a program to write
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i], "-s"))
			{
				if(i + 1 < argc)
				{
					start_address = get_number(argv[i + 1]);
					i++;
				}
				else
				{
					printf("main %d: -s needs the starting address\n", __LINE__);
					exit(1);
				}
			}
			else
			{
				filename = argv[i];
				FILE *fd = fopen(filename, "r");
				if(!fd)
				{
					printf("main %d: couldn't open %s\n", __LINE__, filename);
					return 1;
				}

				fseek(fd, 0, SEEK_END);
				size = ftell(fd);
				fseek(fd, 0, SEEK_SET);

				data = malloc(size);
				fread(data, size, 1, fd);
				fclose(fd);

				printf("main %d: writing %s address 0x%x size %d\n", 
					__LINE__,
					filename,
					start_address,
					size);

				write_program = 1;
			}
		}
		
	}



	int serial_fd = init_serial("/dev/ttyUSB0", BAUD);
	if(serial_fd < 0) serial_fd = init_serial("/dev/ttyUSB1", BAUD);
	if(serial_fd < 0) serial_fd = init_serial("/dev/ttyUSB2", BAUD);
	if(serial_fd < 0) return 1;

#ifdef USE_USB
	int usb_status = init_usb();
#endif

	capture = fopen(CAPTURE_FILE, "w");
	if(!capture) 
	{
		printf("Couldn't open capture file %s\n", CAPTURE_FILE);
		return 1;
	}

// Reset chip
#ifdef USE_USB
	if(!usb_status)
	{
		printf("main %d: resetting chip\n", __LINE__);
		write_usb(~CHIP_RESET_PIN, ~CHIP_RESET_PIN);
		usleep(RESET_DELAY * 1000);
		write_usb(0xff, 0xff);
		close_usb();
	}
#endif

	if(write_program)
	{
		char code[4] = { 0, 0, 0, 0 };
		while(1)
		{
			unsigned char c = read_char(serial_fd);
			code[0] = code[1];
			code[1] = code[2];
			code[2] = code[3];
			code[3] = c;



			print_char(c);
	//printf("main %d: %c%c%c%c\n", __LINE__, code[0], code[1], code[2], code[3]);


			if(code[0] == 'S' &&
				code[1] == 'Y' &&
				code[2] == 'S' &&
				code[3] == 'C')
			{
	// Send code to activate bootloader
					write_char(serial_fd, 'X');
					usleep(100000);
					write_char(serial_fd, 'Y');
					usleep(100000);
					write_char(serial_fd, 'Z');
					usleep(100000);
	//printf("main %d\n", __LINE__);
					break;
			}

		}

		wait_uart(serial_fd);

//printf("main %d\n", __LINE__);
//printf("main %d\n", __LINE__);
		
// Command to erase flash
		for(i = 4; i < total_sectors; i++)
		{
			if(flash_sectors[i] >= start_address &&
				flash_sectors[i] < start_address + size)
			{
//printf("main %d\n", __LINE__);
				write_char(serial_fd, 'E');
				write_char(serial_fd, i);
				
//printf("main %d\n", __LINE__);
				wait_uart(serial_fd);
//printf("main %d\n", __LINE__);
			}
		}


		for(i = 0; i < size; i += RAM_SIZE)
		{
			int segment = RAM_SIZE;
			if(i + segment > size)
			{
				segment = size - i;
			}
			
			write_char(serial_fd, 'W');
			int address = start_address + i;
			write_buffer(serial_fd, (unsigned char*)&address, 4);
			write_buffer(serial_fd, (unsigned char*)&segment, 4);

//printf("main %d %d\n", __LINE__, segment);
// Wait for malloc printf
			sleep(1);
			write_buffer(serial_fd, data + i, segment);
//printf("main %d\n", __LINE__);
			wait_uart(serial_fd);
//printf("main %d\n", __LINE__);
		}
		
		write_char(serial_fd, 'B');
	}



// go into terminal mode
	struct termios info;
	tcgetattr(fileno(stdin), &info);
	info.c_lflag &= ~ICANON;
	info.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &info);

	unsigned char test_buffer[32];
	fd_set rfds;
	while(1)
	{
		FD_SET(serial_fd, &rfds);
		FD_SET(0, &rfds);
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		int result = select(serial_fd + 1, 
			&rfds, 
			0, 
			0, 
			&timeout);

		if(FD_ISSET(serial_fd, &rfds))
		{

			unsigned char c = read_char(serial_fd);
			print_char(c);
		}

// send input from console
		if(FD_ISSET(0, &rfds))
		{
			int i;
			int bytes = read(0, test_buffer, sizeof(test_buffer));
			
			for(i = 0; i < bytes; i++)
			{
				char c = test_buffer[i];
				if(c < 0xa)
					printf("0x%02x ", c);
				else
					printf("%c", c);

				fflush(stdout);

				if(c == 0xa)
				{
					write_char(serial_fd, 0xd);
// delay to avoid overflowing a 9600 passthrough
					usleep(10000);
					write_char(serial_fd, 0xa);
				}
				else
					write_char(serial_fd, c);


// delay to avoid overflowing a 9600 passthrough
				usleep(10000);
			}
		}
	}

}













