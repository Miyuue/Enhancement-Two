///////////////////////////////////////////////////////////////////////////////
// CompositeMeshObject.cpp
// ============
// A class for collecting various MeshObjects into one Composite,
// ensuring all collected MeshObjects move according to the central-transform.
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#include "CompositeMeshObject.h"

/***********************************************************
*  CompositeMeshObject()
*
*  The constructor for the class
***********************************************************/
CompositeMeshObject::CompositeMeshObject()
{
	m_transform.Reset();
}

/***********************************************************
*  AddMeshObject()
*
*  Adds passed in MeshObject to the children vector
***********************************************************/
void CompositeMeshObject::AddMeshObject(MeshObject child)
{
	m_children.push_back(child);
}

/***********************************************************
*  GetMeshObjects()
*
*  Returns a copy of the children-MeshObject vector
***********************************************************/
std::vector<MeshObject>& CompositeMeshObject::GetMeshObjects()
{
	return m_children;
}

/***********************************************************
*  SetTransform()
*
*  Moves all children MeshObject positions relative to this
*  CompositeMeshObject's position.
*
*  Should only be used when all MeshObjects are finished being
*  added to this CompositeMeshObject.
***********************************************************/
void CompositeMeshObject::SetTransform(Transform transform)
{
	// Set each child MeshObject's position relative to the composite's position
	for (MeshObject& child : m_children)
	{
		child.transform.positionXYZ += transform.positionXYZ;
	}

	this->m_transform = transform; // Set the central CompositeMeshObjects transform
}