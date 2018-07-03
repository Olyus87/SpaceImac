#include "solarsystem.h"

SolarSystem::SolarSystem(const glimac::FilePath& texturesFolder)
	: m_sun(texturesFolder + "sunmap.jpg",	200000.f, 25.f*24.f,
			glm::vec3(1.f,0.7f,0.5f), 600.f)
{
	Satellite& mercury = m_sun.addSatellite("Mercury", Satellite(texturesFolder + "mercurymap.jpg",
																 4879.f, 1407.6f,
																 m_sun, 7.f, 88.f, 46.f, 69.8f));
	Satellite& venus = m_sun.addSatellite("Venus", Satellite(texturesFolder + "venusmap.jpg",
															 12104.f, -5832.5f,
															 m_sun, 3.4f, 224.7f, 107.1f, 108.9f));
	Satellite& earth = m_sun.addSatellite("Earth", Satellite(texturesFolder + "earthmap.jpg",
															 12756.f, 23.9f,
															 m_sun, 0.0f, 365.2f, 147.1f, 152.1f));
	Satellite& mars = m_sun.addSatellite("Mars", Satellite(texturesFolder + "marsmap.jpg",
														   6792.f, 24.6f,
														   m_sun, 1.9f, 687.f, 206.6f, 249.2f));
	Satellite& jupiter = m_sun.addSatellite("Jupiter", Satellite(texturesFolder + "jupitermap.jpg",
																 142984.f, 9.9f,
																 m_sun, 1.3f, 4331.f, 740.5f, 816.6f));
	Satellite& saturn = m_sun.addSatellite("Saturn", Satellite(texturesFolder + "saturnmap.jpg",
															   120536.f, 10.7f,
															   m_sun, 2.5f, 10747.f, 1352.6f, 1514.5f));
	Satellite& uranus = m_sun.addSatellite("Uranus", Satellite(texturesFolder + "uranusmap.jpg",
															   51118.f, -17.2f,
															   m_sun, 0.8f, 30589.f, 2741.3f, 3003.61f));
	Satellite& neptune = m_sun.addSatellite("Neptune", Satellite(texturesFolder + "neptunemap.jpg",
																 49528.f, 16.1f,
																 m_sun, 1.8f, 59800.f, 4444.5f, 4545.7f));
	Satellite& pluto = m_sun.addSatellite("Pluto", Satellite(texturesFolder + "plutomap.jpg",
															 2370.f, -153.3f,
															 m_sun, 17.2f, 90560.f, 4436.8f, 7375.9f));
	earth.addSatellite("Moon", Satellite(texturesFolder	+ "moonmap.jpg",
										 3470, 655.7,
										 earth, 5.1f, 27.3f, 3.63f, 4.06f));
	mars.addSatellite("Phobos", Satellite(texturesFolder + "moonmap.jpg",
										  910.f, 24,
										  mars, 1.08f, 0.31f, .9f, .9f));
	mars.addSatellite("Deimos", Satellite(texturesFolder + "moonmap.jpg",
										  510.f, 24,
										  mars, 1.79f, 1.26f, 2.3f, 2.3f));
}

const Star& SolarSystem::sun() const
{
	return m_sun;
}
