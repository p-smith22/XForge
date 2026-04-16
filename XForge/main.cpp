#include <fstream>
#include <cstdlib>

int main() {

    std::ofstream file("input.in");

    file << "NACA 2412\n";   // define airfoil
    file << "OPER\n";        // enter oper mode
    file << "PACC\n";        // polar accumulation
    file << "polar.dat\n\n"; // output file
    file << "ALFA 0 10 1\n"; // run alpha sweep
    file << "\n";
    file << "QUIT\n";

    file.close();

    // run xfoil with script:
    system("..\\XForge\\XFOIL\\xfoil.exe < input.in");
    
    return 0;
}