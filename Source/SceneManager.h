///////////////////////////////////////////////////////////////////////////////
// scenemanager.h
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
// 
//  Modified by: Samuel Williamson
//  FOR:
//    CS - 499 (Category: Software Design and Engineering) (5/31/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"
#include "SceneLoader.h"
#include "Transform.h"
#include "Light.h"
#include "Material.h"

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
	SceneManager(ShaderManager *pShaderManager);
	~SceneManager();

	struct TEXTURE_INFO
	{
		std::string tag;
		uint32_t ID;
	};

private:
	ShaderManager* m_pShaderManager; // provides interfaces that manage shader related data
	SceneLoader* m_sceneLoader; // allows for loading scene (and scene-related) files
	ShapeMeshes* m_basicMeshes; // gives access to basic meshes for rendering
	
	Scene* m_scene; // loaded scene
	int m_loadedTextures; // total number of loaded textures
	TEXTURE_INFO m_textureIDs[16]; // loaded textures info
	std::vector<Material> m_materials; // loaded scene materials

	// load texture images and convert to OpenGL texture data
	bool CreateGLTexture(const char* filename, std::string tag);

	// bind loaded OpenGL textures to slots in memory
	void BindGLTextures();

	// free the loaded OpenGL textures
	void DestroyGLTextures();

	// find a loaded texture by tag
	int FindTextureID(std::string tag);
	int FindTextureSlot(std::string tag);

	// find a defined material by tag
	bool FindMaterial(std::string tag, Material& material);

	glm::mat4 CreateModelView(Transform transform);

	// set the transformation values 
	// into the transform buffer
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// This allows the use of the Transform struct
	void SetTransformations(Transform transform)
	{
		SetTransformations(
			transform.scaleXYZ,
			transform.rotDegXYZ.x,
			transform.rotDegXYZ.y,
			transform.rotDegXYZ.z,
			transform.positionXYZ);
	}

	// set the texture data into the shader
	void SetShaderTexture(std::string textureTag);

	// set the UV scale for the texture mapping
	void SetTextureUVScale(float u, float v);

	// set the object material into the shader
	void SetShaderMaterial(std::string materialTag);

	// loads scene lights
	void LoadSceneLights(const std::vector<Light>& lights);

	// unloads scene lights
	void UnloadSceneLights();
	
	// determines and loads the correct mesh into memory
	void LoadMesh(const Mesh mesh);

public:
	void ReloadScene();
	void PrepareScene();
	void RenderScene();

};