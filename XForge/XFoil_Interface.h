// Only define once:
#pragma once

// Include packages:
#include <string>
#include <vector>
#include "DigestPolar.h"

using namespace std;

// XFoil interface class to interact with XFOIL backend:
class XFoil_Interface {

public:

    // Constructor:
    XFoil_Interface();

    // Setter:
    void setAirfoil(string desAirfoil) { airfoil = desAirfoil; }
    void setOutputFile(string desName) { outputFile = desName; }
    void setAlpha(double, double, double);

    // Getters to get data from polar:
    vector<DigestPolar> getPolar();
    vector<double> getAlpha(const vector<DigestPolar>&);
    vector<double> getCL(const vector<DigestPolar>&);
    vector<double> getCD(const vector<DigestPolar>&);
    
    // Member functions to run XFOIL:
    void runXFOIL();
    void writeInput();

    // Destructor:
    ~XFoil_Interface();

private:

    // Create private data members:
    string airfoil;
    double alphaStart, alphaEnd, alphaStep;
    string outputFile;

};
