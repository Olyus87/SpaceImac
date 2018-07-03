#version 330
#ifdef GL_ES
precision mediump float;
#endif

// Sommets
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;

out vec3 vTexCoords;

// Matrices
uniform mat4 uMVPMatrix;

void main()
{
	vTexCoords = aVertexPosition;
	gl_Position = uMVPMatrix*vec4(aVertexPosition, 1.0f);
}
