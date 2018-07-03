#version 330
#ifdef GL_ES
precision mediump float;
#endif

// Entrées
in vec2 vTexCoords;
in vec3 vPosition_vs;
in vec3 vNormal_vs;

// Sorties
out vec3 fFragColor;

void main() {
		fFragColor = normalize(vNormal_vs);
}
