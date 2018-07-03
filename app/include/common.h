#ifndef COMMON_H
#define COMMON_H

#define GLEW_STATIC

#include "glimac/common.hpp"
#include "glimac/Image.hpp"
#include "glimac/Geometry.hpp"

#include "glm/gtx/euler_angles.hpp"

typedef uint32_t uint;

/**
 * @brief Structure which store data for drawing mesh
 */
struct Mesh {
	Mesh(uint indexOffset, uint indexCount, int materialIndex)
		: indexOffset(indexOffset), indexCount(indexCount), materialId(materialIndex)
	{}
	Mesh()
		: Mesh(0,0,-1)
	{}

	/**
	 * @brief draw mesh by drawing triangles according to vertex index and count
	 */
	void draw() const
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*) (indexOffset*sizeof(GLuint)));
	}

	/**
	 * @brief Offset in the index buffer
	 */
	uint indexOffset;
	/**
	 * @brief Number of vertex indices
	 */
	uint indexCount;
	/**
	 * @brief If a material is affected to this mesh, materialId define the id of this material\n
	 * in the scene material buffer. Otherwise materialId is -1
	 */
	int materialId; // -1 if no material assigned
};

/**
 * @brief Structure which store data for binding textures\n
 * call initialize to process the texture from the given image
 */
struct Texture
{
	/**
	 * @brief Loaded image giving pixel buffer
	 */
	std::unique_ptr<glimac::Image> image;
	/**
	 * @brief Stored texture's id
	 */
	GLuint textureId;
	/**
	 * @brief Sampler object's id
	 */
	GLuint samplerId;
	/**
	 * @brief Texture target (GL_TEXTURE_2D, GL_TEXTURE_3D, ...)
	 */
	GLenum target;

	/**
	 * @brief Empty constructor, used for allocation
	 */
	Texture() :
		textureId(0), samplerId(0), target(GL_TEXTURE_2D)
	{}

	/**
	 * @brief Load an image from the given file path
	 * @param filepath path to the image
	 */
	Texture(const glimac::FilePath& filepath) :
		image(glimac::loadImage(filepath)), textureId(0), samplerId(0),
		target(GL_TEXTURE_2D)
	{
		if (!image)
			throw std::runtime_error("Can't load texture:" + filepath.str());
	}

	/**
	 * @brief Copy constructor, copy only the opengl attributes (id, sampler and target)
	 */
	Texture(const Texture& other) :
		image(nullptr), textureId(other.textureId),
		samplerId(other.samplerId), target(other.target)
	{}

	/**
	 * @brief Generate the opengl texture from the image
	 * @param filterParam GL parameter used for GL_TEXTURE_*_FILTER when initialize sampler
	 * @param wrapParam GL parameter used for GL_TEXTURE_WRAP_* when initialize sampler
	 */
	virtual void initialize(GLint filterParam = GL_LINEAR, GLint wrapParam = GL_REPEAT)
	{
		if (!image.get())
			throw std::runtime_error("The texture has no image reference");
		glGenTextures(1, &textureId);
		glBindTexture(target, textureId);
		glTexStorage2D(target, 1, GL_RGB32F,
									 image->getWidth(), image->getHeight());
		glTexSubImage2D(target, 0, 0, 0,
										image->getWidth(), image->getHeight(),
										GL_RGBA, GL_FLOAT, image->getPixels());
		glBindTexture(target, 0);

		glGenSamplers(1, &samplerId);
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, filterParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, filterParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, wrapParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, wrapParam);
		image.release();
	}

	/**
	 * @brief Bind the texture to the given texture unit
	 * @param textureUnit id of the texture unit (0 = GL_TEXTURE0, 1 = GL_TEXTURE1, ...)
	 */
	virtual void bind(GLint textureUnit) const
	{
		glBindSampler(textureUnit, samplerId);
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(target, textureId);
	}
};

/**
 * @brief Texture to use for creating sky cube
 */
struct SkyTexture : public Texture
{
	/**
	 * @brief Rather than using an unique image, we use 6 for each face of the cube\n
	 * Each image contains pixel buffer
	 */
	std::unique_ptr<glimac::Image> images[6];

	SkyTexture()
	{
		target = GL_TEXTURE_CUBE_MAP;
	}

	/**
	 * @brief Initialize the images from files in the folderpath. The folder must contains
	 * up.tga, dn.tga, rt.tga, lf.tga, ft.tga, bk.tga
	 */
	SkyTexture(const glimac::FilePath& folderpath)
	{
		target = GL_TEXTURE_CUBE_MAP;
		images[0] = glimac::loadImage(folderpath + "up.tga");
		if (!images[0])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/up.tga");
		images[1] = glimac::loadImage(folderpath + "dn.tga");
		if (!images[1])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/dn.tga");
		images[2] = glimac::loadImage(folderpath + "rt.tga");
		if (!images[2])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/rt.tga");
		images[3] = glimac::loadImage(folderpath + "lf.tga");
		if (!images[3])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/lf.tga");
		images[4] = glimac::loadImage(folderpath + "ft.tga");
		if (!images[4])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/ft.tga");
		images[5] = glimac::loadImage(folderpath + "bk.tga");
		if (!images[5])
			throw std::runtime_error("Can't load skybox:" + folderpath.str() + "/bk.tga");
	}

	/**
	 * @brief Copy constructor, copy only the opengl attributes (id, sampler and target)
	 */
	SkyTexture(const Texture& other) :
		Texture(other)
	{}

	/**
	 * @brief Generate the opengl textures from the images for each face
	 * @param filterParam GL parameter used for GL_TEXTURE_*_FILTER when initialize sampler
	 * @param wrapParam GL parameter used for GL_TEXTURE_WRAP_* when initialize sampler
	 */
	void initialize(GLint filterParam = GL_LINEAR, GLint wrapParam = GL_CLAMP_TO_EDGE)
	{
		if (!images[0].get())
			throw std::runtime_error("The texture has no image reference");

		glGenTextures(1, &textureId);
		glBindTexture(target, textureId);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
				0, GL_RGBA, GL_FLOAT, images[3]->getPixels());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
						0, GL_RGBA, GL_FLOAT, images[2]->getPixels());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
						0, GL_RGBA, GL_FLOAT, images[1]->getPixels());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
						0, GL_RGBA, GL_FLOAT, images[0]->getPixels());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
						0, GL_RGBA, GL_FLOAT, images[5]->getPixels());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA32F,
								 images[0]->getWidth(), images[0]->getHeight(),
						0, GL_RGBA, GL_FLOAT, images[4]->getPixels());

		glGenSamplers(1, &samplerId);
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, filterParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, filterParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, wrapParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, wrapParam);
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_R, wrapParam);

		for (int i=0; i<6; ++i)
			images[i].release();
	}
};

/**
 * @brief Data structure used for shading
 */
struct Material {
	/**
	 * @brief Ambiant color
	 */
	glm::vec3 ka;
	/**
	 * @brief Diffuse color
	 */
	glm::vec3 kd;
	/**
	 * @brief Specular color
	 */
	glm::vec3 ks;
	float shininess;
	/**
	 * @brief If an ambiant texture is needed, kaTextureId is the id of the this texture in scene materials buffer\n
	 * -1 otherwise
	 */
	int kaTextureId; // id in the scene
	/**
	 * @brief If an diffuse texture is needed, kdTextureId is the id of the this texture in scene materials buffer\n
	 * -1 otherwise
	 */
	int kdTextureId;
	/**
	 * @brief If a specular texture is needed, ksTextureId is the id of the this texture in scene materials buffer\n
	 * -1 otherwise
	 */
	int ksTextureId;
	/**
	 * @brief If an normal texture is needed, kaTextureId is the id of the this texture in scene materials buffer\n
	 * -1 otherwise
	 */
	int normalTextureId;

	Material(glm::vec3 ka = glm::vec3(0.5,0.5,0.5),
					 glm::vec3 kd = glm::vec3(0.5,0.5,0.5),
					 glm::vec3 ks = glm::vec3(0.5,0.5,0.5),
					 float shininess = 1,	int kaTextureId = -1, int kdTextureId = -1,
					 int ksTextureId = -1, int normalTextureId = -1)
		: ka(ka), kd(kd), ks(ks), shininess(shininess),
			kaTextureId(kaTextureId), kdTextureId(kdTextureId),
			ksTextureId(ksTextureId), normalTextureId(normalTextureId)
	{}

	Material(glm::vec3 color)
		: Material(color, color)
	{}

	Material(int textureId)
		: Material(glm::vec3(0.5,0.5,0.5),glm::vec3(0.5,0.5,0.5),glm::vec3(0.5,0.5,0.5),
							 1, textureId, textureId)
	{}

	Material(glm::vec3 color, int textureId)
		: Material(color, color,glm::vec3(0.5,0.5,0.5),
							 1, textureId, textureId)
	{}

	Material(const glimac::Geometry::Material& other)
		: Material(other.m_Ka, other.m_Kd, other.m_Ks, other.m_Shininess)
	{}
};

/**
 * @brief Data structure used for mesh transformation
 */
struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform()
		:	position{0,0,0}, rotation{0,0,0}, scale{1,1,1}
	{}

	/**
	 * @return the matrix used for transformation
	 */
	glm::mat4 getModelMatrix() const
	{
		glm::mat4 rotationXYZ = glm::eulerAngleYXZ(rotation.y,rotation.x,rotation.z);
		return	glm::translate(glm::mat4(1.f), position) *
						rotationXYZ *
						glm::scale(glm::mat4(1.f), scale);
	}
};

/**
 * @brief Instanciation of a mesh with a specific transform
 */
struct Instance
{
	Transform transform;
	/**
	 * @brief id in the scene mesh buffer
	 */
	int meshId;
	/**
	 * @brief id in the scene material buffer if the material is overrided, -1 otherwise
	 */
	int materialId; // -1 if no material assigned

	Instance(int meshId)
		: meshId(meshId), materialId(-1)
	{}
};

/**
 * @brief Data structure describing directional light behavior
 */
struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 color;
	float power;
};

/**
 * @brief Data structure describing point light behavior
 */
struct PointLight
{
	glm::vec3 position;
	glm::vec3 color;
	float power;
};

/**
 * @brief Data structure describing ambiant light behavior
 */
struct AmbiantLight
{
	glm::vec3 color;
	float power;
};
#endif // COMMON_H
