// Only create once:
#pragma once

// Include libraries:
#include <string>
#include <vector>
#include "DigestPolar.h"

using namespace std;

// XFoil Interface class:
class XFoil_Interface {

public:

    // Constructor:
    XFoil_Interface();

    // Setters:
    void setAirfoil(string desAirfoil)  { airfoil = desAirfoil; }
    void setOutputFile(string desName)  { outputFile = desName; }
    void setAlpha(double, double, double);
    void setViscous(bool v)             { viscous = v; }
    void setReynolds(double re)         { reynolds = re; }
    void setMach(double m)              { mach = m; }
    void setNcrit(int nc)               { ncrit = nc; }
    void setXtr(double top, double bot) { xtrTop = top; xtrBot = bot; }

    // Getters:
    vector<double> getAlpha();
    vector<double> getCL();
    vector<double> getCD();

    // Member functions:
    vector<DigestPolar> getPolar();
    void runXFOIL();
    void writeInput();

    // Destructors:
    ~XFoil_Interface();

private:

    // Data members:
    string airfoil;
    double alphaStart, alphaEnd, alphaStep;
    string outputFile;
    vector<DigestPolar> polar;
    bool   viscous  = true;
    double reynolds = 1e6;
    double mach     = 0.0;
    int    ncrit    = 9;
    double xtrTop   = 1.0;
    double xtrBot   = 1.0;

};