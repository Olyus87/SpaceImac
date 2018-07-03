#include "spaceelement.h"
#define GLM_FORCE_RADIANS // #rotate fonction taking degrees is deprecated
#include "glm/trigonometric.hpp"
#include "glm/ext.hpp"

SpaceElement::SpaceElement(glimac::FilePath texture, float diameter, float rotationPeriod)
	: m_diffuseTexture(texture), diameter(diameter), rotationPeriod(rotationPeriod)
{}

SpaceElement::SpaceElement(const SpaceElement &other)
	: m_diffuseTexture(other.m_diffuseTexture), diameter(other.diameter), rotationPeriod(other.rotationPeriod)
{}

SpaceElement::~SpaceElement()
{}

Satellite& SpaceElement::addSatellite(const std::string& name, const Satellite& satellite)
{
	satellites.insert(std::make_pair(name, std::make_unique<Satellite>(satellite)));
	return *satellites.at(name);
}

SpaceElement::SatellitesMap::const_iterator SpaceElement::firstSatellite() const
{
	return satellites.cbegin();
}

SpaceElement::SatellitesMap::const_iterator SpaceElement::lastSatellite() const
{
	return satellites.cend();
}

glm::vec3 SpaceElement::getColor() const
{
	return glm::vec3(1.f,1.f,1.f);
}

const glimac::FilePath &SpaceElement::diffuseTexture() const
{
	return m_diffuseTexture;
}

Satellite* SpaceElement::satellite(const std::string &name)
{
	try
	{
		Satellite* satellite = satellites.at(name).get();
		return satellite;
	}
	catch (const std::out_of_range&)
	{}
	return nullptr;
}

glm::vec3 SpaceElement::getPosition(float) const
{
	return glm::vec3(0,0,0);
}

glm::vec3 SpaceElement::getRotation(float days) const
{
	return glm::vec3(0, 0.1f*days*24.f*2.f*glm::pi<float>()/rotationPeriod, glm::pi<float>());
}

glm::vec3 SpaceElement::getSize() const
{
	return glm::vec3(diameter, diameter, diameter);
}

Satellite::Satellite(glimac::FilePath texture, float diameter, float rotationPeriod,
										 const SpaceElement &parent, float orbitalInclinaison, float orbitalPeriod,
										 float perihelion, float aphelion)
	: SpaceElement(texture, diameter, rotationPeriod), parent(parent), orbitalInclinaison(orbitalInclinaison),
		orbitalPeriod(orbitalPeriod), perihelion(perihelion), aphelion(aphelion)
{}

Satellite::Satellite(const Satellite &other)
	: SpaceElement(other), parent(other.parent), orbitalInclinaison(other.orbitalInclinaison),
		orbitalPeriod(other.orbitalPeriod), perihelion(other.perihelion), aphelion(other.aphelion)
{

}

glm::vec3 Satellite::getPosition(float days) const
{
	double angle = days * glm::pi<double>()*2./orbitalPeriod;
	glm::vec3 ellipsePosition	= glm::vec3(glm::cos<double>(angle)*perihelion*1000000.,
																			 0,
																			 glm::sin<double>(angle)*aphelion*1000000.);

	return parent.getPosition(days)
			+ glm::vec3(ellipsePosition.x*glm::cos<double>(glm::radians<double>(orbitalInclinaison)),
									ellipsePosition.x*glm::sin<double>(glm::radians<double>(orbitalInclinaison)),
									ellipsePosition.z);
}

Star::Star(glimac::FilePath texture, float diameter, float rotationPeriod,
					 glm::vec3 lightColor, float lightPower)
	: SpaceElement(texture, diameter, rotationPeriod),
		m_lightColor(lightColor), m_lightPower(lightPower)
{}

glm::vec3 Star::getColor() const
{
	return m_lightColor * (m_lightPower / 50.f);
}

glm::vec3 Star::lightColor() const
{
	return m_lightColor;
}

float Star::lightPower() const
{
	return m_lightPower;
}
