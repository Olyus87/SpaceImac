#include "camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/gtx/euler_angles.hpp"

BaseCamera::BaseCamera()
	: FoV(70), near(0.1), far(100)
{}

void BaseCamera::handleEvent(const SDL_Event &e)
{}

void BaseCamera::update(float deltaTime)
{}

glm::mat4 BaseCamera::getViewMatrix() const
{
	return glm::mat4(1.0f);
}

glm::mat4 BaseCamera::getProjectionMatrix(float viewWidth, float viewHeight) const
{
	return glm::perspective(glm::radians(FoV), viewWidth / viewHeight, near, far);
}

OrbitalCamera::OrbitalCamera()
	: target{0,0,0}, distance(1), translationAcc(0.2f), translationSpeed(0),
		pitch(0), yaw(glm::pi<double>()/2.0), roll(0), rotationAcc(0.2),
		speedPitch(0), speedYaw(0)
{}

/**
 * set a translation and/or a rotation speed according to inputs
 * note: setting a speed rather than the position give smooth moves
 */
void OrbitalCamera::handleEvent(const SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN){
		switch(e.key.keysym.sym){
		case SDLK_q:
			speedYaw = rotationAcc;
			break;
		case SDLK_z:
			speedPitch = rotationAcc;
			break;
		case SDLK_s:
			speedPitch = -rotationAcc;
			break;
		case SDLK_d:
			speedYaw = -rotationAcc;
			break;
		default:
			break;
		}

	}
	if (e.type == SDL_KEYUP){
		Uint8* keyState = SDL_GetKeyState(nullptr);
		switch(e.key.keysym.sym){
		case SDLK_q:
			if(!keyState[SDLK_q])
				speedYaw = 0;
			break;
		case SDLK_z:
			if(!keyState[SDLK_z])
				speedPitch = 0;
			break;
		case SDLK_d:
			if(!keyState[SDLK_d])
				speedYaw = 0;
			break;
		case SDLK_s:
			if(!keyState[SDLK_s])
				speedPitch = 0;
			break;
		default:
			break;
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		if (e.button.button == SDL_BUTTON_WHEELDOWN)
		{
			translationSpeed += translationAcc;
		}
		else if (e.button.button == SDL_BUTTON_WHEELUP)
		{
			translationSpeed -= translationAcc;
		}
	}
}

/**
 * update the distance and rotations according to speed
 */
void OrbitalCamera::update(float deltaTime)
{
	pitch += speedPitch * deltaTime;
	yaw += speedYaw * deltaTime;
	distance += translationSpeed * deltaTime;
	translationSpeed *= 0.7f; // inertia, smooth translation
	if (translationSpeed < 0.05f) // to be sure to have no movement
		translationSpeed = 0;
}

/**
 * give a point of view using rotations and lookAt function
 */
glm::mat4 OrbitalCamera::getViewMatrix() const
{
	glm::mat4 rotationXYZ = glm::eulerAngleYXZ(yaw,pitch,roll);

	glm::vec3 direction(rotationXYZ * glm::vec4(0,0,1.f,0));
	glm::vec3 eye(target + glm::normalize(direction) * distance);
	glm::vec3 up(rotationXYZ * glm::vec4(0,1.f,0,0));

	return glm::lookAt(eye,target,up);
}
