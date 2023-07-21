#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "gen.h"
#include "stdio.h"


void writefile(num* k1, FILE* fp);
int genAndwFile(num *alpha[]);
extern "C" int server1(int getalpha, char * filenumber);
