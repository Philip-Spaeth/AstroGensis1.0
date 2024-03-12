#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <glm.hpp>
#ifdef WIN32
#include <gtc\constants.hpp>
#else
#include <gtc/constants.hpp>
#endif

class MathFunctions
{
public:
	MathFunctions(){};

	//SPH Kernels
	static double cubicSplineKernel(double r, double h);
	static double laplaceCubicSplineKernel(const glm::dvec3& rVec, double h);
	static glm::dvec3 gradientCubicSplineKernel(const glm::dvec3& r, double h);
	//SPH functions
	static double tempretureToInternalEnergy(double tempreture);

	//random Functions
	static void setRandomSeed(unsigned int seed);
	static double random(double min, double max);
	static double gaussianRandom(double mean = 0.0, double stddev = 1.0);
};

#endif