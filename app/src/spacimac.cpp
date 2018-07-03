#include "spacimac.h"

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>

#include "scene.h"
#include "renderer.h"
#include "camera.h"

SpacImac* SpacImac::m_instance = nullptr;

SpacImac::SpacImac(int argc, char **argv, const std::string& title, bool fullscreen)
	: path(argv[0]), done(false), ratio(16.f/9.f), sizeScale(0.0001f), distanceScale(0.000001f),
		width(754), height(512),
		timeSpeed(1), lastTimeSpeed(1), timeStep(1),
		currentCamera(0), solarSystem(path.dirPath() + "assets")
{
	if(0 != SDL_Init(SDL_INIT_VIDEO)) {
			std::cerr << SDL_GetError() << std::endl;
			return;
	}
	Uint32 videoflags = SDL_OPENGL | SDL_DOUBLEBUF;
	if (fullscreen)
		videoflags |= SDL_FULLSCREEN;
	else
		videoflags |= SDL_RESIZABLE;
	if(!SDL_SetVideoMode(width, height, 32, videoflags)) {
			std::cerr << SDL_GetError() << std::endl;
			return;
	}
	SDL_WM_SetCaption(title.c_str(), nullptr);

	GLenum glewInitError = glewInit();
	if(GLEW_OK != glewInitError) {
		std::cerr << glewGetErrorString(glewInitError) << std::endl;
	}
	m_instance = this;
}

SpacImac::SpacImac(int argc, char **argv, const std::string &title, uint width, uint height)
	: path(argv[0]), done(false), ratio(16.f/9.f), sizeScale(0.0001f), distanceScale(0.000001f),
		width(width), height(height),
		timeSpeed(1), lastTimeSpeed(1), timeStep(1),
		currentCamera(0), solarSystem(path.dirPath() + "assets")
{
	if(0 != SDL_Init(SDL_INIT_VIDEO)) {
			std::cerr << SDL_GetError() << std::endl;
			return;
	}
	Uint32 videoflags = SDL_OPENGL | SDL_DOUBLEBUF;
	if(!SDL_SetVideoMode(width, height, 32, videoflags)) {
			std::cerr << SDL_GetError() << std::endl;
			return;
	}
	SDL_WM_SetCaption(title.c_str(), nullptr);

	GLenum glewInitError = glewInit();
	if(GLEW_OK != glewInitError) {
		std::cerr << glewGetErrorString(glewInitError) << std::endl;
	}
	m_instance = this;
}

SpacImac::~SpacImac()
{
	SDL_Quit();
}

int SpacImac::exec()
{
	SDL_Event e;
	Uint32 start, end;
	initialize();
	while(!done)
	{
		start = SDL_GetTicks();
		while (SDL_PollEvent(&e)) {
			handleEvent(e);
		}
		render();
		SDL_GL_SwapBuffers();
		end = SDL_GetTicks();
		if (end - start < 30)
			SDL_Delay(30 - (end - start));
		update((SDL_GetTicks() - start) * 0.001f);
	}
	return EXIT_SUCCESS;
}

void SpacImac::update(float deltaTime)
{
	std::cout << "Frame n:" << frame << " Delta time:" << deltaTime << std::endl;

	cameras[currentCamera]->update(deltaTime);
	std::for_each(solarSystemMeshes.begin(), solarSystemMeshes.end(),
								[this](std::pair<Instance*, const SpaceElement*> solarElement)
	{
		updateSpaceElementMesh(solarElement);
	});
	time += deltaTime * timeSpeed;
	++frame;
}

void SpacImac::render() const
{
	glClearColor(0.05,0.05,0.05,1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	if (skyRenderer.get())
	{
		skyRenderer->render(m_scene, *cameras[currentCamera]);
	}
	if (renderer)
	{
		renderer->render(m_scene, *cameras[currentCamera]);
	}
}

void SpacImac::setRenderer(Renderer* renderer)
{
	this->renderer = renderer;
	if (renderer)
		renderer->initialize();
}

std::string SpacImac::getFilePath(const std::string& file) const
{
	return path.dirPath() + file;
}

uint SpacImac::viewWidth() const
{
	return m_viewWidth;
}

uint SpacImac::viewHeight() const
{
	return m_viewHeight;
}

Scene &SpacImac::scene()
{
	return m_scene;
}

SpacImac *SpacImac::instance()
{
	if (!m_instance)
		throw std::runtime_error("There is no initialized instance of SpacImac");
	return m_instance;
}

void SpacImac::resize(uint width, uint height)
{
	Uint32 videoflags = SDL_OPENGL | SDL_DOUBLEBUF | SDL_RESIZABLE;
	if(!SDL_SetVideoMode(width, height, 32, videoflags)) {
			std::cerr << SDL_GetError() << std::endl;
			return;
	}

	glViewport(0,0,width, height);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	this->height = height;
	this->width = width;
	m_viewWidth = width;
	m_viewHeight = width * 1.f/ratio;
	if(m_viewHeight>height){
		m_viewHeight = height;
		m_viewWidth = height * ratio;
	}
	m_viewX = width/2 - m_viewWidth/2;
	m_viewY = height/2 - m_viewHeight/2;

	glViewport(m_viewX, m_viewY, m_viewWidth, m_viewHeight);
}

void SpacImac::addSpaceElementMesh(const SpaceElement &element, uint sphereMeshId)
{
	uint textureId = m_scene.addTexture(element.diffuseTexture());
	Instance& mesh = m_scene.makeInstance(sphereMeshId);
	mesh.transform.position = element.getPosition(0) * distanceScale;
	mesh.transform.scale = element.getSize() * sizeScale;
	mesh.transform.rotation = element.getRotation(0);
	mesh.materialId = m_scene.addMaterial(Material(element.getColor(), textureId));
	solarSystemMeshes.push_back(std::pair<Instance*, const SpaceElement*>(&mesh, &element));

	std::for_each(element.firstSatellite(), element.lastSatellite(),
	[this, sphereMeshId](const std::pair<const std::string, std::unique_ptr<Satellite> >& satellite){
		addSpaceElementMesh(*satellite.second, sphereMeshId);
	});
}

void SpacImac::initialize()
{
	resize(width,	height);

	skyRenderer = std::make_unique<SkyboxRenderer>();
	skyRenderer->initialize();

	// Skybox
	glimac::Geometry cube;
	cube.loadOBJ(getFilePath("assets/cube.obj"), getFilePath("assets"));
	m_scene.setSkybox(cube, getFilePath("assets/starfield"));

	// Planets
	glimac::Geometry sphere;
	sphere.loadOBJ(getFilePath("assets/sphere.obj"),getFilePath("assets"));
	uint sphereId = m_scene.addMeshes(sphere);

	addSpaceElementMesh(solarSystem.sun(), sphereId);

	m_scene.directionalLight.power = 0.1f;
	m_scene.directionalLight.color = glm::vec3(1.f,1.f,1.f);
	m_scene.pointLight.position = solarSystem.sun().getPosition(0);
	m_scene.pointLight.color = solarSystem.sun().lightColor();
	m_scene.pointLight.power = solarSystem.sun().lightPower();
	m_scene.initializeBuffers();

	cameras.push_back(std::make_unique<OrbitalCamera>());
	OrbitalCamera* oc = dynamic_cast<OrbitalCamera*>(cameras.back().get());
	oc->distance = 55.0f;
	oc->near = 1.f;
	oc->far = 20000.f;
	oc->pitch = -3.f * glm::pi<float>() / 8.f;
	oc->translationAcc = (oc->far - oc->near) / 150.f;
	oc->rotationAcc = 0;

	cameras.push_back(std::make_unique<OrbitalCamera>());
	oc = dynamic_cast<OrbitalCamera*>(cameras.back().get());
	oc->distance = 55.0f;
	oc->near = 1.f;
	oc->far = 20000.f;
	oc->pitch = -glm::pi<float>() / 16.f;
	oc->translationAcc = (oc->far - oc->near) / 150.f;

	cameras.push_back(std::make_unique<TargetCamera>(++solarSystemMeshes.cbegin(),
																									 solarSystemMeshes.cend()));
	oc = dynamic_cast<OrbitalCamera*>(cameras.back().get());
	oc->distance = 20.0f;
	oc->near = 0.5f;
	oc->far = 1000.f;
	oc->pitch = 2.f * glm::pi<float>() / 8.f;
	oc->translationAcc = (oc->far - oc->near) / 200.f;

	time = 0;
	frame = 0;
}

void SpacImac::updateSpaceElementMesh(std::pair<Instance*, const SpaceElement*> solarElement)
{
	solarElement.first->transform.position = solarElement.second->getPosition(time) * distanceScale;
	solarElement.first->transform.scale = solarElement.second->getSize() * sizeScale;
	solarElement.first->transform.rotation = solarElement.second->getRotation(time);
}

void SpacImac::handleEvent(const SDL_Event& e)
{
	if (e.type == SDL_QUIT) {
		done = true;
	}
	else if (e.type == SDL_KEYUP) {
		switch(e.key.keysym.sym){
		case SDLK_ESCAPE:
			done = true;
			break;
		case SDLK_p:
			if (std::abs(timeSpeed) > 0.01)
			{
				lastTimeSpeed = timeSpeed;
				timeSpeed = 0;
			}
			else
			{
				timeSpeed = lastTimeSpeed;
			}
			break;
		case SDLK_RIGHT:
		case SDLK_LEFT:
			timeSpeed = lastTimeSpeed;
			break;
		case SDLK_UP:
			time += timeStep;
			break;
		case SDLK_DOWN:
			time -= timeStep;
			break;
		case SDLK_TAB:
			currentCamera++;
			currentCamera = currentCamera % cameras.size();
			break;
		default:
			break;
		}
	}
	else if (e.type == SDL_KEYDOWN) {
		switch(e.key.keysym.sym){
		case SDLK_RIGHT:
			if (std::abs(timeSpeed) < 0.01f)
				timeSpeed = lastTimeSpeed;
			lastTimeSpeed = timeSpeed;
			timeSpeed *= 3;
			break;
		case SDLK_LEFT:
			if (std::abs(timeSpeed) < 0.01f)
				timeSpeed = lastTimeSpeed;
			lastTimeSpeed = timeSpeed;
			timeSpeed *= -3;
			break;
		break;
		default:
			break;
		}
	}
	else if (e.type == SDL_VIDEORESIZE)
	{
		resize(e.resize.w, e.resize.h);
	}
	cameras[currentCamera]->handleEvent(e);
}

TargetCamera::TargetCamera(SpacImac::SpaceElementMeshes::const_iterator start,
													 SpacImac::SpaceElementMeshes::const_iterator end)
	: start(start), end(end), current(start)
{
	updateDistance();
}

void TargetCamera::handleEvent(const SDL_Event &e)
{
	OrbitalCamera::handleEvent(e);
	if (e.type == SDL_KEYUP) {
		switch(e.key.keysym.sym){
		case SDLK_SPACE:
			current++;
			if (current == end)
				current = start;
			updateDistance();
			break;
		default:
			break;
		}
	}
}

void TargetCamera::update(float deltaTime)
{
	OrbitalCamera::update(deltaTime);
	target = current->first->transform.position;
}

void TargetCamera::updateDistance()
{
	distance = glm::length(current->second->getSize()) * 0.0002f;
	near = glm::length(current->second->getSize()) * 0.00002f;
	far = glm::length(current->second->getSize()) * 2.f;
}
