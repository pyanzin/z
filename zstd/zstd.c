#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char* concat(char* s1, char* s2) {
	int resLen = strlen(s1) + strlen(s2) + 1;
	char* res = malloc(resLen);
	strcpy(res, s1);
	strcat(res, s2);
	strcat(res, "\0");
	return res;
}

int compare(char* s1, char* s2) {
	auto res = strcmp(s1, s2);
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

char* int2string(int i) {
	char* buf = malloc(16);
	itoa(i, buf, 10);
	return buf;
}

char* char2string(char ch) {
	char* res = malloc(2);
	*res = ch;
	*(res + 1) = '\0';
	return res;
}

void pause(int millis) {
	Sleep(millis);
}

void* allocate(size_t elemSize) {
	return malloc(elemSize);
}

void* allocateArray(int count, size_t elemSize) {
	int* sizePtr = calloc(count * elemSize + sizeof(int), sizeof(char));
	*sizePtr = count;
	//int* typePtr = sizePtr + 1;
	//*typePtr = 0;
	return sizePtr + 1;
}

int size(void* xs) {
	int* size = (int*)xs - 1;
	return *size;
}

DWORD WINAPI funcWrapper(void* fn) {
	((void(*)())fn)();
	return 0;
}

void fork(void (*fn)()) {
	CreateThread(NULL, 0, funcWrapper, fn, 0, NULL);
}

