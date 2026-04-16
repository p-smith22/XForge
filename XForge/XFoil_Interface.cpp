// Include packages:
#include "XFoil_Interface.h"
#include <fstream>
#include <cstdlib>
#include "DigestPolar.h"

// === Constructor ===
// Construct default arguments to build file:
XFoil_Interface::XFoil_Interface() {

    // Set a default airfoil:
    airfoil = "NACA 0012";

    // Set default outputFile:
    outputFile = "polar.dat";

    // Initialize angles:
    alphaStart, alphaEnd, alphaStep = 0.0;

    // Create an input file with defaults just in case:
    writeInput();

}

// === Setters ===
// Set alpha sweep:
void XFoil_Interface::setAlpha(double start, double end, double step) {

    // Set alphas:
    alphaStart = start;
    alphaEnd = end;
    alphaStep = step;

}

// === Getters ===
vector<DigestPolar> XFoil_Interface::getPolar() {

    // Build DigestPolar object:
    DigestPolar PolarFile(outputFile);

    // Unpack and return polar:
    return PolarFile.Unpack();

}

// Fetch aerodynamic data:
vector<double> XFoil_Interface::getAlpha(const std::vector<DigestPolar>& polar) {
    std::vector<double> out;
    for (auto& p : polar) out.push_back(p.getAlphaVal());
    return out;
}
vector<double> XFoil_Interface::getCL(const std::vector<DigestPolar>& polar) {
    std::vector<double> out;
    for (auto& p : polar) out.push_back(p.getCLVal());
    return out;
}
vector<double> XFoil_Interface::getCD(const std::vector<DigestPolar>& polar) {
    std::vector<double> out;
    for (auto& p : polar) out.push_back(p.getCDVal());
    return out;
}

// === Member Functions ===
// Write input file:
void XFoil_Interface::writeInput() {

    // Build file with desired parameters:
    std::ofstream file("input.in");
    file << airfoil << "\n";
    file << "OPER\n";
    file << "PACC\n";
    file << outputFile << "\n\n";
    file << "ASEQ " << alphaStart << " " << alphaEnd << " " << alphaStep << "\n";
    file << "\n";
    file << "QUIT\n";
    file.close();
}


// Run XFOIL with input file:
void XFoil_Interface::runXFOIL() {

    // Clear previous output file:
    remove(outputFile.c_str());

    // Run file:
    system("..\\XForge\\XFOIL\\xfoil.exe < input.in");

}

// === Destructor ===
XFoil_Interface::~XFoil_Interface() {}
