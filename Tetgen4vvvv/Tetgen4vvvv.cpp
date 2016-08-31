#include "stdafx.h"
#include "Tetgen4vvvv.h"
#include "tetgen.h"

namespace TTGN {

	tetgenio in, outa;
	tetgenio::facet *f;
	tetgenio::polygon *p;

	double* tetCalculate(int *retSize, char *behaviour, double *vertXYZ, int *numPoly, int *numVertices, int *vertIndex, int *numFHoles, double *fHoleXYZ, int *facetMarker, double *HoleXYZ, double *RegionXYZ, double *RegionAttrib, double *RegionVolConst, int *binSizes, char* fileName)
	{
		
		

		// All indices start from 1.
		in.firstnumber = 1;//DW: won't work with 0

		int numPoints = binSizes[0];
		int numFacets = binSizes[1];
		int numHoles = binSizes[2];
		int numRegions = binSizes[3];
		int doWriteIn = binSizes[4];
		int doWriteOut = binSizes[5];

		int polygons = 0;
		int vertices = 0;
		int fHoles = 0;

		//POINTS

		in.numberofpoints = numPoints;
		in.pointlist = new REAL[in.numberofpoints * 3];
		for (int i = 0; i < numPoints * 3; i++) {
			in.pointlist[i] = vertXYZ[i];
		}
		

		//FACETS

		in.numberoffacets = numFacets;
		in.facetlist = new tetgenio::facet[in.numberoffacets];
		in.facetmarkerlist = new int[in.numberoffacets];
		
	
		for (int facetID = 0; facetID < numFacets; facetID++) {
			f = &in.facetlist[facetID];
			f->numberofpolygons = numPoly[facetID];
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = numFHoles[facetID];
//			f->numberofholes = 0;
			
			if (numFHoles[facetID] != 0) {
				f->holelist = new REAL[numFHoles[facetID] * 3];
				for (int holeIDXYZ = 0; holeIDXYZ < numFHoles[facetID]*3; holeIDXYZ++) {
					f->holelist[holeIDXYZ] = fHoleXYZ[fHoles*3+ holeIDXYZ];
				}
			}
			else {
				f->holelist = NULL;
			}
			fHoles += numFHoles[facetID];

			for (int polyID = 0; polyID < numPoly[facetID]; polyID++) {
				p = &f->polygonlist[polyID];
				p->numberofvertices = numVertices[polygons+polyID];
				p->vertexlist = new int[p->numberofvertices];
				for (int vertexID = 0; vertexID < numVertices[polygons + polyID]; vertexID++) {
					p->vertexlist[vertexID] = vertIndex[vertices + vertexID];
				}
				vertices+= numVertices[polygons + polyID];
			}

			polygons += numPoly[facetID];

			in.facetmarkerlist[facetID] = facetMarker[facetID];
		}

		//HOLES
		in.numberofholes = numHoles;
		if (numHoles != 0) {			
			in.holelist = new REAL[numHoles * 3];
			for (int i = 0; i < numHoles * 3; i++) {
				in.holelist[i] = HoleXYZ[i];
			}
		}
		else {
			in.holelist = NULL;
		}


		//REGIONS
		in.numberofregions = numRegions;
		if (numRegions != 0) {
			in.regionlist = new REAL[numRegions * 5];
			
			for (int i = 0; i < numRegions; i++) {
				in.regionlist[i*5] = RegionXYZ[i*3];//x
				in.regionlist[i * 5 + 1] = RegionXYZ[i * 3 + 1];//y
				in.regionlist[i * 5 + 2] = RegionXYZ[i * 3 + 2];//z
				in.regionlist[i * 5 + 3] = RegionAttrib[i];//region attribute
				in.regionlist[i * 5 + 4] = RegionVolConst[i];//region attribute
			}
		}
		else {
			in.regionlist = NULL;
		}
		if (doWriteIn) {
			in.save_nodes(fileName);
			in.save_poly(fileName);
		}

		// Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
		//   do quality mesh generation (q) with a specified quality bound
		//   (1.414), and apply a maximum volume constraint (a0.1).

			tetrahedralize(behaviour, &in, &outa);



			// Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.

			
			char* str2;
			str2 = "_out";
			char * fileNameOut = (char *)malloc(1 + strlen(fileName) + strlen(str2));
			strcpy(fileNameOut, fileName);
			strcat(fileNameOut, str2);
			if (doWriteOut) {
				outa.save_nodes(fileNameOut);
				outa.save_elements(fileNameOut);
				outa.save_faces(fileNameOut);
			}		

		int nOfPoints = outa.numberofpoints;
		int nOfPointsXYZ = outa.numberofpoints*3;
		int nOfTriIndices = outa.numberoftrifaces * 3;
		int nOfTet = outa.numberoftetrahedra;
		int nOfTetIndices = outa.numberoftetrahedra*4;

		int nOfAllEntries = 3 + nOfPointsXYZ + nOfTriIndices + nOfTetIndices;

		*retSize = nOfAllEntries;
		
		double* retArr = new double[nOfAllEntries];
		//	char* commands = "pq1.414a0.1";
		//	retArr.size = 3;

		retArr[0] = (double)nOfPoints;
		retArr[1] = (double)nOfTriIndices;
		retArr[2] = (double)nOfTetIndices;

		for (int i = 0; i < nOfPointsXYZ; i++) {
			int j = 3 + i;
			retArr[j] = outa.pointlist[i];
		}
		for (int i = 0; i < nOfTriIndices; i++) {
			int j = 3 + nOfPointsXYZ+i;
			retArr[j] = outa.trifacelist[i];
		}
		for (int i = 0; i < nOfTetIndices; i++) {
			int j = 3 + nOfPointsXYZ + nOfTriIndices + i;
			retArr[j] = outa.tetrahedronlist[i];
		}
		
		in.deinitialize();
		outa.deinitialize();

		return retArr;
		delete[] retArr;
	}


	int ReleaseMemory(double* pArray)
	{
		delete[] pArray;
//		in.deinitialize();
//		outa.deinitialize();
		return 0;
	}

}