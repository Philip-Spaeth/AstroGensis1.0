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

	G = gravitationalConstantInSI * ((lengthUnit * lengthUnit * lengthUnit) / (massUnit * (timeUnit * timeUnit)));
}
