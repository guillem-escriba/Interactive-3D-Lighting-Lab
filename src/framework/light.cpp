#include "light.h"



Light::Light()
{
	position.set(100, 20,  0);
	diffuse_color.set(0.6f,0.6f,0.6f);
	specular_color.set(0.6f, 0.6f, 0.6f);
}

void Light::addlight(Vector3 p, Vector3 d, Vector3 s) {
	position = p;
	diffuse_color = d;
	specular_color = s;
}


