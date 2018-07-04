#ifndef TRACKBALL_CAMERA_H
#define TRACKBALL_CAMERA_H

#include <glimac/glm.hpp>

class TrackballCamera {
private:
    float m_fDistance;
    float m_fAngleX;
    float m_fAngleY;
public:
    TrackballCamera();
    void moveFront(float delta);
    void rotateLeft(float degrees);
    void rotateUp(float degrees);
    glm::mat4 getViewMatrix() const;
};

#endif //TRACKBALL_CAMERA_H
