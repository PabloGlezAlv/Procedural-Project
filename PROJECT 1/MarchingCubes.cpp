#include "pch.h"
#include "MarchingCubes.h"
#include <DirectXMath.h>

/*
   Given a grid cell and an isolevel, calculate the triangular
   facets required to represent the isosurface through the cell.
   Return the number of triangular facets, the array "triangles"
   will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
   of totally below the isolevel.
*/
int MarchingCubes::Polygonise(GRIDCELL grid, double isolevel, TRIANGLE* triangles)
{
	int i, ntriang;
	int cubeindex;
	DirectX::XMFLOAT3 vertlist[12];
	/*
  Determine the index into the edge table which
  tells us which vertices are inside of the surface
*/
	cubeindex = 0;
	if (grid.val[0] < isolevel) cubeindex |= 1;
	if (grid.val[1] < isolevel) cubeindex |= 2;
	if (grid.val[2] < isolevel) cubeindex |= 4;
	if (grid.val[3] < isolevel) cubeindex |= 8;
	if (grid.val[4] < isolevel) cubeindex |= 16;
	if (grid.val[5] < isolevel) cubeindex |= 32;
	if (grid.val[6] < isolevel) cubeindex |= 64;
	if (grid.val[7] < isolevel) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edgeTable[cubeindex] == 0)
		return(0);

	/* Find the vertices where the surface intersects the cube */
	if (edgeTable[cubeindex] & 1)
		vertlist[0] =
		VertexInterp(isolevel, grid.myPoints[0], grid.myPoints[1], grid.val[0], grid.val[1]);
	if (edgeTable[cubeindex] & 2)
		vertlist[1] =
		VertexInterp(isolevel, grid.myPoints[1], grid.myPoints[2], grid.val[1], grid.val[2]);
	if (edgeTable[cubeindex] & 4)
		vertlist[2] =
		VertexInterp(isolevel, grid.myPoints[2], grid.myPoints[3], grid.val[2], grid.val[3]);
	if (edgeTable[cubeindex] & 8)
		vertlist[3] =
		VertexInterp(isolevel, grid.myPoints[3], grid.myPoints[0], grid.val[3], grid.val[0]);
	if (edgeTable[cubeindex] & 16)
		vertlist[4] =
		VertexInterp(isolevel, grid.myPoints[4], grid.myPoints[5], grid.val[4], grid.val[5]);
	if (edgeTable[cubeindex] & 32)
		vertlist[5] =
		VertexInterp(isolevel, grid.myPoints[5], grid.myPoints[6], grid.val[5], grid.val[6]);
	if (edgeTable[cubeindex] & 64)
		vertlist[6] =
		VertexInterp(isolevel, grid.myPoints[6], grid.myPoints[7], grid.val[6], grid.val[7]);
	if (edgeTable[cubeindex] & 128)
		vertlist[7] =
		VertexInterp(isolevel, grid.myPoints[7], grid.myPoints[4], grid.val[7], grid.val[4]);
	if (edgeTable[cubeindex] & 256)
		vertlist[8] =
		VertexInterp(isolevel, grid.myPoints[0], grid.myPoints[4], grid.val[0], grid.val[4]);
	if (edgeTable[cubeindex] & 512)
		vertlist[9] =
		VertexInterp(isolevel, grid.myPoints[1], grid.myPoints[5], grid.val[1], grid.val[5]);
	if (edgeTable[cubeindex] & 1024)
		vertlist[10] =
		VertexInterp(isolevel, grid.myPoints[2], grid.myPoints[6], grid.val[2], grid.val[6]);
	if (edgeTable[cubeindex] & 2048)
		vertlist[11] =
		VertexInterp(isolevel, grid.myPoints[3], grid.myPoints[7], grid.val[3], grid.val[7]);

	/* Create the triangle */
	ntriang = 0;
	for (i = 0; triTable[cubeindex][i] != -1; i += 3) {
			triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i]];
		triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i + 1]];
		triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i + 2]];
		ntriang++;
	}

	return(ntriang);
}

/*
Linearly interpolate the position where an isosurface cuts
an edge between two vertices, each with their own scalar value
 */
DirectX::XMFLOAT3 MarchingCubes::VertexInterp(double isolevel, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, double valp1, double valp2)
{
	double mu;
	DirectX::XMFLOAT3 p;

	if (abs(isolevel - valp1) < 0.00001)
		return(p1);
	if (abs(isolevel - valp2) < 0.00001)
		return(p2);
	if (abs(valp1 - valp2) < 0.00001)
		return(p1);
	mu = (isolevel - valp1) / (valp2 - valp1);
	p.x = p1.x + mu * (p2.x - p1.x);
	p.y = p1.y + mu * (p2.y - p1.y);
	p.z = p1.z + mu * (p2.z - p1.z);

	return(p);
}

DirectX::XMFLOAT3 MarchingCubes::LinearInterp(double isolevel, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, double valp1, double valp2)
{
	if (MinorThan(p2,p1))
	{
		DirectX::XMFLOAT3 temp;
		temp = p1;
		p1 = p2;
		p2 = temp;
	}

	DirectX::XMFLOAT3 p;
	if (fabs(valp1 - valp2) > 0.00001)
		p = SumVector(p1, DivideDouble((RestVector(p2,p1)), (valp2 - valp1) * (isolevel - valp1)));
	else
		p = (DirectX::XMFLOAT3)p1;
	return p;
}

bool MarchingCubes::MinorThan(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	if (v1.x != v2.x) {
		return v1.x < v2.x;
	}
	else if (v1.y != v2.y) {
		return v1.y < v2.y;
	}
	else {
		return v1.z < v2.z;
	}
}

DirectX::XMFLOAT3 MarchingCubes::RestVector(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	return DirectX::XMFLOAT3(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}

DirectX::XMFLOAT3 MarchingCubes::SumVector(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	return DirectX::XMFLOAT3(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

DirectX::XMFLOAT3 MarchingCubes::DivideDouble(DirectX::XMFLOAT3 vec, double divisor)
{
	return DirectX::XMFLOAT3(vec.x / divisor, vec.y / divisor, vec.z / divisor);
}
