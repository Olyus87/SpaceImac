#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "spaceelement.h"

/**
 * @brief Container which generate the solar system "tree", considering the sun as the root
 */
class SolarSystem
{
public:
	/**
	 * @brief Create all planets with their satellites
	 * @param texturesFolder the path where we can find the planet texture
	 * (<planet_name>map.jpg, ex: earthmap.jpg)
	 */
	SolarSystem(const glimac::FilePath &texturesFolder);
	const Star& sun() const;

private:
	Star m_sun;
};

#endif // SOLARSYSTEM_H
