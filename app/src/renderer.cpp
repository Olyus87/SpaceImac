#include "renderer.h"

#include "spacimac.h"
#include "scene.h"
#include "camera.h"

Renderer::Renderer()
{}

void Renderer::initialize()
{
	loadProgram();
	loadUniforms();
}

void Renderer::loadProgram()
{
	program = glimac::loadProgram(
		SpacImac::instance()->getFilePath("shaders/3D.vs.glsl"),
		SpacImac::instance()->getFilePath("shaders/normals.fs.glsl")
	);
}

void Renderer::loadUniforms()
{
	uMVPMatrix = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
	uMVMatrix = glGetUniformLocation(program.getGLId(), "uMVMatrix");
	uNormalMatrix = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
}

typedef std::list<Instance>::const_iterator InstanceIterator;

void Renderer::render(const Scene& scene, const BaseCamera& camera) const
{
	glEnable(GL_DEPTH_TEST);

	// bind the shaders and the scene buffers
	program.use();
	scene.bind();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat4 projMatrix = camera.getProjectionMatrix(SpacImac::instance()->viewWidth(),
													  SpacImac::instance()->viewHeight());

	// for each instance, defining the MV and MVP matrix then draw it
	for(InstanceIterator i = scene.begin(); i != scene.end(); ++i)
	{
		glm::mat4 MVMatrix = viewMatrix * i->transform.getModelMatrix();
		glm::mat4 MVPMatrix = projMatrix * MVMatrix;
		glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
		glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix))));
		glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

		scene.mesh(i->meshId).draw();
	}
	scene.unbind();
}

LightRenderer::LightRenderer()
{}

void LightRenderer::loadProgram()
{
	program = glimac::loadProgram(
				SpacImac::instance()->getFilePath("shaders/light.vs.glsl"),
				SpacImac::instance()->getFilePath("shaders/light.fs.glsl")
				);
}

void LightRenderer::loadUniforms()
{
	Renderer::loadUniforms();
	uVMatrix = glGetUniformLocation(program.getGLId(), "uVMatrix");

	uDirectionalLightDir = glGetUniformLocation(program.getGLId(), "uDirectionalLightDir");
	uDirectionalLightColor = glGetUniformLocation(program.getGLId(), "uDirectionalLightColor");
	uDirectionalLightPower = glGetUniformLocation(program.getGLId(), "uDirectionalLightPower");

	uPointLightPos = glGetUniformLocation(program.getGLId(), "uPointLightPos");
	uPointLightColor = glGetUniformLocation(program.getGLId(), "uPointLightColor");
	uPointLightPower = glGetUniformLocation(program.getGLId(), "uPointLightPower");

	uAmbiantLightColor = glGetUniformLocation(program.getGLId(), "uAmbiantLightColor");
	uAmbiantLightPower = glGetUniformLocation(program.getGLId(), "uAmbiantLightPower");

	// Material
	uKa = glGetUniformLocation(program.getGLId(), "uKa");
	uKd = glGetUniformLocation(program.getGLId(), "uKd");
	uKs = glGetUniformLocation(program.getGLId(), "uKs");
	uShininess = glGetUniformLocation(program.getGLId(), "uShininess");
}

void LightRenderer::render(const Scene &scene, const BaseCamera &camera) const
{
	glEnable(GL_DEPTH_TEST);

	program.use();
	scene.bind();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat4 projMatrix = camera.getProjectionMatrix(SpacImac::instance()->viewWidth(),
													  SpacImac::instance()->viewHeight());

	glUniform3fv(uDirectionalLightDir, 1, glm::value_ptr(scene.directionalLight.direction));
	glUniform3fv(uDirectionalLightColor, 1, glm::value_ptr(scene.directionalLight.color));
	glUniform1f(uDirectionalLightPower, scene.directionalLight.power);
	glUniform3fv(uPointLightPos, 1, glm::value_ptr(scene.pointLight.position));
	glUniform3fv(uPointLightColor, 1, glm::value_ptr(scene.pointLight.color));
	glUniform1f(uPointLightPower, scene.pointLight.power);
	glUniform3fv(uAmbiantLightColor, 1, glm::value_ptr(scene.ambiantLight.color));
	glUniform1f(uAmbiantLightPower, scene.ambiantLight.power);

	glUniformMatrix4fv(uVMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// for each instance, defining the matrix and the material then draw it
	for(InstanceIterator i = scene.begin(); i != scene.end(); ++i)
	{
		glm::mat4 MVMatrix = viewMatrix * i->transform.getModelMatrix();
		glm::mat4 MVPMatrix = projMatrix * MVMatrix;
		const Material& m = scene.materialOfInstance(*i);

		glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
		glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix))));
		glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

		glUniform3fv(uKa, 1, glm::value_ptr(m.ka));
		glUniform3fv(uKd, 1, glm::value_ptr(m.kd));
		glUniform3fv(uKs, 1, glm::value_ptr(m.ks));
		glUniform1f(uShininess, m.shininess);

		scene.mesh(i->meshId).draw();
	}
	scene.unbind();
}



TextureAndLightRenderer::TextureAndLightRenderer()
{}

void TextureAndLightRenderer::loadProgram()
{
	program = glimac::loadProgram(
				SpacImac::instance()->getFilePath("shaders/light.vs.glsl"),
				SpacImac::instance()->getFilePath("shaders/textureandlight.fs.glsl")
	);
}

void TextureAndLightRenderer::loadUniforms()
{
	LightRenderer::loadUniforms();

	uUseKaTexture = glGetUniformLocation(program.getGLId(), "uUseKaTexture");
	uUseKdTexture = glGetUniformLocation(program.getGLId(), "uUseKdTexture");
	uUseKsTexture = glGetUniformLocation(program.getGLId(), "uUseKsTexture");
	uUseNormalTexture = glGetUniformLocation(program.getGLId(), "uUseNormalTexture");

	uKaTexture = glGetUniformLocation(program.getGLId(), "uKaTexture");
	uKdTexture = glGetUniformLocation(program.getGLId(), "uKdTexture");
	uKsTexture = glGetUniformLocation(program.getGLId(), "uKsTexture");
	uNormalTexture = glGetUniformLocation(program.getGLId(), "uNormalTexture");
}

void TextureAndLightRenderer::render(const Scene &scene, const BaseCamera &camera) const
{
	glEnable(GL_DEPTH_TEST);

	program.use();
	scene.bind();
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat4 projMatrix = camera.getProjectionMatrix(SpacImac::instance()->viewWidth(),
													  SpacImac::instance()->viewHeight());
	glUniform3fv(uDirectionalLightDir, 1, glm::value_ptr(scene.directionalLight.direction));
	glUniform3fv(uDirectionalLightColor, 1, glm::value_ptr(scene.directionalLight.color));
	glUniform1f(uDirectionalLightPower, scene.directionalLight.power);
	glUniform3fv(uPointLightPos, 1, glm::value_ptr(scene.pointLight.position));
	glUniform3fv(uPointLightColor, 1, glm::value_ptr(scene.pointLight.color));
	glUniform1f(uPointLightPower, scene.pointLight.power);
	glUniform3fv(uAmbiantLightColor, 1, glm::value_ptr(scene.ambiantLight.color));
	glUniform1f(uAmbiantLightPower, scene.ambiantLight.power);

	glUniformMatrix4fv(uVMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glUniform1i(uKaTexture, 0);
	glUniform1i(uKdTexture, 1);
	glUniform1i(uKsTexture, 2);
	glUniform1i(uNormalTexture, 3);

	// for each instance, defining the matrix and the material, bind the textures, then draw it
	for(InstanceIterator i = scene.begin(); i != scene.end(); ++i)
	{
		glm::mat4 MVMatrix = viewMatrix * i->transform.getModelMatrix();
		glm::mat4 MVPMatrix = projMatrix * MVMatrix;
		const Material& m = scene.materialOfInstance(*i);

		glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
		glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(MVMatrix))));
		glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

		glUniform3fv(uKa, 1, glm::value_ptr(m.ka));
		glUniform3fv(uKd, 1, glm::value_ptr(m.kd));
		glUniform3fv(uKs, 1, glm::value_ptr(m.ks));
		glUniform1f(uShininess, m.shininess);

		bindMaterial(m, scene);

		scene.mesh(i->meshId).draw();
	}
	scene.unbind();
}

void TextureAndLightRenderer::bindMaterial(const Material &m, const Scene& scene) const
{
	if (m.kaTextureId>=0)
	{
		scene.texture(m.kaTextureId).bind(0);
		glUniform1i(uUseKaTexture, 1);
	}
	else
	{
		glUniform1i(uUseKaTexture, 0);
	}
	if (m.kdTextureId>=0)
	{
		scene.texture(m.kdTextureId).bind(1);
		glUniform1i(uUseKdTexture, 1);
	}
	else
	{
		glUniform1i(uUseKdTexture, 0);
	}
	if (m.ksTextureId>=0)
	{
		scene.texture(m.ksTextureId).bind(2);
		glUniform1i(uUseKsTexture, 1);
	}
	else
	{
		glUniform1i(uUseKsTexture, 0);
	}
	if (m.normalTextureId>=0)
	{
		scene.texture(m.normalTextureId).bind(3);
		glUniform1i(uUseNormalTexture, 1);
	}
	else
	{
		glUniform1i(uUseNormalTexture, 0);
	}
}

SkyboxRenderer::SkyboxRenderer()
	{}

void SkyboxRenderer::loadProgram()
{
	program = glimac::loadProgram(
				SpacImac::instance()->getFilePath("shaders/skybox.vs.glsl"),
				SpacImac::instance()->getFilePath("shaders/skybox.fs.glsl")
				);
}

void SkyboxRenderer::loadUniforms()
{
	Renderer::loadUniforms();

	uTexture = glGetUniformLocation(program.getGLId(), "uTexture");
}

void SkyboxRenderer::render(const Scene &scene, const BaseCamera &camera) const
{
	if (scene.skybox().meshId < 0)
		return;
	// Disable depth to always the sky in background
	glDisable(GL_DEPTH_TEST);

	program.use();
	scene.bind();

	const Material& m = scene.material(scene.skybox().materialId);
	const Texture& t = scene.texture(m.kdTextureId);
	t.bind(30);
	glUniform1i(uTexture, 30);

	glm::mat4 viewMatrix = camera.getViewMatrix();
	viewMatrix[3].x *= 0.01f; // low camera translation given that the sky is far far away
	viewMatrix[3].y *= 0.01f;
	viewMatrix[3].z *= 0.01f;
	glm::mat4 projMatrix = camera.getProjectionMatrix(SpacImac::instance()->viewWidth(), SpacImac::instance()->viewHeight());
	glm::mat4 MVMatrix = viewMatrix * scene.skybox().transform.getModelMatrix();
	glm::mat4 MVPMatrix = projMatrix * MVMatrix;

	glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

	scene.mesh(scene.skybox().meshId).draw();

	scene.unbind();
}
