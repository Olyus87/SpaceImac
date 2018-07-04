#include <glimac/FreeflyCamera.hpp>

void FreeflyCamera::computeDirectionVectors()
{
    m_FrontVector = glm::vec3(glm::cos(m_fTheta)*glm::sin(m_fPhi),
                              glm::sin(m_fTheta),
                              glm::cos(m_fTheta)*glm::cos(m_fPhi));
    m_LeftVector = glm::vec3(glm::sin(m_fPhi+(glm::pi<float>()*0.5)),
                0,
                glm::cos(m_fPhi+(glm::pi<float>()*0.5)));
    m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
}

FreeflyCamera::FreeflyCamera()
{
	m_Position = glm::vec3(0,0,-5);
	m_fPhi = 0;
    m_fTheta = 0;
    computeDirectionVectors();
}

void FreeflyCamera::moveLeft(float t)
{
    m_Position += m_LeftVector*t;
}

void FreeflyCamera::moveFront(float t)
{
    m_Position += m_FrontVector*t;
}

void FreeflyCamera::rotateLeft(float degrees)
{
    m_fPhi += glm::radians(degrees);
    computeDirectionVectors();
}

void FreeflyCamera::rotateUp(float degrees)
{
    m_fTheta += glm::radians(degrees);
    computeDirectionVectors();
}

glm::mat4 FreeflyCamera::getViewMatrix() const
{
    return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
}
