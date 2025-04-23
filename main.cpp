#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;


int main()
{
    PolygonalMesh mesh;
    if(!ImportMesh(mesh))
    {
        cerr << "file not found" << endl;
        return 1;
    }

    Gedim::UCDUtilities utilities;
    utilities.ExportPoints("./Cell0Ds.inp",
                           mesh.Cell0DsCoordinates);

    utilities.ExportSegments("./Cell1Ds.inp",
                             mesh.Cell0DsCoordinates,
                             mesh.Cell1DsExtrema);
    return 0;
}
