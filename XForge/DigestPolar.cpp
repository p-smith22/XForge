// Include packages:
#include "DigestPolar.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>

using namespace std;

// === Constructor ===
// Default constructor for each iteration:
DigestPolar::DigestPolar() {}

// Main constructor for main run:
DigestPolar::DigestPolar(string name) {

	// Assign output file on creation:
	outputFile = name;

}

// === Member Functions ===
vector<DigestPolar> DigestPolar::Unpack() {

    // Clear polar vector for new file:
    polar.clear();

    // Load file and initialize line deconstructor:
    ifstream file(outputFile);
    string line;

    // Skip header and dashed line:
    while (std::getline(file, line))
        if (line.find("alpha") != string::npos) break;
    std::getline(file, line);

    // While there are still lines to digest:s
    while (std::getline(file, line)) {

        // Skip empty lines:
        if (line.empty()) continue;

        // Pack line into string stream, create a temp object to pack values:
        istringstream ss(line);
        DigestPolar p;

        // Delimit into the respective variables:
        ss >> p.alpha >> p.CL >> p.CD >> p.CDp >> p.CM >> p.topXtr >> p.botXtr;

        // Add to vector:
        polar.push_back(p);
    }

    // Return polar:
    return polar;

}

// === Destructor ===
DigestPolar::~DigestPolar() {}
