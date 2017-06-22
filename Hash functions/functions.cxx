#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <list>
#include <iterator>
using namespace std; //Poshel naxyu

#include "hash_functions.hxx"

int hash1(const char* s) {
	return (s[0] * 333 ) % HASH_TABLE_SIZE;
}
//======================================================================
int hash2(const char* s) {
	return (strlen(s) * 400 - 21) % HASH_TABLE_SIZE;
}
//======================================================================
int hash3(const char* s) {
	int res = 0;
	int len = strlen(s);
	for(int i = 0; i < len; i++)
		res += (s[i] * i);
	return (res * 6546) % HASH_TABLE_SIZE;
}
//======================================================================
int hash4(const char* s) {
	int res = 0;
	int len = strlen(s);
	for(int i = 0; i < len; i++)
		res = res * 41 + s[i];
	res = res % HASH_TABLE_SIZE;
	return abs(res);
}
//======================================================================
int hash5(const char* s) {
	int res = 0;
	int len = strlen(s);
	for(int i = 0; i < len; i++) {
		res += s[i];
		res *= (res >> 7);
		res += (s[i] << 3);
	}
	return abs(res) % HASH_TABLE_SIZE;
}
