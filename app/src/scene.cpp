#include "scene.h"

#include <vector>

Scene::Scene()
	: ambiantLight{glm::vec3(0.2,0.2,0.2), 1},
		directionalLight{glm::vec3(-0.7f,-0.7,0.f),glm::vec3(0.2,0.3f,0.2),1},
		pointLight{glm::vec3(1,1,1), glm::vec3(0.2,0.3,0.7),3},
		m_VAOid(0), m_VBOid(0), m_IBOid(0), m_skybox(-1),
		m_initialized(false)
	{}

Scene::~Scene()
{
	if(m_VAOid)
		glDeleteVertexArrays(1, &m_VAOid);
	if(m_VBOid)
		glDeleteBuffers(1, &m_VBOid);
	if(m_IBOid)
		glDeleteBuffers(1, &m_IBOid);
}

/**
 * update scene from a glimac::Geometry
 * Add meshes, vertices, vertices index and materials
 */
uint Scene::addMeshes(const glimac::Geometry &geometry)
{
	const glimac::Geometry::Mesh* meshes = geometry.getMeshBuffer();
	const glimac::Geometry::Vertex* vertices = geometry.getVertexBuffer();
	const glimac::Geometry::Material* materials = geometry.getMaterialBuffer();
	const unsigned int* index = geometry.getIndexBuffer();
	unsigned int firstindex = this->meshes.size();

	/* for each mesh in geometry
	 * We create a new mesh, set the index count and define the index offset as the
	 * index of the last vertice + 1 (start of the new mesh)
	 * If a material is affected, create the material and
	 * define the material id with the new one
	 * Add vertices index
	 */
	for (int i=0; i<geometry.getMeshCount(); ++i)
	{
		glimac::Geometry::Mesh mesh = meshes[i];
		Mesh newMesh;
		newMesh.indexCount = mesh.m_nIndexCount;
		newMesh.indexOffset = this->verticesIndex.size();
		if (mesh.m_nMaterialIndex >= 0)
		{
			newMesh.materialId = this->materials.size();
			this->materials.push_back(Material(materials[mesh.m_nMaterialIndex]));
		}
		for (int j=mesh.m_nIndexOffset; j<mesh.m_nIndexOffset+mesh.m_nIndexCount; ++j)
		{
			this->verticesIndex.push_back(this->vertices.size() + index[j]);
		}
		this->meshes.push_back(newMesh);
	}
	/*
	 * Add all vertices
	 */
	for (int i=0; i<geometry.getVertexCount(); ++i)
	{
		this->vertices.push_back(vertices[i]);
	}
	return firstindex;
}

Instance& Scene::makeInstance(uint meshId)
{
	instances.push_back(Instance(meshId));
	return instances.back();
}

/**
 * Create a texture from the path, initialize it and add it in textures
 */
uint Scene::addTexture(const glimac::FilePath &imagePath)
{
	Texture t(imagePath);
	uint id = textures.size();
	t.initialize();
	textures.push_back(t);
	return id;
}

/**
 * Create texture from the path, initialize it and add it in textures
 */
uint Scene::addSkyTexture(const glimac::FilePath& folderPath)
{
	SkyTexture t(folderPath);
	uint id = textures.size();
	t.initialize();
	textures.push_back(t);
	return id;
}

uint Scene::addMaterial(const Material &other)
{
	uint id = materials.size();
	materials.push_back(other);
	return id;
}

/**
 * Buffers initializing
 * Generate with glGenVertexArrays and glGenBuffers
 * Make VBO from vertices (transfer vertices buffer)
 * Make IBO from vertices index (transfer vertices index buffer)
 * Make VAO by binding VBO and IBO and defining attributes with GLATTRIBUT enum
 */
void Scene::initializeBuffers()
{
	if (vertices.empty())
		throw std::runtime_error("there isn't any mesh in the scene");
	glGenVertexArrays(1, &m_VAOid);
	glGenBuffers(1, &m_VBOid);
	glGenBuffers(1, &m_IBOid);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOid);
	glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glimac::Geometry::Vertex),
			std::vector<glimac::Geometry::Vertex>(std::begin(vertices), std::end(vertices)).data(),
			GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			verticesIndex.size() * sizeof(GLuint),
			std::vector<GLuint>(std::begin(verticesIndex), std::end(verticesIndex)).data(),
			GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(m_VAOid);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOid);

	glEnableVertexAttribArray(VertexPosition);
	glEnableVertexAttribArray(VertexNormal);
	glEnableVertexAttribArray(VertexTexCoord);

	glVertexAttribPointer(VertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex),
												(GLvoid*) offsetof(glimac::Geometry::Vertex, m_Position));
	glVertexAttribPointer(VertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex),
												(GLvoid*) offsetof(glimac::Geometry::Vertex, m_Normal));
	glVertexAttribPointer(VertexTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::Geometry::Vertex),
												(GLvoid*) offsetof(glimac::Geometry::Vertex, m_TexCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOid);

	glBindVertexArray(0);

	m_initialized = true;
}

void Scene::setSkybox(const glimac::Geometry &box, const glimac::FilePath &folderPath)
{
	uint textureId = addSkyTexture(folderPath);
	m_skybox.meshId = addMeshes(box);
	m_skybox.materialId = addMaterial(Material(textureId));
	m_skybox.transform.scale = {75,75,75};
}

void Scene::bind() const
{
	glBindVertexArray(m_VAOid);
}

void Scene::unbind() const
{
	glBindVertexArray(0);
}

GLuint Scene::VAOid() const
{
	return m_VAOid;
}

GLuint Scene::VBOid() const
{
	return m_VBOid;
}

GLuint Scene::IBOid() const
{
	return m_IBOid;
}

bool Scene::initialized() const
{
	return m_initialized;
}

std::list<Instance>::const_iterator Scene::begin() const
{
	return instances.cbegin();
}

std::list<Instance>::const_iterator Scene::end() const
{
	return instances.cend();
}

const Texture& Scene::texture(uint i) const
{
	return textures[i];
}

const Material& Scene::material(uint i) const
{
	return materials[i];
}

Material &Scene::material(uint i)
{
	return materials[i];
}

const Mesh& Scene::mesh(uint i) const
{
	return meshes[i];
}

Mesh& Scene::mesh(uint i)
{
	return meshes[i];
}

const Instance &Scene::skybox() const
{
	return m_skybox;
}

Material _default;

const Material &Scene::materialOfInstance(const Instance &i) const
{
	if (i.materialId >= 0)
		return materials[i.materialId];
	else if (meshes[i.meshId].materialId >= 0)
		return materials[meshes[i.meshId].materialId];
	return _default;
}
