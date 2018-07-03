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

uniform bool uUseKaTexture;
uniform bool uUseKdTexture;
uniform bool uUseKsTexture;
uniform bool uUseNormalTexture;

uniform sampler2D uKaTexture;
uniform sampler2D uKdTexture;
uniform sampler2D uKsTexture;
uniform sampler2D uNormalTexture;

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

vec3 computeDirectional(vec3 n, vec3 e, vec3 kd, vec3 ks)
{
	vec3 l = normalize(-vCSDirectionalLightDir);
	vec3 r = reflect(-l,n);
	float cosTheta = clamp(dot(n,l), 0.f, 1.f);
	float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

	vec3 sensibility = kd * cosTheta + ks * pow(cosAlpha, uShininess);
	vec3 intensity = uDirectionalLightColor * uDirectionalLightPower;
	return intensity * sensibility;
}

vec3 computePoint(vec3 n, vec3 e, vec3 kd, vec3 ks)
{
	float distanceFL = distance(vCSPosition, vCSPointLightPos); // distance Fragment-Light
	distanceFL = pow(distanceFL,1.5f);

	vec3 l = normalize(-vCSPointLightDir);
	vec3 r = reflect(-l,n);
	float cosTheta = clamp(dot(n,l), 0.f, 1.f);
	float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

	vec3 sensibility = kd * cosTheta + ks * pow(cosAlpha, uShininess);
	vec3 intensity = uPointLightColor * uPointLightPower;
	return intensity * sensibility / distanceFL;
}

vec3 computeAmbiant(vec3 ka)
{
	return ka * uAmbiantLightColor * uAmbiantLightPower;
}

void main(void)
{
	vec3 n = normalize(vCSNormal);
	vec3 e = normalize(vCSEyeDir);

	vec3 ka = uKa;
	vec3 kd = uKd;
	vec3 ks = uKs;

	if (uUseKaTexture)
		ka *= texture2D(uKaTexture, vTexCoords).xyz;
	if (uUseKdTexture)
		kd *= texture2D(uKdTexture, vTexCoords).xyz;
	if (uUseKsTexture)
		ks *= texture2D(uKsTexture, vTexCoords).xyz;

	fFragColor = computeDirectional(n,e,kd,ks) +
			computeAmbiant(ka) +
			computePoint(n,e,kd,ks);
}
