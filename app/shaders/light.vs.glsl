#version 330
#ifdef GL_ES
precision mediump float;
#endif

// Sommets
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;

// Matrices
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uVMatrix;
uniform mat4 uNormalMatrix;

// Directional Light
uniform vec3 uDirectionalLightDir;

// Point Light
uniform vec3 uPointLightPos;


// Sorties
out vec3 vWSPosition;
out vec3 vCSPosition;
out vec3 vCSNormal;
out vec2 vTexCoords;

out vec3 vCSEyeDir;

out vec3 vCSPointLightPos;
out vec3 vCSPointLightDir;

out vec3 vCSDirectionalLightDir;

void main() {
		vec4 aVertexPosition = vec4(aVertexPosition, 1);
		vec4 aVertexNormal = vec4(aVertexNormal, 0);

		vCSPosition = vec3(uMVMatrix*aVertexPosition);
		vCSNormal = vec3(uNormalMatrix*aVertexNormal);

		vCSEyeDir = vec3(0,0,0) - vCSPosition;
		vTexCoords = aVertexTexCoords;

		vCSPointLightPos = vec3(uVMatrix * vec4(uPointLightPos, 1));
		vCSPointLightDir = vCSPosition - vCSPointLightPos;

		vCSDirectionalLightDir = vec3(uVMatrix * vec4(uDirectionalLightDir, 0));

		gl_Position = uMVPMatrix*aVertexPosition;
}
