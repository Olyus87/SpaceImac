#ifndef SPACIMAC_H
#define SPACIMAC_H

#include <string>
#include <glimac/FilePath.hpp>

#include "camera.h"
#include "renderer.h"
#include "scene.h"
#include "solarsystem.h"

/**
 * @brief Master class which connect the Solar system model with the Opengl view
 */
class SpacImac
{
public:
	typedef std::list<std::pair<Instance*, const SpaceElement*>> SpaceElementMeshes;

	SpacImac(int argc, char** argv, const std::string& title, bool fullscreen=false);
	SpacImac(int argc, char** argv, const std::string& title, uint width, uint height);
	~SpacImac();

	/**
	 * @brief Main loop which handle event, solar system update and frame drawing
	 * @return the application exit code
	 */
	int exec();
	/**
	 * @brief resize handler
	 */
	void resize(uint width, uint height);

	/**
	 * @brief Make a mesh instance from the element and the mesh given by sphereMeshId
	 */
	void addSpaceElementMesh(const SpaceElement& element, uint sphereMeshId);
	/**
	 * @brief Initialize meshes, sky and cameras
	 */
	void initialize();
	/**
	 * @brief Update the SpaceElementMesh Transform according to the time
	 */
	void updateSpaceElementMesh(std::pair<Instance*, const SpaceElement*> solarElement);
	/**
	 * @brief Handle SDL_Event
	 */
	void handleEvent(const SDL_Event& event);
	/**
	 * @brief update the time and all objects
	 */
	void update(float deltaTime);
	/**
	 * @brief call renderer
	 */
	void render() const;

	void setRenderer(Renderer* renderer);

	/**
	 * @brief Get the path of file using the working path (path of the application executable)
	 */
	std::string getFilePath(const std::string& file) const;

	uint viewWidth() const;
	uint viewHeight() const;
	Scene& scene();

	static SpacImac* instance();
private:
	glimac::FilePath path;

	bool done;

	float ratio;
	float sizeScale, distanceScale;
	uint width, height;
	uint m_viewX, m_viewY;
	uint m_viewWidth, m_viewHeight;
	uint frame;

	float time; // s
	float timeSpeed;
	float lastTimeSpeed;
	float timeStep;

	std::unique_ptr<SkyboxRenderer> skyRenderer;
	Renderer* renderer;
	std::vector<std::unique_ptr<BaseCamera>> cameras;
	int currentCamera;
	Scene m_scene;
	SolarSystem solarSystem;
	SpaceElementMeshes solarSystemMeshes;

	static SpacImac* m_instance;
};

/**
 * @brief OrbitalCamera wich target a specific planet among a list given by a start iterator and an end iterator
 */
class TargetCamera : public OrbitalCamera
{
public:
	TargetCamera(SpacImac::SpaceElementMeshes::const_iterator start,
							 SpacImac::SpaceElementMeshes::const_iterator end);

	virtual void handleEvent(const SDL_Event& e);
	virtual void update(float deltaTime);

	/**
	 * @brief Update the distance according to the target size
	 */
	void updateDistance();
private:
	SpacImac::SpaceElementMeshes::const_iterator start, end, current;
};

#endif // SPACIMAC_H
