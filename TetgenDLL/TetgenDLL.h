#pragma once
#include <stdexcept>
using namespace std;

extern "C" { __declspec(dllexport) double* tetCalculate(); }
extern "C" __declspec(dllexport) int ReleaseMemory(double* pArray)
{
	delete[] pArray;
	//delete[] Usize;
	return 0;
}