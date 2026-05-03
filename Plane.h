#pragma once
#include "XFoil_Interface.h"
#include <iostream>
#include <vector>
using namespace std;

class Plane : public XFoil_Interface {
public:
    // Constructors and overidden constructors
    Plane();
    Plane(double chord, double wingspan, double epsilon, double rho, double Vinf);

    // setters
    double setChord(double Chord);
    double setWingspan(double Wingspan);
    double setEpsilon(double Epsilon);
    void   setRho(double Rho);
    void   setVinf(double vinf);

    // Getters
    void getChord()   const;
    void getWingspan() const;
    void getEpsilon() const;

    // Geometry:
    double calculateAspectRatio();

    // Aerodynamic calculations (2D to 3D)
    double calculate3DLiftSlope();          // a3D  (1/rad)
    double calculate3DAoA(double alpha2D, double CL2D);
    double calculatedrag3D(double CD2D, double CL3D);

    // Lift-to-drag ratios at alpha:
    pair<double, double> calculateLift2Drag(int alphaIdx); // returns 2D and 3D

    // Schrenk approximation
    vector<double> schrenkApproximation();

    // Prandtl Lifting Line Theory
    vector<double> liftingLineTheory(int N = 30); // returns sectional lift (N/m) for plotting

protected:
    vector<DigestPolar> polar;

private:
    double wingspan;
    double chord;
    double aspectRatio;
    double epsilon;      // Oswald eff. factor
    double rho;          // air density (kg/m^3)
    double Vinf;         // freestream velocity (m/s)
    double liftSlope3D;
    double lift3D;
    double drag3D;
};
