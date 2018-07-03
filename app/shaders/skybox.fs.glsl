#version 330
#ifdef GL_ES
precision mediump float;
#endif

in vec3 vTexCoords;
uniform samplerCube uTexture;

out vec3 fFragColor;

void main()
{
	fFragColor = texture(uTexture, vTexCoords).xyz;
	//fFragColor = vec3(0.5,0.8,0.5);
}
