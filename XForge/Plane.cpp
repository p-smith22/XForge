#include "Plane.h"

// What do we want to accomplish in this class? 
// Create plane object that calculates the lift, CL alpha, drag, Cd not, lift to drag ratio max 

Plane::Plane() {
	// Initialize dummy values (it doesn't really matter for this one, they will get filled):
	wingspan = chord = aspectRatio = epsilon = lift3D = liftSlope3D = drag3D = 0.0;
}

// Getters:
void Plane::getChord() {
	cout << "The chord is: " << chord << endl;
}

void Plane::getWingspan() {
	cout << "The wingspan is: " << wingspan << endl;
}	

void Plane::getEpsilon() {
	cout << "The oswald efficiency factor is: " << epsilon << endl;
}

// Setters:
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

// calculation fucntions:  
// Aspect Ratio 
double Plane::calculateAspectRatio(double chord, double wingspan) {
	aspectRatio = (wingspan * wingspan) / (chord * wingspan);
	return aspectRatio;
}
// 3D angle of attack 
double Plane::calculate3DAoA() {

}
// 3D Lift Slope 
double Plane::calculate3DLiftSlope() {

}
// 3D Drag 
double Plane::calculatedrag3D() {

	// drag3D = Cd + CL^2/(pi*AR*epsilon);

	return drag3D;
}