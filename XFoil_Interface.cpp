// Include libraries:
#include "XFoil_Interface.h"
#include "DigestPolar.h"
#include <fstream>
#include <cstdlib>
#include <string>

// === CONSTRUCTOR ===
XFoil_Interface::XFoil_Interface() {

    // Set default options:
    airfoil    = "NACA 0012";
    outputFile = "polar.dat";

    // Initialize variables:
    alphaStart = alphaEnd = alphaStep = 0.0;
    viscous    = true;
    reynolds   = 1e6;
    mach       = 0.0;
    ncrit      = 9;
    xtrTop     = 1.0;
    xtrBot     = 1.0;

    // Write a default input file just in case:
    writeInput();
}

// === SETTERS ===
void XFoil_Interface::setAlpha(double start, double end, double step) {

    // Set alpha sweep:
    alphaStart = start;
    alphaEnd   = end;
    alphaStep  = step;
}

// Get aerodynamic data:
vector<double> XFoil_Interface::getAlpha() {
    vector<double> out;
    for (auto& p : polar) out.push_back(p.getAlphaVal());
    return out;
}
vector<double> XFoil_Interface::getCL() {
    vector<double> out;
    for (auto& p : polar) out.push_back(p.getCLVal());
    return out;
}
vector<double> XFoil_Interface::getCD() {
    vector<double> out;
    for (auto& p : polar) out.push_back(p.getCDVal());
    return out;
}

// Digest the polar file produced from XFOIL:
vector<DigestPolar> XFoil_Interface::getPolar() {

    // Create polar object:
    DigestPolar PolarFile(outputFile);

    // Unpack into vector:
    return PolarFile.Unpack();
}

// Write input file:
void XFoil_Interface::writeInput() {
    std::ofstream file("input.in");

    // Suppress graphics
    file << "PLOP\n";
    file << "G\n";
    file << "\n";

    // Load airfoil
    file << airfoil << "\n";

    // Enter OPER menu:
    file << "OPER\n";

    // Set Mach:
    if (mach > 0.0)
        file << "MACH " << mach << "\n";

    // Viscous mode:
    if (viscous) {
        file << "VISC " << static_cast<long long>(reynolds) << "\n";
        // Set Ncrit / Xtr via VPAR submenu
        file << "VPAR\n";
        file << "N " << ncrit << "\n";
        file << "XTR " << xtrTop << " " << xtrBot << "\n";
        file << "\n"; // exit VPAR
    }

    // Accumulate polar:
    file << "PACC\n";
    file << outputFile << "\n\n";

    // Alpha sweep:
    file << "ASEQ " << alphaStart << " " << alphaEnd << " " << alphaStep << "\n";
    file << "\n";
    file << "QUIT\n";
    file.close();
}

// Run XFOIL:
void XFoil_Interface::runXFOIL() {

    // Remove old input file and write new input:
    remove(outputFile.c_str());
    writeInput();

    // Run XFOIL:
    system("xfoil.exe < input.in");

    // Unpack polar:
    polar = this->getPolar();
}

// === DESTRUCTOR ===
XFoil_Interface::~XFoil_Interface() {}