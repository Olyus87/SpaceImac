#ifndef SPACEELEMENT_H
#define SPACEELEMENT_H

#include "glimac/FilePath.hpp"
#include "glm/common.hpp"

#include <map>
#include <memory>

class Satellite;

/**
 * @brief The model structure describing a spherical element with satellites
 */
class SpaceElement
{
public:
	typedef std::map<std::string, std::unique_ptr<Satellite> > SatellitesMap;

	/**
	 * @param texture The diffuse texture map of the planet
	 * @param diameter The diameter in km
	 * @param rotationPeriod The rotation period which is the time in hours
	 * when the planet go back to its initial rotation
	 */
	SpaceElement(glimac::FilePath texture, float diameter, float rotationPeriod);
	SpaceElement(const SpaceElement& other);
	virtual ~SpaceElement();

	/**
	 * @brief add satellite in the map with name as key
	 * @return the created Satellite
	 */
	Satellite& addSatellite(const std::string& name, const Satellite& satellite);
	/**
	 * @return the start map iterator allowing iteration through the satellites map
	 */
	SatellitesMap::const_iterator firstSatellite() const;
	/**
	 * @return the end map iterator allowing iteration through the satellites map
	 */
	SatellitesMap::const_iterator lastSatellite() const;

	/**
	 * @return the diffuse color
	 */
	virtual glm::vec3 getColor() const;
	/**
	 * @return the diffuse texture path
	 */
	const glimac::FilePath& diffuseTexture() const;
	/**
	 * @return the satellite given by name
	 */
	Satellite* satellite(const std::string& name);

	/**
	 * @return the position in km according to the time in days
	 */
	virtual glm::vec3 getPosition(float days) const;
	/**
	 * @return the rotation of the planet around itself in radian according to the time in days
	 */
	virtual glm::vec3 getRotation(float days) const;
	/**
	 * @return the size in km
	 */
	virtual glm::vec3 getSize() const;

private:
	glimac::FilePath m_diffuseTexture;
	float diameter; // km
	float rotationPeriod; // hours
	SatellitesMap satellites;
};

/**
 * @brief A SpaceElement which turn around another SpaceElement
 */
class Satellite : public SpaceElement
{
public:
	/**
	 * @param texture The diffuse texture map of the planet
	 * @param diameter The diameter in km
	 * @param rotationPeriod The rotation period which is the time in hours
	 *  when the planet go back to its initial rotation
	 * @param parent The SpaceElement the Satellite turn around
	 * @param orbitalInclinaison Inclinaison of the orbital plane (rotation around the z-axis)
	 * @param orbitalPeriod The orbital period which is the time in hours
	 *  when the planet go back to its initial position around its parent
	 * @param perihelion the closest distance from the parent
	 * @param aphelion the farest distance from the parent
	 */
	Satellite(glimac::FilePath texture, float diameter, float rotationPeriod,
						const SpaceElement& parent, float orbitalInclinaison, float orbitalPeriod,
						float perihelion, float aphelion);
	Satellite(const Satellite& other);

	virtual glm::vec3 getPosition(float days) const; // depending on days, position in km
private:
	const SpaceElement& parent;
	float orbitalInclinaison; // degrees
	float orbitalPeriod; // days
	float perihelion; // 10^6 km
	float aphelion; // 10^6 km
};

typedef Satellite Planet;

/**
 * @brief The SpaceElement which emits light (we considering a point light)
 */
class Star : public SpaceElement
{
public:
	Star(glimac::FilePath texture, float diameter, float rotationPeriod,
			 glm::vec3 m_lightColor, float m_lightPower);

	virtual glm::vec3 getColor() const;
	glm::vec3 lightColor() const;
	float lightPower() const;

private:
	glm::vec3 m_lightColor;
	float m_lightPower;
};

#endif // SPACEELEMENT_H
