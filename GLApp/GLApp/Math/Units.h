#pragma once

class Units
{
public:
	Units(double lUnit, double MUnit);
	~Units() {};
	//Time Scale
	double lengthUnit;
	double massUnit;
	double timeUnit;

	//Derived Units
	double velocityUnit = lengthUnit / timeUnit;
	double accelerationUnit = lengthUnit / (timeUnit * timeUnit);
	double forceUnit = massUnit * accelerationUnit;
	double energyUnit = forceUnit * lengthUnit;
	double densityUnit = massUnit / (lengthUnit * lengthUnit * lengthUnit);
	double pressureUnit = forceUnit / (lengthUnit * lengthUnit);
	double temperatureUnit = energyUnit / massUnit;

	//Constants
	double gravitationalConstantInSI = 6.67430e-11;
	//Gravitational Constant is 1 

	//Conversion Functions to Unit Scale
	double length(double l) { return l / lengthUnit; }
	double mass(double m) { return m / massUnit; }
	double time(double t) { return t / timeUnit; }
	double velocity(double v) { return v / velocityUnit; }
	double acceleration(double a) { return a / accelerationUnit; }
	double force(double f) { return f / forceUnit; }
	double energy(double e) { return e / energyUnit; }
	double density(double d) { return d / densityUnit; }
	double pressure(double p) { return p / pressureUnit; }
	double temperature(double T) { return T / temperatureUnit; }

	//Conversion Functions to SI
	double lengthSI(double l) { return l * lengthUnit; }
	double massSI(double m) { return m * massUnit; }
	double timeSI(double t) { return t * timeUnit; }
	double velocitySI(double v) { return v * velocityUnit; }
	double accelerationSI(double a) { return a * accelerationUnit; }
	double forceSI(double f) { return f * forceUnit; }
	double energySI(double e) { return e * energyUnit; }
	double densitySI(double d) { return d * densityUnit; }
	double pressureSI(double p) { return p * pressureUnit; }
	double temperatureSI(double T) { return T * temperatureUnit; }
};