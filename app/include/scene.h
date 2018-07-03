#ifndef SCENE_H
#define SCENE_H

#include <list>

#include "glimac/common.hpp"
#include "glimac/Geometry.hpp"

#include "common.h"

/**
 * @brief Data structure containing all structures for rendering 3D scene
 */
class Scene
{ 
public:
	enum GLATTRIBUT {
		VertexPosition=0,
		VertexNormal,
		VertexTexCoord
	};

	Scene();
	~Scene();

	/**
	 * @return index of the first added mesh
	 */
	uint addMeshes(const glimac::Geometry& geometry);
	/**
	 * @brief add an instance of the mesh given by meshId
	 * @return the reference of the created instance
	 */
	Instance& makeInstance(uint meshId);
	/**
	 * @return the id of the created texture
	 */
	uint addTexture(const glimac::FilePath& imagePath);
	/**
	 * @return the id of the created texture
	 */
	uint addSkyTexture(const glimac::FilePath &folderPath);
	/**
	 * @return the id of the created material
	 */
	uint addMaterial(const Material& other);
	/**
	 * @brief make a mesh from box, create a sky texture and affect this texture to the new mesh
	 */
	void setSkybox(const glimac::Geometry& box, const glimac::FilePath& folderPath);
	/**
	 * @brief initialize VBO, VAO and IBO from meshes of the scene.\n
	 * Note: Add all model meshes before calling this function for storing the vertex in
	 * the GPU
	 */
	void initializeBuffers();

	/**
	 * @brief Bind buffers
	 */
	void bind() const;
	void unbind() const;

	GLuint VAOid() const;
	GLuint VBOid() const;
	GLuint IBOid() const;
	/**
	 * @return true if the scene buffers are initialized
	 */
	bool initialized() const;
	/**
	 * @return a list start iterator for iterating through the mesh instances
	 */
	std::list<Instance>::const_iterator begin() const;
	/**
	 * @return a list end iterator for iterating through the mesh instances
	 */
	std::list<Instance>::const_iterator end() const;
	const Texture& texture(uint i) const;
	const Material &material(uint i) const;
	Material& material(uint i);
	const Mesh& mesh(uint i) const;
	Mesh& mesh(uint i);
	const Instance& skybox() const;
	/**
	 * @return the material affected to the instance or default material if nothing is affected
	 */
	const Material& materialOfInstance(const Instance& i) const;

	AmbiantLight ambiantLight;
	DirectionalLight directionalLight;
	PointLight pointLight;
private:
	GLuint m_VAOid;
	GLuint m_VBOid;
	GLuint m_IBOid;

	/**
	 * @brief models which contain the index of vertices index
	 */
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	/**
	 * @brief instances to draw
	 */
	std::list<Instance> instances;
	Instance m_skybox;

	/**
	 * @brief vertices to send to the VBO
	 */
	std::list<glimac::Geometry::Vertex> vertices;
	/**
	 * @brief vertices index to send to the IBO
	 */
	std::list<GLuint> verticesIndex;

	bool m_initialized;
};

#endif // SCENE_H
