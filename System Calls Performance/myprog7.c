/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program to measure time to write files using write and fwrite calls.
 */

/* Include Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/time.h>

const long int FILE_SIZE = 128*1024;

/* Return current size of the file */
long int size_of_file(char filename[]) {

	int prevPtr;
	long int size;
	FILE *fp = fopen(filename, "r");
	if (fileno(fp) < 0) {
		printf("File not found\n");
		exit(-1);
	}
	fseek(fp, 0L, SEEK_END); // seek till end of file
	size = ftell(fp); // find size
	fclose(fp);
	return size;
	
}

/* Calculates and returns time in microseconds between start and end timeval */
long int calculate_elapsed_time(struct timeval start, struct timeval end) {

	return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	
}

/* Calculates the timings for write call and prints summary */
void print_summary(char filename[], int nb, struct timeval start, struct timeval end, struct timeval total_time_per_write, int num_write_calls) {

	struct timeval elapsed_time, time_per_write, time_per_byte;

	// Calculate time elapsed for all the write calls
	elapsed_time.tv_usec = calculate_elapsed_time(start, end);
	time_per_write.tv_usec = total_time_per_write.tv_usec / num_write_calls;
	time_per_byte.tv_usec = time_per_write.tv_usec / nb;
	
	// Print summary
	printf("\nFILE NAME: %s\n", filename);
	printf("Write size: %d bytes\n", nb);
	printf("Number of write calls: %d\n", num_write_calls);
	printf("Time elapsed for write is: %ld microseconds\n", elapsed_time.tv_usec);
	printf("Time per write: %ld microseconds\n", time_per_write.tv_usec);
	printf("Time per byte: %ld microseconds\n", time_per_byte.tv_usec);
	
}

/* Function to write */
void unix_write_sys_call(int nb, int fn) {

	FILE *fp;
	int file_desc;
	struct timeval start, end, startPerWrite, endPerWrite;
	struct timeval elapsed_time, time_per_write;
	struct timeval total_time_per_write = (struct timeval){0};
	int num_write_calls = 0;
	char str[] = "qwertyuiopasdfghjklzxcvbnm\n";
	char filename[45] = {};
	// assign filename
	snprintf(filename, 40, "/tmp/write_test_%dbytes_file%d.txt", nb, fn);
	
	// open file
	fp = fopen(filename, "w");
	// start timer for all writes
	gettimeofday(&start, NULL);
	while (size_of_file(filename) < FILE_SIZE) {
		gettimeofday(&startPerWrite, NULL);
		write(fileno(fp), str, nb);
		gettimeofday(&endPerWrite, NULL);
		num_write_calls++;
		time_per_write.tv_usec = calculate_elapsed_time(startPerWrite, endPerWrite);
		total_time_per_write.tv_usec += time_per_write.tv_usec;
	}
	// end timer for all writes
	gettimeofday(&end, NULL);
	
	// close file
	fclose(fp);
	
	print_summary(filename, nb, start, end, total_time_per_write, num_write_calls);
	
}

/* Function to fwrite */
void fwrite_io_routine(int nb, int fn) {

	FILE *fp;
	struct timeval start, end, startPerfWrite, endPerfWrite;
	struct timeval elapsed_time, time_per_fwrite;
	struct timeval total_time_per_fwrite = (struct timeval){0};
	int num_fwrite_calls = 0;
	char str[] = "qwertyuiopasdfghjklzxcvbnm\n";
	char filename[80] = {};
	
	// assign filename
	snprintf(filename, 42, "/tmp/fwrite_test_%dbytes_file%d.txt", nb, fn);
	
	// open file
	fp = fopen(filename, "w");
	
	// start timer for all writes
	gettimeofday(&start, NULL);
	while (size_of_file(filename) < FILE_SIZE) {
		gettimeofday(&startPerfWrite, NULL);
		fwrite(str, 1, nb, fp); 
		gettimeofday(&endPerfWrite, NULL);
		num_fwrite_calls++;
		// Calculate time elapsed for all the write calls
		time_per_fwrite.tv_usec = calculate_elapsed_time(startPerfWrite, endPerfWrite);
		total_time_per_fwrite.tv_usec += time_per_fwrite.tv_usec;
	}
	// end timer for all writes
	gettimeofday(&end, NULL);
	
	// close file
	fclose(fp);
	
	print_summary(filename, nb, start, end, total_time_per_fwrite, num_fwrite_calls);
	
}

/* Main Function */
int main(int argc, char* argv[]){

	int num_bytes, file_num;
	
	// UNIX write system call
	for (num_bytes = 2; num_bytes <= 65536; num_bytes *= 8) {
		for (file_num = 1; file_num <= 10; file_num++) {
			printf("\n***UNIX write system call***\n");
			unix_write_sys_call(num_bytes, file_num);
		}
		printf("===================================================\n");
	}
	
	printf("\n*************************************************\n");
	printf("*************************************************\n");
	printf("*************************************************\n\n");
	
	
	// I/O routine fwrite
	for (num_bytes = 2; num_bytes <= 65536; num_bytes *= 8) {
		for (file_num = 1; file_num <= 10; file_num++) {
			printf("\n***I/O routine fwrite***\n");
			fwrite_io_routine(num_bytes, file_num);
		}
		printf("===================================================\n");
	}
    return 0;
    
}
