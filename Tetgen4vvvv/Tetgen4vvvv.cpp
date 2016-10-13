#include "stdafx.h"
#include "Tetgen4vvvv.h"
#include "tetgen.h"


namespace TTGN {

	tetgenio in, outa;
	tetgenio::facet *f;
	tetgenio::polygon *p;

	int nOfPoints,nOfPointsXYZ,nOfTriIndices,nOfTet,nOfTetIndices,nOfFaces, nOfTetAttr, nOfPointAttr, computeNeighbors, computeRegionAttributes;

	int* tetCalculate(/*int *retSize,*/ char *behaviour, double *vertXYZ, double *vertAttr, int *vertMarker, int *numPoly, int *numVertices, int *vertIndex, int *numFHoles, double *fHoleXYZ, int *facetMarker, double *HoleXYZ, double *RegionXYZ, double *RegionAttrib, double *RegionVolConst, int *binSizes, char* fileName)
	{
		
		// All indices start from 1.
		in.firstnumber = 1;//DW: won't work with 0

		int numPoints = binSizes[0];
		int numFacets = binSizes[1];
		int numHoles = binSizes[2];
		int numRegions = binSizes[3];
		int doWriteIn = binSizes[4];
		int doWriteOut = binSizes[5];
		computeNeighbors = binSizes[6];
		computeRegionAttributes = binSizes[7];

		int polygons = 0;
		int vertices = 0;
		int fHoles = 0;

	
		//POINTS

		in.numberofpoints = numPoints;
		in.pointlist = new REAL[in.numberofpoints * 3];
		
//		in.numberofpointattributes = 1; //anything else possible?
		in.pointattributelist = new REAL[in.numberofpoints];
		in.pointmarkerlist = new int[in.numberofpoints];
		for (int i = 0; i < numPoints; i++) {
			for (int j = 0; j < 3; j++) {
				in.pointlist[i*3+j] = vertXYZ[i*3+j];
			}
			in.pointattributelist[i] = vertAttr[i];
			in.pointmarkerlist[i] = vertMarker[i];
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
				if (computeNeighbors) {
					outa.save_neighbors(fileNameOut);
				}
			}		

		nOfPoints = outa.numberofpoints;
		nOfPointsXYZ = outa.numberofpoints*3;
		nOfTriIndices = outa.numberoftrifaces * 3;
		nOfTet = outa.numberoftetrahedra;
		nOfTetIndices = outa.numberoftetrahedra*4;
		nOfFaces = outa.numberoftrifaces;
		nOfTetAttr = outa.numberoftetrahedronattributes;
		nOfPointAttr = outa.numberofpointattributes;
		
//		int nOfAllEntries = 6;

//		*retSize = nOfAllEntries;
		
		int* retArr = new int[5];

		retArr[0] = nOfPoints;
		retArr[1] = nOfFaces;
		retArr[2] = nOfTet;
		retArr[3] = nOfTetAttr;//
		retArr[4] = nOfPointAttr;//always 1?
		
		
		in.deinitialize();
		
		return retArr;
		delete[] retArr;
		
	}

	void getValues(double * vertXYZ, int * triIndices, int * tetIndices, double * regionAttr, int * pointMarker, int * faceMarker, double * pointAttr, int* neighborList) {

		for (int i = 0; i < nOfPoints; i++) {
			for (int j = 0; j < 3; j++) {
				vertXYZ[i*3+j] = outa.pointlist[i*3+j];
			}
			if (computeRegionAttributes) {
				for (int j = 0; j < nOfPointAttr; j++) {//more than 1 possible? +what for?
					pointAttr[i*nOfPointAttr + j] = outa.pointattributelist[i*nOfPointAttr + j];
				}
			}
			
			pointMarker[i] = outa.pointmarkerlist[i];
		}
		for (int i = 0; i < nOfFaces; i++) {
			for (int j = 0; j < 3; j++) {
				triIndices[i*3+j] = outa.trifacelist[i * 3 + j];
			}
			faceMarker[i] = outa.trifacemarkerlist[i];
		}
		for (int i = 0; i < nOfTet; i++) {
			for (int j = 0; j < 4; j++) {
				tetIndices[i * 4 + j] = outa.tetrahedronlist[i * 4 + j];
				if (computeNeighbors) {
					neighborList[i * 4 + j] = outa.neighborlist[i * 4 + j];
				}
				
			}
			for (int j = 0; j < nOfTetAttr; j++) {//more than 1 possible? +what for?
				regionAttr[i*nOfTetAttr+j] = outa.tetrahedronattributelist[i*nOfTetAttr+j];
			}
		
		}

		

				
		outa.deinitialize();

	}

	int ReleaseMemory(double* pArray)
	{
		delete[] pArray;
//		in.deinitialize();
//		outa.deinitialize();
		return 0;
	}

}