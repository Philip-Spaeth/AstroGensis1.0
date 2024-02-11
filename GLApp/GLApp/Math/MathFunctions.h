#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <glm.hpp>
#include <gtc\constants.hpp>

class MathFunctions
{
public:
	MathFunctions(){};

	//SPH Kernels
	static double cubicSplineKernel(double r, double h);
	static double laplaceCubicSplineKernel(const glm::dvec3& rVec, double h);

	//random Functions
	static void setRandomSeed(unsigned int seed);
	static double random(double min, double max);
	static double gaussianRandom(double mean = 0.0, double stddev = 1.0);
};

#endif