/* 
 * Name: Batul Zamin
 * Class: CS621 Operating Systems 
 * Program: Write a C program to rewrite memory allocation functions.
 */

/* Include Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "malloc2.h"

struct addLenTuple{
    void *addr; // starting address of memory allocated
    size_t len; // size of memory allocated
};
static struct addLenTuple tuple;

/* Function with additional functionalities to malloc */
void *malloc2(size_t size) {
	if(size <= 0) {
		printf("Error allocating memory, zero size\n");
		exit(-1);
	}
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL) {
		printf("Memory allocation failed!\n");
		exit(-1);
	}
	tuple.addr = ptr;
	tuple.len = size;
	printf("Address of ptr in memory: %p\n", ptr);
	printf("Size of allocated memory: %zu bytes\n", tuple.len);
	return ptr;
}

/* Function with additional functionalities to free */
void free2(void *ptr) {
	if (ptr == tuple.addr) {
		tuple.addr = NULL;
		tuple.len = 0;
		free(ptr);
		if (ptr != NULL) {
			ptr = NULL;
		}
		printf("Memory freed successfully!\n");
	}
	else if (ptr > tuple.addr && ptr <= tuple.addr + tuple.len) {
		printf("Freeing memory that is not the first byte of the range of memory that was allocated\n");
		exit(-1);
	}
	else if (tuple.len == 0) {
		printf("Freeing memory that was previously freed (double free)\n");
		exit(-1);
	}
	else {
		printf("Freeing memory that is not allocated with malloc2()\n");
		exit(-1);
	}
}

/* Function with additional functionalities to realloc */
void *realloc2(void *ptr, size_t size) {
	if (ptr == NULL) {
		printf("Pointer is null, allocating memory with malloc2()\n");
		ptr = malloc2(size);
	}
	else if (size == 0 && ptr != NULL) {
		printf("Zero size, calling free2()\n");
		free2(ptr);
	}
	else {
		printf("Reallocating memory...\n");
		if (ptr == tuple.addr) {
			tuple.addr = NULL;
			tuple.len = 0;
			ptr = realloc(ptr, size);
			tuple.addr = ptr; 
			tuple.len = size;
			printf("New Address of ptr in memory: %p\n", ptr);
			printf("New Size of allocated memory: %zu bytes\n", tuple.len);
			return ptr;
		}
		else {
			printf("Reallocating failed!\n");
			exit(-1);
		}
	}
}

/* Function to check if memory is in valid range and proper size */
void memcheck2(void *ptr, size_t size) {
	if (ptr == NULL) {
		printf("Memory error: null pointer given\n");
		exit(-1);
	} 
	if (size == 0) {
		printf("Memory error: zero size given\n");
		exit(-1);
	}
	if (size <= tuple.len && ptr >= tuple.addr && ptr <= tuple.addr + tuple.len) {
		printf("Memory check passed\n");
	}
	else if(size > tuple.len) {
		printf("Memory error: Length size is not in range specified by malloc2 and free2\n"); 
		exit(-1);
	}
	else {
		printf("Memory error: Address is not in range specified by malloc2 and free2\n"); 
		exit(-1);
	}
}

/* Main Function */
int main(int argc, char* argv[]){
	int *ptr = (int *)malloc(2*sizeof(int));
	// int *ptr1 = (int *)malloc2(0); //zero size error
	// free2(ptr); // to test freeing memory not allocated with malloc
	free(ptr);
	
	char *buff = (char *)malloc2(10); // Memory allocated with malloc2
    char c;
    
    memcheck2 (&(buff[5]), 1); // Memcheck passes
    c = buff[5];
    
	realloc2(buff, 40); // Memory reallocated with realloc2
	// realloc2(buff, 0); // trying to reallocate memory of 0 size
	
    memcheck2 (buff, strlen("Hello World !")); // Memcheck passes
    strcpy(buff, "Hello World !");
    
    free2(buff); // frees memory successfully
	// free2(buff); // double free
	
	memcheck2 (&(buff[41]), 1); // Memcheck fails
    
    return 0;
}
