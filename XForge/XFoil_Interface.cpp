// Include packages:
#include "XFoil_Interface.h"
#include <fstream>
#include <cstdlib>

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

// Set output file name:
void XFoil_Interface::setOutputFile(std::string name) {

    // Set file name:
    outputFile = name;

}

// Set desired airfoil:
void XFoil_Interface::setAirfoil(std::string myAirfoil) {

    // Set airfoil:
    airfoil = myAirfoil;

}

// === Getters ===


// === Member Functions ===
// Write input file:
void XFoil_Interface::writeInput() {

    // Build file with desired parameters:
    std::ofstream file("input.in");
    file << airfoil << "\n";
    file << "OPER\n";
    file << "PACC\n";
    file << outputFile << "\n\n";
    file << "ALFA " << alphaStart << " " << alphaEnd << " " << alphaStep << "\n";
    file << "QUIT\n";
    file.close();
}


// Run XFOIL with input file:
void XFoil_Interface::runXFOIL() {

    // Run file:
    system("..\\XForge\\XFOIL\\xfoil.exe < input.in");

}

// === Destructor ===
XFoil_Interface::~XFoil_Interface() {}







// XfoilRunner.cpp
#include <fstream>
#include <sstream>

void XfoilRunner::parsePolar() {
    polar.clear();
    std::ifstream file(outputFile);
    std::string line;

    // skip header lines
    while (std::getline(file, line))
        if (line.find("alpha") != std::string::npos) break;
    std::getline(file, line); // skip dashes line

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        PolarPoint p;
        ss >> p.alpha >> p.CL >> p.CD >> p.CDp >> p.CM >> p.topXtr >> p.botXtr;
        polar.push_back(p);
    }
}

double XfoilRunner::getCLmax() {
    double max = -1e9;
    for (auto& p : polar) max = std::max(max, p.CL);
    return max;
}

double XfoilRunner::getLDmax() {
    double max = -1e9;
    for (auto& p : polar) max = std::max(max, p.CL / p.CD);
    return max;
}

std::vector<double> XfoilRunner::getCol(double PolarPoint::* member) {
    std::vector<double> out;
    for (auto& p : polar) out.push_back(p.*member);
    return out;
}