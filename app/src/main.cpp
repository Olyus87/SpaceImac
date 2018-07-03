#include <iostream>
#include <glimac/Sphere.hpp>
#include <glimac/Geometry.hpp>

#include "spacimac.h"
#include "scene.h"
#include "renderer.h"

int main(int argc, char** argv) {

	SpacImac spacimac(argc, argv, "SpacImac");

	TextureAndLightRenderer renderer;
	spacimac.setRenderer(&renderer);

	return spacimac.exec();
}
