#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

void printInt(int i) {
	printf("%i", i);
}

char* int2string(int i) {
	char* buf = malloc(8);
	itoa(i, buf, 10);
	return buf;
}