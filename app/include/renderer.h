#ifndef RENDERER_H
#define RENDERER_H

#include "glimac/Program.hpp"

class Scene;
class BaseCamera;
class Material;

/**
 * @brief Base class for rendering a scene. Use a the normal shading
 */
class Renderer
{
public:
	Renderer();

	/**
	 * @brief Initialize the shaders and the uniform
	 */
	void initialize();
	/**
	 * @brief Initialize the shaders. Override this function to change the shaders to use
	 * with glimac::loadProgram
	 */
	virtual void loadProgram();
	/**
	 * @brief Initialize shaders uniforms. Override this function for getting the uniforms according
	 * to the loaded shaders
	 */
	virtual void loadUniforms();

	/**
	 * @brief render the scene according to the camera view.
	 * Override this function to render according to the shaders\n
	 * Draw each mesh instance of the scene by setting shaders uniform
	 */
	virtual void render(const Scene& scene, const BaseCamera &camera) const;

protected:
	/**
	 * @brief shaders loaded in the GPU by glimac::loadProgram
	 */
	glimac::Program program;

	/**
	 * @brief id of the uniform uMVPMatrix, the Model View Projection matrix which is a matrix 4x4
	 */
	GLint uMVPMatrix;
	/**
	 * @brief id of the uniform uMVMatrix, the Model View matrix which is a matrix 4x4
	 */
	GLint uMVMatrix;
	/**
	 * @brief id of the uniform uNormalMatrix, the Normal matrix
	 * (transposed inverse of MV matrix) which is a matrix 4x4
	 */
	GLint uNormalMatrix;
};

/**
 * @brief Rendering the scene with the bling-phong model
 */
class LightRenderer : public Renderer
{
public:
	LightRenderer();

	virtual void loadProgram();
	virtual void loadUniforms();
	virtual void render(const Scene& scene, const BaseCamera &camera) const;

protected:
	/**
	 * @brief id of the uniform uVMatrix, the View matrix which is a matrix 4x4
	 */
	GLint uVMatrix;

	/**
	 * @brief id of the uniform uDirectionalLightDir,
	 * the direction of the directional light which is a vector 3
	 */
	GLint uDirectionalLightDir;
	/**
	 * @brief id of the uniform uDirectionalLightColor,
	 * the color of the directional light which is a vector 3
	 */
	GLint uDirectionalLightColor;
	/**
	 * @brief id of the uniform uDirectionalLightPower,
	 * the power of the directional light which is a float
	 */
	GLint uDirectionalLightPower;

	/**
	 * @brief id of the uniform uPointLightPos,
	 * the position of the point light which is a vector 3
	 */
	GLint uPointLightPos;
	/**
	 * @brief id of the uniform uPointLightColor,
	 * the color of the point light which is a vector 3
	 */
	GLint uPointLightColor;
	/**
	 * @brief id of the uniform uPointLightPower,
	 * the power of the point light which is a float
	 */
	GLint uPointLightPower;

	/**
	 * @brief id of the uniform uAmbiantLightColor,
	 * the color of the directional light which is a vector 3
	 */
	GLint uAmbiantLightColor;
	/**
	 * @brief id of the uniform uAmbiantLightPower,
	 * the power of the ambiant light which is a float
	 */
	GLint uAmbiantLightPower;

	// Material
	GLint uKa;
	GLint uKd;
	GLint uKs;
	GLint uShininess;
};

/**
 * @brief Rendering the scene with the bling-phong model and with the textures
 */
class TextureAndLightRenderer : public LightRenderer
{
public:
	TextureAndLightRenderer();

	virtual void loadProgram();
	virtual void loadUniforms();
	virtual void render(const Scene& scene, const BaseCamera &camera) const;

	void bindMaterial(const Material& m, const Scene &scene) const;
protected:
	//Material
	/**
	 * @brief id of the uniform uUseKaTexture,
	 * boolean which says whether or not we use the ambiant texture
	 */
	GLint uUseKaTexture;
	/**
	 * @brief id of the uniform uUseKdTexture,
	 * boolean which says whether or not we use the diffuse texture
	 */
	GLint uUseKdTexture;
	/**
	 * @brief id of the uniform uUseKsTexture,
	 * boolean which says whether or not we use the specular texture
	 */
	GLint uUseKsTexture;
	/**
	 * @brief id of the uniform uUseNormalTexture,
	 * boolean which says whether or not we use the normal texture
	 */
	GLint uUseNormalTexture;

	/**
	 * @brief id of the uniform uKaTexture,
	 * the unit where the ambiant texture is binded
	 */
	GLint uKaTexture;
	/**
	 * @brief id of the uniform uKdTexture,
	 * the unit where the diffuse texture is binded
	 */
	GLint uKdTexture;
	/**
	 * @brief id of the uniform uKsTexture,
	 * the unit where the specular texture is binded
	 */
	GLint uKsTexture;
	/**
	 * @brief id of the uniform uNormalTexture,
	 * the unit where the normal texture is binded
	 */
	GLint uNormalTexture;
};

/**
 * @brief Rendering the skybox of a scene
 */
class SkyboxRenderer : public Renderer
{
public:
	SkyboxRenderer();

	virtual void loadProgram();
	virtual void loadUniforms();
	virtual void render(const Scene& scene, const BaseCamera &camera) const;

protected:
	/**
	 * @brief id of the uniform uTexture,
	 * the unit where the sky texture is binded
	 */
	GLint uTexture;
};
#endif // RENDERER_H
