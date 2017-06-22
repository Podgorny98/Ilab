#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

char* str;
int p;

enum {MAX_STRING_LEN = 100};

int GetG(char* string);
int GetE();
int GetT();
int GetP();
int GetN();

int main() {
	char string[MAX_STRING_LEN] = {};
	scanf("%s", string);
	printf("\nAnswer is %d\n", GetG(string));
	return 0;
}

int GetG(char* string) {
	str = string;
	p = 0;
	int val = GetE();
	assert(str[p] == '\0');
	return val;
}
//======================================================================
int GetE() {
	int val = GetT();
	while(str[p] == '+' || str[p] == '-') {
		char operation = str[p];
		p++;
		int val2 = GetT();
		if(operation == '+')
			val += val2;
		else
			val -= val2;
	}
	return val;
}
//======================================================================
int GetT() {
	int val = GetP();
	while(str[p] == '*' || str[p] == '/') {
		char operation = str[p];
		p++;
		int val2 = GetP();
		if(operation == '*')
			val *= val2;
		else
			val /= val2;
	}
	return val;
}
//======================================================================
int GetP() {
	if(str[p] == '(') {
		p++;
		int val = GetE();
		assert(str[p] == ')');
		p++;
		return val;
	}
	else
		return GetN();
}
//======================================================================
int GetN() {
	int val = 0;
	int p0 = p;
	while('0' <= str[p] && str[p] <= '9') {
		val = val * 10 + str[p] - '0';
		p++;
	}
	if(p == p0) {
		printf("Missed number.");
		exit(1);
	}
	return val;
}









