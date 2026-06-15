///////////////////////////////////////////////////////////////////////////////
// Scene.h
// ============
// A struct for the collection of data related to the layout of a 3D scene,
// as well as the rendering of that scene.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompositeMeshObject.h"
#include "Light.h"

struct Scene
{
	std::string tag;

	std::vector<CompositeMeshObject> cmos;

	std::vector<Light> lights;
};