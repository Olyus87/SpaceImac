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
uniform mat4 uNormalMatrix;

// Sorties
out vec3 vPosition_vs;
out vec3 vNormal_vs;
out vec2 vTexCoords;

void main() {
	vec4 aVertexPosition = vec4(aVertexPosition.xyz, 1);
	vec4 aVertexNormal = vec4(aVertexNormal.xyz, 0);

	vPosition_vs = vec3(uMVMatrix*aVertexPosition);
	vNormal_vs = vec3(uNormalMatrix*aVertexNormal);
	vTexCoords = aVertexTexCoords;

	gl_Position = uMVPMatrix*aVertexPosition;
}
