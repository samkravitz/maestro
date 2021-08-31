/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Noah Bennett
 *
 * FILE: kmalloc.test.c
 * DATE: Augh 31, 2021
 * DESCRIPTI N: test kmalloc
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/kmalloc.h"

#include "string.h"

int main()
{
	char *str;
	str = kmalloc(11 * sizeof(*str));
	memcpy(str, "Hello world..?", strlen("Hello world..?"));
	printf("Running a test on chars: \n");
	printf("\nafter filling str\n");
	printf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		printf("%c", str[i]);
	}
	printf("\n");
	printf("memory block free bit (*str): %d\n", get_block_ptr(str)->free);
	printf("memory block debug value (*str): %x\n", get_block_ptr(str)->debug);

	kfree(str);
	printf("\nafter freeing str (should be the same as above bc its just marked as free):\n");
	printf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		printf("%c", str[i]);
	}

	memcpy(str, "Big Chungis", strlen("Big Chungis"));
	printf("\nafter reassigning str:\n");
	printf("Char is filled with: \n");
	for(int i =0; i<14;i++) {
		printf("%c", str[i]);
	}
	printf("\n");
	printf("memory block free bit (*str): %d\n", get_block_ptr(str)->free);
	printf("memory block debug value (*str): %x\n", get_block_ptr(str)->debug);

	printf("Finding 7! (factorial (but filling the array with each number))\n");
	printf("filling the array: ");
	int *fac = kmalloc(8 * sizeof(*fac));
	for(int i=1;i<=7;i++){
		fac[i-1] = i;
		if(i < 7) printf("%d, ", i);
		else printf("%d", i);
	}
	printf("\n");
	printf("memory block free bit (*fac): %d\n", get_block_ptr(fac)->free);
	printf("memory block debug value (*fac): %x\n", get_block_ptr(fac)->debug);

	printf("7! = ");
	for(int i=1;i<=7;i++){
		if(i < 7) printf("%d * ", fac[i-1]);
		else printf("%d", fac[i-1]);
	}
	printf("\n");

	int sol = 1;
	for(int i=1;i<7;i++) {
		sol *= fac[i];
	}
	printf("7! = %d\n",sol);
	kfree(fac);
	printf("freed the array of numbers :)\n");
	printf("memory block free bit (*fac): %d\n", get_block_ptr(fac)->free);
	printf("memory block debug value (*fac): %x\n", get_block_ptr(fac)->debug);
	
  return 0;
}
