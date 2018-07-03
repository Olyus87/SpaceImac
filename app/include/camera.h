#ifndef CAMERA_H
#define CAMERA_H

#include <SDL/SDL_events.h>

#include "glimac/common.hpp"
#include "spaceelement.h"

/**
 * @brief
 * This object define the camera view by processing the view matrix and the projection matrix\n
 * It can be updated according to input events and time
 */
class BaseCamera
{
public:
	BaseCamera();

	/**
	 * @brief handleEvent
	 * This function to update the camera according to SDL events. By default, the camera doesn't change
	 */
	virtual void handleEvent(const SDL_Event& e);
	/**
	 * @brief update
	 * This function to update the camera according to time. By default, the camera doesn't change
	 * @param deltaTime
	 * Time between two frame in seconds
	 */
	virtual void update(float deltaTime);

	/**
	 * @brief getViewMatrix
	 * This function to process a specific point of view according to attributes.\n
	 * By default, it returns identity
	 * @return The view matrix processed according to the attributes
	 */
	virtual glm::mat4 getViewMatrix() const;
	/**
	 * @brief getProjectionMatrix
	 * This function to process a specific screen projection according to attributes.\n
	 * By default, it returns perspective matrix according to FoV, near and far
	 * @return The projection matrix processed according to the attributes
	 */
	virtual glm::mat4 getProjectionMatrix(float viewWidth, float viewHeight) const;

	/**
	 * @brief Field of view in degrees
	 */
	float FoV;
	float near;
	float far;
};

/**
 * @brief
 * Derived from BaseCamera, this camera define a point of view by looking at a target and turning around\n
 * this
 */
class OrbitalCamera : public BaseCamera
{
public:
	OrbitalCamera();

	virtual void handleEvent(const SDL_Event& e);
	virtual void update(float deltaTime);

	virtual glm::mat4 getViewMatrix() const;

	/**
	 * @brief
	 * the point to look at
	 */
	glm::vec3 target;
	/**
	 * @brief distance to the target
	 */
	float distance;
	/**
	 * @brief how much we increase the speed
	 */
	float translationAcc;
	/**
	 * @brief increase of distance in units/second
	 */
	float translationSpeed;
	/**
	 * @brief rotation around x-axis in radians
	 */
	float pitch; // rotation X
	/**
	 * @brief rotation around y-axis in radians
	 */
	float yaw; // rotation Y
	/**
	 * @brief rotation around z-axis in radians
	 */
	float roll; // rotation Z
	/**
	 * @brief how much we increase the rotation speed
	 */
	float rotationAcc;

private:
	float speedPitch;
	float speedYaw;

};

#endif // CAMERA_H
