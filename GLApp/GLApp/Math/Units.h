#pragma once

class Units
{
	public:
	Units() : lengthUnit(1e21), massUnit(1e40), timeUnit(1e12), velocityUnit(lengthUnit / timeUnit), accelerationUnit(lengthUnit / (timeUnit * timeUnit)), forceUnit(massUnit * accelerationUnit), energyUnit(forceUnit * lengthUnit), densityUnit(massUnit / (lengthUnit * lengthUnit * lengthUnit)), pressureUnit(forceUnit / (lengthUnit * lengthUnit)), temperatureUnit(energyUnit / massUnit), gravitationalConstant(6.67430e-11 * lengthUnit * lengthUnit * lengthUnit / (massUnit * timeUnit * timeUnit)) {}
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
	double gravitationalConstant = 6.67430e-11 * lengthUnit * lengthUnit * lengthUnit / (massUnit * timeUnit * timeUnit);
};