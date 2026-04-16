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

    // Create pointer data member:
    vector<DigestPolar> polar;


};


class XfoilRunner {
public:
    // ... existing members ...

    void parsePolar();

    std::vector<PolarPoint> getPolar() { return polar; }
    std::vector<double> getAlpha() { return getCol(&PolarPoint::alpha); }
    std::vector<double> getCL() { return getCol(&PolarPoint::CL); }
    std::vector<double> getCD() { return getCol(&PolarPoint::CD); }
    std::vector<double> getCM() { return getCol(&PolarPoint::CM); }
    double getCLmax();
    double getLDmax();  // max CL/CD

private:
    std::vector<PolarPoint> polar;
    std::vector<double> getCol(double PolarPoint::* member);
};

struct PolarPoint {
    double alpha, CL, CD, CDp, CM, topXtr, botXtr;
};