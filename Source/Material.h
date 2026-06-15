///////////////////////////////////////////////////////////////////////////////
// Material.h
// ============
// Struct for holding material data needed in the shader for 3D rendering.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <string>

struct Material
{
	std::string tag;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;
	float ambientStrength;
};