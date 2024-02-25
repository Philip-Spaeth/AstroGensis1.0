#include "Units.h"
#include <cmath>
#include <iostream>

Units::Units(double lUnit, double MUnit)
{
	lengthUnit = lUnit;
	massUnit = MUnit;

	//Time Unit is calculated so that the gravitational constant is 1
	timeUnit = std::sqrt(((lengthUnit * lengthUnit) * lengthUnit) / (massUnit * gravitationalConstantInSI));

	velocityUnit = lengthUnit / timeUnit;
	accelerationUnit = lengthUnit / (timeUnit * timeUnit);
	forceUnit = massUnit * accelerationUnit;
	energyUnit = forceUnit * lengthUnit;
	densityUnit = massUnit / (lengthUnit * lengthUnit * lengthUnit);
	pressureUnit = forceUnit / (lengthUnit * lengthUnit);
	temperatureUnit = energyUnit / massUnit;
}
