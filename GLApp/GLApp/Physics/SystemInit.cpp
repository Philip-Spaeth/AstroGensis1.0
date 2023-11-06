#include "SystemInit.h"
#include <iostream>
#include <cmath>
#include <random>

void SystemInit::start(std::vector<std::vector<Particle>>& particles)
{
	solarSystem(particles);

	//ellipticalGalaxy(0, 999, { 0,0,0 }, { 0,0,0 }, particles);
	//ellipticalGalaxy(1000, 1999, { 2e22, 0, 0}, { 0,0,0 }, particles);
	//spiralGalaxy(0, 999, { 0,0,0 }, { 0, 0, 0}, particles);
	//spiralGalaxy(1000, 1999, { 1.5e22, 0.5e22, 0 }, { 0,0,0 }, particles);
}

void SystemInit::spiralGalaxy(int startIndex, int endIndex, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1 - startIndex; 

	double diskRadius = 1e21;

	int numArms = 2;
	double armWidth = 0;
	double armDensity = 0.3;
	double interArmDensity = 1;

	double depth = 1e20;

	int i = 0;

	// Erstellen einer Spiralgalaxie
	for (int j = startIndex; j != endIndex; j++)
	{
		// Schwarzes Loch in der Mitte
		if (j == startIndex)
		{
			Particle particle;
			particle.position = position;
			particle.velocity = velocity;
			particle.mass = 1e36;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][startIndex] =  particle;
		}

		// Sterne in Spiralarmen um das schwarze Loch
		else
		{
			Particle particle;

			double r = physics.random(0,diskRadius/3);

			r = r * std::exp(1 * r / diskRadius);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)
			double v = std::sqrt((physics.G * particles[0][startIndex].mass) / r);

			double armAngle = 2 * 3.14 * (j - startIndex) / numArms; // Winkel für die Anzahl der Arme

			particle.position = glm::vec3(r * std::cos(armAngle), r * std::sin(armAngle), physics.random(-depth, depth)) + position;
			particle.velocity = glm::vec3(-v * std::sin(armAngle), v * std::cos(armAngle), 0) + velocity;
			particle.mass = 1e30;
			particle.radius = 0.01;
			//particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}
		i++;
	} 
}	

void SystemInit::ellipticalGalaxy(int startIndex, int endIndex, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1;

	double diskRadius = 1e21;

	double starSpeed = 1;

	double depth = 1e20;

	int i = 0;

	// Erstellen einer Spiralgalaxie
	for (int j = startIndex; j < size; j++)
	{
		// Mass Sagittarius A
		if (j == startIndex)
		{
			particles[0][j].position = position;
			particles[0][j].velocity = velocity;
			particles[0][j].mass = 1e36;
			particles[0][j].radius = 2;
			particles[0][j].color = glm::vec3(1, 1, 1);
		}

		//stars like our sun
		else
		{
			Particle particle;
			double r = (diskRadius / size) * i;
			double angle = physics.random(0, 2 * 3.14);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)
			//double v = 0;

			double v = std::sqrt((physics.G * particles[0][startIndex].mass) / r) * starSpeed;

			particles[0][j].position = glm::vec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth , depth)) + position;
			particles[0][j].velocity = glm::vec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
			particles[0][j].mass = 1e30;
			particles[0][j].radius = 0.01;
			particle.color = glm::vec3(1, 1, 1);
		}
		i++;
	}
	//set the other particles to 0
	for (int j = endIndex; j < particles.size(); j++)
	{
		Particle particle;
		particle.position = glm::vec3(1e20, 0, 0);
		particle.velocity = glm::vec3(0, 0, 0);
		particle.mass = 0;
		particle.radius = 0;
		particle.color = glm::vec3(0, 0, 0);
		particles[0][j] = particle;
	}
}


void SystemInit::solarSystem(std::vector<std::vector<Particle>>& particles)
{
	
	Physics physics;

	double distanceFaktor = 1.4848e11;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//barycenter
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 0;
			particle.radius = 0.5;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//One sun with a lot of mass
		if (j == 1)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 5.972e24;
			particle.radius = 7;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}
		if(j == 2)
		{
			Particle particle;
			particle.position = glm::vec3(j * distanceFaktor, 0,0);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)

			double v = std::sqrt((physics.G * particles[0][0].mass) / (j * distanceFaktor));

			particle.velocity = glm::vec3(0, v, 0);
			particle.mass = 5.972e24;

			//diffrent color for every planet

			int color = j % 3;
			if (color == 0)
			{
				particle.color = glm::vec3(1, 0, 0);
			}
			else if (color == 1)
			{
				particle.color = glm::vec3(0, 0, 1);
			}
			else if (color == 2)
			{
				particle.color = glm::vec3(0, 1, 0);
			}

			//diffrent size for every planet
			int size = j % 3;
			if (size == 0)
			{
				particle.radius = 2;
			}
			else if (size == 1)
			{
				particle.radius = 6;
			}
			else if (size == 2)
			{
				particle.radius = 4;
			}
			particles[0][j] = particle;
		}
	}
	
	/*
	// Data from JPL Horizons Nasa date: 2023-11-03 https://ssd.jpl.nasa.gov/horizons/app.html#/

	Physics physics;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//barycenter
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0,0,0);
			particle.velocity = glm::vec3(0,0,0);
			particle.mass = 0;
			particle.radius = 0.5;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//Sun
		if (j == 1)
		{
			Particle particle;
			particle.position = glm::vec3(-1.232889413340201E+09, -3.701008409382875E+08, 3.182190337972944E+07);
			particle.velocity = glm::vec3(7.553656090601099E-06, -1.314542894579846E-05, -5.980701970240924E-08);
			particle.mass = 1.989e30;
			particle.radius = 3;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}
		
		//Mercury
		if (j == 2)
		{
			Particle particle;
			particle.position = glm::vec3(-2.833890574079202E+10, -6.433544070413450E+10, -2.709210365528632E+09);
			particle.velocity = glm::vec3(3.506217279457973E+04, -1.663184147799844E+04, -4.573514934938731E+03);
			particle.mass = 3.285e23;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//Venus
		if (j == 3)
		{
			Particle particle;
			particle.position = glm::vec3(-1.974525542125584E+09, 1.072748929350662E+011, 1.552945401597492E+09);
			particle.velocity = glm::vec3(-3.513093389909205E+04, -4.398265789814597E-04, 2.021598070855121E+03);
			particle.mass = 4.867e24;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//Earth
		if (j == 4)
		{
			Particle particle;
			particle.position = glm::vec3(1.125180426713600E+011, 9.498467179386520E+010, 2.536544014597684E+07);
			particle.velocity = glm::vec3(-1.960323703173124E+04, 2.270532953792154E+04, -1.453550850493812E-06);
			particle.mass = 5.972e24;
			particle.radius = 2;
			particle.color = glm::vec3(0, 0, 1);
			particles[0][j] = particle;
		}

		//Mars
		if (j == 5)
		{
			Particle particle;
			particle.position = glm::vec3(-1.583021045070701E+11, -1.722955812925235E+11, 2.815284531350955E+08);
			particle.velocity = glm::vec3(1.880510937072378E+04 , -1.428351853907849E+04, -7.602208426060137E-04);
			particle.mass = 6.39e23;
			particle.radius = 1;
			particle.color = glm::vec3(1,0,0);
			particles[0][j] = particle;
		}

		//Jupiter
		if (j == 6)
		{
			Particle particle;
			particle.position = glm::vec3(5.674157345283098E+011, 4.794451702492975E+011, -1.468376033100995E+011);
			particle.velocity = glm::vec3(-8.577094730753281E+03, 1.059915652873620E+04, 1.479940080679594E-04);
			particle.mass = 1.898e27;
			particle.radius = 2;
			particle.color = glm::vec3(194, 152, 0);
			particles[0][j] = particle;
		}

		//Saturn
		if (j == 7)
		{
			Particle particle;
			particle.position = glm::vec3(1.327777402825330E+012, -6.014482395672525E+011, -4.240741994847396E+010);
			particle.velocity = glm::vec3(3.446783812617688E+03, 8.780996507066352E+03, -2.903989498217112E-04);
			particle.mass = 5.683e26;
			particle.radius = 2;
			particle.color = glm::vec3(194, 152, 0);
			particles[0][j] = particle;
		}

		//Uranus
		if (j == 8)
		{
			Particle particle;
			particle.position = glm::vec3(1.861737036150919E+012, 2.268205049707670E+012, -1.569495020045030E+010);
			particle.velocity = glm::vec3(-5.313649027682310E+03, 4.003224261836031E+03, 8.368147455424935E-05);
			particle.mass = 8.681e25;
			particle.radius = 1;
			particle.color = glm::vec3(150, 150, 150);
			particles[0][j] = particle;
		}
		
		//Neptune
		if (j == 9)
		{
			Particle particle;
			particle.position = glm::vec3(4.461688987151100E+012, -2.961660800756106E+011, -9.672524124557048E+010);
			particle.velocity = glm::vec3(3.239880117859616E-04, 5.455536775178176E+03, -1.197895876486910E-04);
			particle.mass = 1.024e26;
			particle.radius = 1;
			particle.color = glm::vec3(0, 0, 1);
			particles[0][j] = particle;
		}
	}
	*/
}