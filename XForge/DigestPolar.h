// Only create once:
#pragma once

// Import packages:
#include <string>
#include <vector>

using namespace std;

class DigestPolar {

public:

    // Constructor:
    DigestPolar(); 
    DigestPolar(string);

    // Member functions:
    vector<DigestPolar> Unpack();

    // Setters:
    void setAlpha(double a) { alpha = a; }
    void setCL(double cl_temp) { CL = cl_temp; }
    void setCD(double cd_temp) { CD = cd_temp; }
    void setCDp(double cdp_temp) { CDp = cdp_temp; }
    void setCM(double cm_temp) { CM = cm_temp; }
    void setTop(double top) { topXtr = top; }
    void setBot(double bot) { botXtr = bot; }

    // Getters:
    double getAlphaVal() const { return alpha; }
    double getCLVal() const { return CL; }
    double getCDVal() const { return CD; }

    // Destructor:
    ~DigestPolar();

private:

    // Data members:
    string outputFile;
    double alpha, CL, CD, CDp, CM, topXtr, botXtr;
    vector<DigestPolar> polar;

};
