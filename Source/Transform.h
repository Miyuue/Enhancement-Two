///////////////////////////////////////////////////////////////////////////////
// Transform.h
// ============
// Struct for holding position, rotation (in degrees), and scale data 
// related to 3D rendering.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>

struct Transform
{
	glm::vec3 scaleXYZ, rotDegXYZ, positionXYZ;

	/***********************************************************
	*  Reset()
	*
	*  Resets the data to reasonable defaults
	***********************************************************/
	void Reset()
	{
		// Default transform
		positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
		rotDegXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
		scaleXYZ = glm::vec3(1.0f, 1.0f, 1.0f);
	}
};