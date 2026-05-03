#include "Plane.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

// define pi as cmath doesn't work for some reason
static const double PI = 3.14159265;


//  Constructors
Plane::Plane()
    : wingspan(1.0), chord(0.1), aspectRatio(10.0),
    epsilon(0.85), rho(1.225), Vinf(30.0),
    liftSlope3D(0.0), lift3D(0.0), drag3D(0.0)
{
}

// Overide constructor to actually generate usable plane class
Plane::Plane(double chord, double wingspan, double epsilon,
    double rho, double Vinf)
    : chord(chord), wingspan(wingspan), epsilon(epsilon),
    rho(rho), Vinf(Vinf),
    liftSlope3D(0.0), lift3D(0.0), drag3D(0.0)
{
    aspectRatio = calculateAspectRatio();
}

//  Setters
double Plane::setChord(double Chord) {
    chord = Chord;
    return chord;
}
double Plane::setWingspan(double Wingspan) {
    wingspan = Wingspan;
    return wingspan;
}
double Plane::setEpsilon(double Epsilon) {
    epsilon = Epsilon;
    return epsilon;
}
void Plane::setRho(double Rho) {
    rho = Rho;
}
void Plane::setVinf(double vinf) {
    Vinf = vinf;
}


//  getters
void Plane::getChord() const {
    cout << "Chord            : " << chord << " m" << endl;
}
void Plane::getWingspan() const {
    cout << "Wingspan         : " << wingspan << " m" << endl;
}
void Plane::getEpsilon() const {
    cout << "Oswald Efficiency: " << epsilon << endl;
}

//  Plane goemtyt
// AR = b^2 / S  (rectangular wing shape only)
double Plane::calculateAspectRatio() {
    aspectRatio = (wingspan * wingspan) / (chord * wingspan);
    return aspectRatio;
}

// 2D to 3D calculations
// Prandtl finite-wing lift slope: a3D = a2D / (1 + a2D / (pi * AR * epsilon))
// Assumes thin-airfoil 2-D lift slope: a2D = 2*pi [1/rad]
double Plane::calculate3DLiftSlope() {
    double a2D = 2.0 * PI;
    liftSlope3D = a2D / (1.0 + a2D / (PI * aspectRatio * epsilon));
    return liftSlope3D;
}

// Corrected 3-D angle of attack:
// AoA3D = AoA2D + CL2D / (pi * AR * epsilon) - (degrees)
double Plane::calculate3DAoA(double alpha2D, double CL2D) {
    double inducedAngle_rad = CL2D / (PI * aspectRatio * epsilon);
    double inducedAngle_deg = inducedAngle_rad * (180.0 / PI);
    return alpha2D + inducedAngle_deg;
}

// 3D drag (profile + induced):
// CD3D = CD2D + CL^2 / (pi * AR * epsilon)
double Plane::calculatedrag3D(double CD2D, double CL3D) {
    drag3D = CD2D + (CL3D * CL3D) / (PI * aspectRatio * epsilon);
    return drag3D;
}

//  Lift-to-Drag Ratios
// Returns (L/D_2D, L/D_3D) after polar index
pair<double, double> Plane::calculateLift2Drag(int alphaIdx) {
    if (polar.empty())
        cout << "Plane: polar is empty — call runXFOIL() first." << endl;
    if (alphaIdx < 0 || alphaIdx >= (int)polar.size())
        cout << "Plane: alphaIdx out of polar range." << endl;

    double CL2D = polar[alphaIdx].getCLVal();
    double CD2D = polar[alphaIdx].getCDVal();

    if (CD2D == 0.0)
        cout << "Plane: CD2D is zero, cannot compute L/D." << endl;

    double LD2D = CL2D / CD2D;

    // 3D induced drag correction for L/D:
    double CD3D = calculatedrag3D(CD2D, CL2D);
    double LD3D = (CD3D != 0.0) ? (CL2D / CD3D) : 0.0;

    return { LD2D, LD3D };
}

//  Schrenk Approximation — spanwise lift distribution
//  For a rectangular adn elliptical combnination
vector<double> Plane::schrenkApproximation() {
    if (polar.empty())
        cout << "Plane: polar is empty, call runXFOIL() first." << endl;

    // Average 2-D CL over the polar:
    double CLavg = 0.0;
    for (auto& p : polar) CLavg += p.getCLVal();
    CLavg /= polar.size();

    double q = 0.5 * rho * Vinf * Vinf;   // dynamic pressure (Pa)
    double b2 = wingspan / 2.0; // semi-span 

    int N = 50;
    double dy = b2 / N;
    vector<double> liftDist(N);

    for (int i = 0; i < N; i++) {
        double y = (i + 0.5) * dy;  

        // Rectangular distribution (constant chord):
        double q_rect = q * chord * CLavg;

        // Elliptic distribution:
        double ratio = y / b2;
        double q_ellip = q * chord * CLavg * (4.0 / PI) * sqrt(max(0.0, 1.0 - ratio * ratio));
        liftDist[i] = 0.5 * (q_rect + q_ellip);
    }
    return liftDist;  // N/m at each spanwise strip
}

//  Prandtl Lifting Line Theory 
vector<double> Plane::liftingLineTheory(int N) {
    if (polar.empty())
        cout << "Plane: polar is empty, call runXFOIL() first." << endl;
    if (N < 2)
        cout << "Plane: N must be >= 2 for lifting line theory." << endl;

    // Average 2-D lift slope and zero-lift alpha from polar
    double a2D = 2.0 * PI;   // (1/rad)
    double alpha_L0 = 0.0;   // (rad)

    // Average geometric angle of attack from polar (rad):
    double alphaGeom = 0.0;
    for (auto& p : polar) alphaGeom += p.getAlphaVal();
    alphaGeom /= polar.size();
    alphaGeom *= PI / 180.0;
    double c = chord;
    
    // Creating the points
    vector<double> theta(N);
    for (int i = 0; i < N; i++)
        theta[i] = (i + 1) * PI / (N + 1);

    // Generate matrix 
    double mu = (c * a2D) / (4.0 * wingspan);

    MatrixXd M(N, N);
    for (int i = 0; i < N; i++) {
        double sinT = sin(theta[i]);
        for (int n = 0; n < N; n++) {
            int    nOdd = 2 * n + 1;
            M(i, n) = sin(nOdd * theta[i]) * ((double)nOdd / sinT + 1.0 / mu);
        }
    }

    VectorXd rhs(N);
    rhs.setConstant(alphaGeom - alpha_L0);

    // SOlivng for An
    VectorXd A = M.colPivHouseholderQr().solve(rhs);

    // Calculating sectional circulation and lift
    vector<double> Lift_sectional(N);
    for (int i = 0; i < N; i++) {
        double Gamma_i = 0.0;
        for (int n = 0; n < N; n++) {
            int nOdd = 2 * n + 1;
            Gamma_i += A(n) * sin(nOdd * theta[i]);
        }
        Gamma_i *= 2.0 * wingspan * Vinf;          // (m^2/s)
        Lift_sectional[i] = rho * Vinf * Gamma_i;  // (N/m)
    }

    return Lift_sectional;
}
