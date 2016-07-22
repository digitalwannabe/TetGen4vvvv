#pragma once
#include <stdexcept>
#include "stdafx.h"
#include "tetgen.h"
using namespace std;

namespace TTGN {
	extern "C" { __declspec(dllexport) double* tetCalculate(int *retSize, char *behaviour, double *vertXYZ, int *numPoly, int *numVertices, int *vertIndex, int *numHoles, double *fHoleXYZ, int *facetMarker, double *HoleXYZ, double *RegionXYZ, double *RegionAttrib, double *RegionVolConst, int *binSizes, char* fileName); }
	extern "C" {__declspec(dllexport) int ReleaseMemory(double* pArray); }
}