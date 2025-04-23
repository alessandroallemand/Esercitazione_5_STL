#pragma once

#include <iostream>
#include "PolygonalMesh.hpp"
namespace PolygonalLibrary{
bool ImportMesh(PolygonalMesh& mesh);

bool ImportCell0Ds(PolygonalMesh& mesh);

bool ImportCell1Ds(PolygonalMesh& mesh);

bool ImportCell2Ds(PolygonalMesh& mesh);

bool edges_length(PolygonalMesh& mesh);

bool area_area(PolygonalMesh& mesh);
}