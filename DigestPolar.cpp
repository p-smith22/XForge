// Include packages:
#include "DigestPolar.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>

using namespace std;

// === Constructor ===
// Default constructor for each iteration:
DigestPolar::DigestPolar() {

    // Initialize dummy values (it doesn't really matter for this one, they will get filled):
    alpha = CL = CD = CDp = CM = topXtr = botXtr = 0;

}

// Main constructor for main run:
DigestPolar::DigestPolar(string name) {

    // Assign output file on creation:
    outputFile = name;

    // Initialize dummy values (it doesn't really matter for this one, they will get filled):
    alpha = CL = CD = CDp = CM = topXtr = botXtr = 0;

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
        double alpha_temp, cl_temp, cd_temp, cdp_temp, cm_temp, top_temp, bot_temp;
        ss >> alpha_temp >> cl_temp >> cd_temp >> cdp_temp >> cm_temp >> top_temp >> bot_temp;

        // Assign variables:
        p.setAlpha(alpha_temp);
        p.setCL(cl_temp);
        p.setCD(cd_temp);
        p.setCDp(cdp_temp);
        p.setCM(cm_temp);
        p.setTop(top_temp);
        p.setBot(bot_temp);

        // Add to vector:
        polar.push_back(p);
    }

    // Return polar:
    return polar;

}

// === Destructor ===
DigestPolar::~DigestPolar() {}