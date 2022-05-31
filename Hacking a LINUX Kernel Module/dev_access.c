/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program to access some character device files.
 */

/* Include Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

/* Function to open /dev/input/mouse0 file and read from it byte by byte*/
void read_from_dev_input_mouse0() {
	int read_fd, byteread;
	unsigned int read_buffer;
	
	// Open file
	read_fd = open("/dev/input/mouse0", O_RDONLY);
	// File open error
	if(read_fd == -1) {
		fprintf(stderr, "Error opening /dev/input/mouse0 file: %s\n", strerror(errno));
		exit(-1);
	}
	
	// Infinite loop to read 1 byte at a time
	while(1) {
		byteread = read(read_fd, &read_buffer, 1);
		if(byteread < 0) {
			fprintf(stderr, "Error reading from /dev/input/mouse0 file: %s\n", strerror(errno));
		break;
		}
		printf("Byte Read: %d\n", read_buffer);
	}
	// Close file
	close(read_fd);
}

/* Function to open and read /dev/urandom file and write to /dev/null file */
void read_urandom_write_null() {
	int read_fd, write_fd;
	// Open read file
	read_fd = open("/dev/urandom", O_RDONLY);
	if(read_fd == -1) {
		fprintf(stderr, "Error opening /dev/urandom file: %s\n", strerror(errno));
		exit(-1);
	}
	// Open write file
	write_fd = open("/dev/null", O_WRONLY);
	if(write_fd == -1) {
		fprintf(stderr, "Error opening /dev/null file: %s\n", strerror(errno));
		exit(-1);
	}
	
	// time variables
	struct timeval start, end;
	double elapsed_time;
	// read write variables
	int byteread, bytewrite, i = 1;
	int byte_read_count = 1000000; // bytes to read and write at each iteration
	int total_bytes_count = 0;
	int max_bytes_count = 10000000; // max bytes that can be read and written
	char *buffer;
	
	// Read and Write ~1MB at a time with a max of 10MB
	while(total_bytes_count < max_bytes_count) {
		buffer = (char*)malloc(byte_read_count*sizeof(char));
		
		// start clock
		gettimeofday(&start, NULL);
		// read from dev/urandom
		byteread = read(read_fd, buffer, byte_read_count);
		if(byteread < 0) {
			fprintf(stderr, "Error reading from /dev/urandom file: %s\n", strerror(errno));
		break;
		}
		// write to dev/null
		bytewrite = write(write_fd, buffer, byte_read_count);
		if(bytewrite < 0) {
			fprintf(stderr, "Error writing to /dev/null file: %s\n", strerror(errno));
		break;
		}
		// end clock
		gettimeofday(&end, NULL);
		
		// calculate actual bytes read/written
		total_bytes_count += byteread;
		
		// Calculate time elapsed for read and write
		elapsed_time = (end.tv_sec - start.tv_sec) * 1000;
		elapsed_time += (end.tv_usec - start.tv_usec) / 1000;
		printf("%d. Read and write time for ~1MB: %f milliseconds\n", i, elapsed_time);
		i++;
	}
	free(buffer);
	// Close files
	close(read_fd);
	close(write_fd);
}

/* Function to read from ticket character device driver (created with ticket.c) */
void read_ticket_device_driver() { 
	int read_fd, byteread, i;
	unsigned int read_buffer;
	// Open read file
	read_fd = open("/dev/ticket0", O_RDONLY);
	if(read_fd == -1) {
		fprintf(stderr, "Error opening /dev/ticket0 file: %s\n", strerror(errno));
		exit(-1);
	}
	for(i = 0; i < 10; i++) {
		byteread = read(read_fd, &read_buffer, 4);
		// printf("byte read: %d\n", byteread);
		if(byteread < 0) {
			fprintf(stderr, "Error reading from /dev/ticket0 file: %s\n", strerror(errno));
		break;
		}
		printf("Ticket number: %d\n", read_buffer);
		sleep(1);
	}
}

/* Main function */
int main(int argc, char* argv[]) {
	int action;
	if(argc < 2) {
		printf("No arguments provided.\n");
		exit(-1);  
	} 
	else if(argc > 2) { 
		printf("More than one argument provided.\n");
		exit(-1);
	}
	else {
		if(!isdigit(*argv[1])) {
			printf("Argument entered is not a valid integer.\n");
			exit(-1);
		}
		
		/* Convert command line argument to integer */
		action = atoi(argv[1]);
		
		/* If user gives command line argument as 0 */
		if(action == 0) {
			printf("Reading from dev/input/mouse0...\n");
			read_from_dev_input_mouse0();
		}
		
		/* If user gives command line argument as 1 */
		else if(action == 1) {
			printf("Reading from dev/urandom, writing to dev/null...\n");
			read_urandom_write_null();
		}
		/* If user gives command line argument as 2 */
		else if(action == 2) {
			printf("Reading from character device driver...\n");
			read_ticket_device_driver();
		}
		else {
			printf("Arguments are not in range 0 to 2\n");
			exit(-1);
		}
	}
	return 0;
}


