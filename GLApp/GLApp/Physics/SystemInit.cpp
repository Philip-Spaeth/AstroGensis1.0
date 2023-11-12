#include "SystemInit.h"
#include <iostream>
#include <cmath>
#include <random>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

void SystemInit::start(std::vector<std::vector<Particle>>& particles)
{
	//solarSystem(particles);

	ellipticalGalaxy(0, 999, { 0,0,0 }, { 0,300,0 }, { 0,0,0 }, particles);
	ellipticalGalaxy(1000, 1999, { 2e22, 0, 0}, { 90,0,180 }, { 0,0,0 } , particles);
	spiralGalaxy(2000, 2999, { 0,2e22,2e22 }, { 220, 17, 45}, { 0,0,0 }, particles);
	spiralGalaxy(3000, 3999, { 1.5e22, 0.5e22, 0 }, { 234,30,129 }, { 0,0,0 }, particles);
}

void SystemInit::spiralGalaxy(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1 - startIndex; 

	double diskRadius = 1e21;

	int numArms = 2;
	double armWidth = 0;
	double armDensity = 0.3;
	double interArmDensity = 1;

	double depth = diskRadius / 40;

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

			particle.position = glm::dvec3(r * std::cos(armAngle), r * std::sin(armAngle), physics.random(-depth, depth)) + position;
			particle.velocity = glm::dvec3(-v * std::sin(armAngle), v * std::cos(armAngle), 0) + velocity;
			particle.mass = 1e30;
			particle.radius = 0.01;
			//particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}
		i++;
	} 
}	

void SystemInit::ellipticalGalaxy(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1;

	double diskRadius = 1e21;

	double starSpeed = 1;

	double depth = diskRadius / 40;

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


			double r = ((diskRadius / size) * i);

			double angle = physics.random(0, 2 * 3.14);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)
			//double v = 0;

			double v = std::sqrt((physics.G * particles[0][startIndex].mass) / r) * starSpeed;


			particles[0][j].position = glm::dvec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth , depth)) + position;
			particles[0][j].velocity = glm::dvec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
			particles[0][j].mass = 1e30;
			particles[0][j].radius = 0.01;
			particle.color = glm::vec3(1, 1, 1);
		}
		i++;
	}
}


void SystemInit::solarSystem(std::vector<std::vector<Particle>>& particles)
{
	/*
	particles[0][0].position = { -100, 0, 0 };
	particles[0][0].velocity = { 0, 0, 0 };
	particles[0][0].mass = 1e20;
	particles[0][0].radius = 2;
	particles[0][0].color = glm::vec3(1, 1, 1);

	//Particle 1
	particles[0][1].position = { 100, 0, 0 };
	particles[0][1].velocity = { 0, 0, 0 };
	particles[0][1].mass = 1e20;
	particles[0][1].radius = 2;
	particles[0][1].color = glm::vec3(1, 1, 1);
	*/
	
	// Data from JPL Horizons Nasa date: 2020-01-01 https://ssd.jpl.nasa.gov/horizons/app.html#/

	Physics physics;

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

		//Sun
		if (j == 1)
		{
			Particle particle;
			particle.position = glm::vec3(-5.683718842782438E+08, 1.112952208755958E+09, 3.459173372169666E+06);
			particle.velocity = glm::vec3(-1.446153155910682E-05, -3.447513466324272E-06, 3.988450015258638E-07);
			particle.mass = 1.989e30;
			particle.radius = 3;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}

		//Mercury
		if (j == 2)
		{
			Particle particle;
			particle.position = glm::vec3(-1.004313454665499E+010, -6.782852744259514E+010, -4.760875668975301E+09);
			particle.velocity = glm::vec3(3.847265152100766E+04, 4.158689617302953E+03, -3.869763814829368E+03);
			particle.mass = 3.285e23;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//Venus
		if (j == 3)
		{
			Particle particle;
			particle.position = glm::vec3(1.076208530906284E+011, 8.974077731382189E+09, -6.131962732360962E+09);
			particle.velocity = glm::vec3(-2.693485036366443E+03, 3.476650461674002E+04, 6.320912270841283E-04);
			particle.mass = 4.867e24;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}

		//Earth
		if (j == 4)
		{
			Particle particle;
			particle.position = glm::vec3(-2.545334341413143E+010, 1.460912993606863E+011, -2.712536257669330E+06);
			particle.velocity = glm::vec3(-2.986338200299215E+04, -5.165822218319680E+03, 1.135505005904092E-06);
			particle.mass = 5.972e24;
			particle.radius = 2;
			particle.color = glm::vec3(0, 0, 1);
			particles[0][j] = particle;
		}

		//Mars
		if (j == 5)
		{
			Particle particle;
			particle.position = glm::vec3(-1.980536587315298E+11, -1.313944783612352E+11, 2.072259408826552E+09);
			particle.velocity = glm::vec3(1.439273929147519E+04, -1.805004075338039E+04, -7.312486307777304E-04);
			particle.mass = 6.39e23;
			particle.radius = 1;
			particle.color = glm::vec3(1, 0, 0);
			particles[0][j] = particle;
		}

		//Jupiter
		if (j == 6)
		{
			Particle particle;
			particle.position = glm::vec3(7.814210740177372E+10, -7.769489405106664E+11, 1.474081608655989E+09);
			particle.velocity = glm::vec3(1.283931035365873E+04, 1.930357075733133E+03, -2.952599466798547E-04);
			particle.mass = 1.898e27;
			particle.radius = 2;
			particle.color = glm::vec3(194, 152, 0);
			particles[0][j] = particle;
		}

		//Saturn
		if (j == 7)
		{
			Particle particle;
			particle.position = glm::vec3(5.674913734259648E+11, -1.388366508315161E+12, 1.549279065912783E+09);
			particle.velocity = glm::vec3(8.406493534983737E+03, 3.627774825767542E+03, -1.778058503736513E-04);
			particle.mass = 5.683e26;
			particle.radius = 2;
			particle.color = glm::vec3(194, 152, 0);
			particles[0][j] = particle;
		}

		//Uranus
		if (j == 8)
		{
			Particle particle;
			particle.position = glm::vec3(2.426731459899411E+012, 1.703392613402010E+12, -2.511227721322751E+10);
			particle.velocity = glm::vec3(-3.962351347249518E+03, 5.256523406016862E+03, 7.095188573163291E-05);
			particle.mass = 8.681e25;
			particle.radius = 1;
			particle.color = glm::vec3(150, 150, 150);
			particles[0][j] = particle;
		}

		//Neptune
		if (j == 9)
		{
			Particle particle;
			particle.position = glm::vec3(4.374094185858724E+12, 9.514046294520888E+011, -8.121285263483042E+010);
			particle.velocity = glm::vec3(1.119118833334523E+03, 5.343445550714678E+03, -1.357620460013313E-04);
			particle.mass = 1.024e26;
			particle.radius = 1;
			particle.color = glm::vec3(0, 0, 1);
			particles[0][j] = particle;
		}
	}

	/*
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
	}*/
}