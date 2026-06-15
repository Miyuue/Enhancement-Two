///////////////////////////////////////////////////////////////////////////////
// MeshObject.h
// ============
// Struct for containing the typical data that would be associated with
// rendering a mesh.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>

#include "Transform.h"
#include "Material.h"

enum Mesh
{
	INVALID,
	BOX,
	CONE,
	CYLINDER,
	PLANE,
	PRISM,
	SPHERE,
	TAPERED_CYLINDER,
	TORUS
};

static Mesh StringToMesh(const std::string meshStr)
{
	if (meshStr == "box") return BOX;
	if (meshStr == "cone") return CONE;
	if (meshStr == "cylinder") return CYLINDER;
	if (meshStr == "plane") return PLANE;
	if (meshStr == "prism") return PRISM;
	if (meshStr == "sphere") return SPHERE;
	if (meshStr == "tapered_cylinder") return TAPERED_CYLINDER;
	if (meshStr == "torus") return TORUS;

	return INVALID;
};

static const std::string MeshToString(const Mesh mesh)
{
	if (mesh == BOX) return "box";
	if (mesh == CONE) return "cone";
	if (mesh == CYLINDER) return "cylinder";
	if (mesh == PLANE) return "plane";
	if (mesh == PRISM) return "prism";
	if (mesh == SPHERE) return "sphere";
	if (mesh == TAPERED_CYLINDER) return "tapered_cylinder";
	if (mesh == TORUS) return "torus";

	return "invalid mesh";
}

struct MeshObject
{
	Transform transform;
	Mesh mesh;
	Material material;
	std::string texture;

	/***********************************************************
	*  MeshObject()
	*
	*  Constructor that initializes all variables
	*  in a way to make it a flat-white cube as a default.
	***********************************************************/
	MeshObject()
	{
		this->transform.Reset();
		this->mesh = BOX;
		this->texture = "flat";

		Material mat;
		mat.tag = "Default";
		mat.ambientColor = glm::vec3(0.75f, 0.75f, 0.75f);
		mat.diffuseColor = glm::vec3(0.75f, 0.75f, 0.75f);
		mat.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
		mat.ambientStrength = 1.0f;
		mat.shininess = 0.1f;

		this->material = mat;
	}
};