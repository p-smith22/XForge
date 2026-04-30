#pragma once
#include "XFoil_Interface.h"
#include <iostream>
using namespace std;

class Plane: public XFoil_Interface {
	private:
		// set chord, wingspan, epsilon (oswald efficiency factor)
		double setChord(double Chord);
		double setWingspan(double Wingspan);
		double setEpsilon(double Epsilon);

		// get chord, wingspan, epsilon
		void getChord();
		void getWingspan();
		void getEpsilon();

		// calculate the aspect ratio
		double calculateAspectRatio(double chord, double wingspan);
		// calculate the 3D lift, 3D lift slope, 3D drag
		double calculateLift();
		// Calculate the 2D lift to drag ratio, 3D lift to drag ratio
	public:
		double wingspan;
		double chord;
		double aspectRatio;
		double epsilon;
		double lift3D;
		double liftSlope3D;
		double drag3D;

};