// Include packages:
#include <fstream>
#include <cstdlib>
#include "XFoil_Interface.h"
#include "DigestPolar.h"
#include <iostream>

using namespace std;

int main() {

    // Create XFOIL object:
    XFoil_Interface myXFOIL;

    // Import desired settings:
    myXFOIL.setAirfoil("NACA 0012");
    myXFOIL.setAlpha(0, 10, 1);
    myXFOIL.setOutputFile("polar.dat");

    // Build input file:
    myXFOIL.writeInput();

    // Run XFOIL:
    myXFOIL.runXFOIL();

    // Digest resutls:
    vector<DigestPolar> polar = myXFOIL.getPolar();

    // Fetch aerodynamic data:
    vector<double> CL_data = myXFOIL.getCL(polar);
    vector<double> CD_data = myXFOIL.getCD(polar);
    vector<double> ALPHA_data = myXFOIL.getAlpha(polar);

    // Print CL data to test:
    cout << "\n\n\n" << "Printing Results:" << endl;
    for (int i = 0; i < CL_data.size(); i++) {
        cout << CL_data[i] << " ";
    }

    // End main:
    return 0;
}