// Semestralka2.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <chrono>

#include "structures/table/treap.h"
#include "System.h"
#include <iostream>

using namespace std;

int main()
{
	initHeapMonitor();

	//chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

	System s = System();

	//chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	//long long duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
	//cout << "\n\nRunning time: " << duration / 1000 << " s";
	return 0;
}

