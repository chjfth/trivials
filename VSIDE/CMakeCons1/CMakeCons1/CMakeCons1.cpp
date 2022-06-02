// CMakeCons1.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS
#include "CMakeCons1.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Hello CMake." << endl;

	const char *home = getenv("HOME"); // Linux
	const char *userprofile = getenv("USERPROFILE"); // Windows
	const char *myval = getenv("MYVAL");

	printf("HOME        = %s\n", home);
	printf("USERPROFILE = %s\n", userprofile);
	printf("MYVAL=%s\n", myval);

	return 0;
}
