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

int compare(char* s1, char* s2) {
    return strcmp(s1, s2);
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

char* int2string(int i) {
	char* buf = malloc(8);
	itoa(i, buf, 10);
	return buf;
}

void pause(int millis) {
	Sleep(millis);
}

void* allocate(size_t elemSize) {
	return malloc(elemSize);
}

void* Array(size_t elemSize, int count) {
	int* sizePtr = malloc(count * elemSize + sizeof(int) + sizeof(int));
	*sizePtr = count;
	int* typePtr = sizePtr + 1;
	*typePtr = 0;
	return typePtr + 1;
}

DWORD WINAPI funcWrapper(void* fn) {
	((void(*)())fn)();
	return 0;
}

void fork(void (*fn)()) {
	CreateThread(NULL, 0, funcWrapper, fn, 0, NULL);
}

