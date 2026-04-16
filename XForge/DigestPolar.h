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
