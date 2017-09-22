#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char* concat(char* s1, char* s2) {
	int resLen = strlen(s1) + strlen(s2);
	char* res = malloc(resLen);
	strcpy(res, s1);
	strcat(res, s2);
	return res;
}

int len(char* str) {
	return strlen(str);
}

void print(char* s) {
	printf("%s", s);
}

char* readline() {
	char* result = malloc(0xFF);

	scanf("%s", result);
	return result;
}

void printInt(int i) {
	printf("%i", i);
}

char* int2string(int i) {
	char* buf = malloc(8);
	itoa(i, buf, 10);
	return buf;
}

void* allocate(size_t elemSize) {
	return malloc(elemSize);
}

void* allocateArray(size_t elemSize, int count) {
	
}

DWORD WINAPI funcWrapper(void* fn) {
	((void(*)())fn)();
	return 0;
}

void forkThread(void (*fn)()) {
	CreateThread(NULL, 0, funcWrapper, fn, 0, NULL);
}

void helloVasily() {
	print("hello vasiliyy");
}

void forkIt() {
	forkThread(helloVasily);
}

