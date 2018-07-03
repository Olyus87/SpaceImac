#version 330
#ifdef GL_ES
precision mediump float;
#endif

// Lights
uniform vec3 uDirectionalLightColor;
uniform float uDirectionalLightPower;
uniform vec3 uPointLightColor;
uniform float uPointLightPower;
uniform vec3 uAmbiantLightColor;
uniform float uAmbiantLightPower;

// Material
uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

// Variable In
in vec3 vWSPosition;
in vec3 vCSPosition;
in vec3 vCSNormal;
in vec2 vTexCoords;

in vec3 vCSEyeDir;

in vec3 vCSPointLightPos;
in vec3 vCSPointLightDir;

in vec3 vCSDirectionalLightDir;

// Sorties
out vec3 fFragColor;

vec3 computeDirectional(vec3 n, vec3 e)
{
	vec3 l = normalize(-vCSDirectionalLightDir);
	vec3 r = reflect(-l,n);
	float cosTheta = clamp(dot(n,l), 0.f, 1.f);
	float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

	vec3 sensibility = uKd * cosTheta + uKs * pow(cosAlpha, uShininess);
	vec3 intensity = uPointLightColor * uPointLightPower;
	return intensity * sensibility;
}

vec3 computePoint(vec3 n, vec3 e)
{
	float distanceFL = distance(vCSPosition, vCSPointLightPos); // distance Fragment-Light

	vec3 l = normalize(-vCSPointLightDir);
	vec3 r = reflect(-l,n);
	float cosTheta = clamp(dot(n,l), 0.f, 1.f);
	float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

	vec3 sensibility = uKd * cosTheta + uKs * pow(cosAlpha, uShininess);
	vec3 intensity = uPointLightColor * uPointLightPower;
	return intensity * sensibility / distanceFL;
}

vec3 computeAmbiant()
{
	return uKa * uAmbiantLightColor * uAmbiantLightPower;
}

void main(void)
{
	vec3 n = normalize(vCSNormal);
	vec3 e = normalize(vCSEyeDir);
	fFragColor = computeDirectional(n, e) +
			computeAmbiant() +
			computePoint(n,e);
}
