///////////////////////////////////////////////////////////////////////////////
// Light.h
// ============
// Struct for holding light data needed in the shader for 3D rendering with lighting.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>

struct Light
{
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float focalStrength;
	float specularIntensity;
};