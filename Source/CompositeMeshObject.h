///////////////////////////////////////////////////////////////////////////////
// CompositeMeshObject.h
// ============
// A class for collecting various MeshObjects into one Composite,
// ensuring all collected MeshObjects move according to the central-transform.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <string>

#include "MeshObject.h"

class CompositeMeshObject
{
private:
	Transform m_transform;
	std::vector<MeshObject> m_children;

public:
	CompositeMeshObject();

	void AddMeshObject(MeshObject child);

	std::vector<MeshObject>& GetMeshObjects();

	void SetTransform(Transform transform);
};