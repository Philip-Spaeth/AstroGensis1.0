#include "Units.h"
#include <cmath>
#include <iostream>

Units::Units(double lUnit, double MUnit, double TUnit)
{
	lengthUnit = lUnit;
	massUnit = MUnit;
	timeUnit = TUnit;
	velocityUnit = lengthUnit / timeUnit;
	accelerationUnit = lengthUnit / (timeUnit * timeUnit);
	forceUnit = massUnit * accelerationUnit;
	energyUnit = forceUnit * lengthUnit;
	densityUnit = massUnit / (lengthUnit * lengthUnit * lengthUnit);
	pressureUnit = forceUnit / (lengthUnit * lengthUnit);
	temperatureUnit = energyUnit / massUnit;
}

double Units::time(double t)
{
	return t / timeUnit;
}

double Units::calcG()
{
	double G_base = 6.67408e-11; // Gravitationskonstante in Basiseinheiten (m^3 kg^-1 s^-2)
	return (G_base * (std::pow(lengthUnit,3))) / (massUnit * std::pow(timeUnit, 1)); // Gravitationskonstante in Einheiten des Systems
}