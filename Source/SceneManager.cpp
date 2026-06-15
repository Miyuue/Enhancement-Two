///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
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

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	m_sceneLoader = m_sceneLoader->GetInstance();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
	delete m_sceneLoader;
	m_sceneLoader = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glDeleteTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 * 
 *  Modified by: Samuel Williamson
 *  FOR:
 *    CS-499 (Category: Software Design and Engineering) (5/31/2026)
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, Material& material)
{
	if (m_materials.size() == 0)
	{
		return(false); // No materials to find
	}

	// Search for matching material.tag
	int index = 0;
	bool bFound = false;
	while ((index < m_materials.size()) && (bFound == false))
	{
		if (m_materials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_materials[index].ambientColor;
			material.ambientStrength = m_materials[index].ambientStrength;
			material.diffuseColor = m_materials[index].diffuseColor;
			material.specularColor = m_materials[index].specularColor;
			material.shininess = m_materials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

glm::mat4 SceneManager::CreateModelView(Transform transform)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(transform.scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(transform.rotDegXYZ.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(transform.rotDegXYZ.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(transform.rotDegXYZ.z), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(transform.positionXYZ);

	return translation * rotationZ * rotationY * rotationX * scale;
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);

	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(std::string materialTag)
{
	if (m_materials.size() > 0)
	{
		Material material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  LoadSceneLights()
 *
 *  This method is called to add the passed in light
 *  sources to the 3D scene.  There are up to 4 light sources.
 * 
 *  Modified by: Samuel Williamson
 *  FOR:
 *    CS-499 (Category: Software Design and Engineering) (5/31/2026)
 ***********************************************************/
void SceneManager::LoadSceneLights(const std::vector<Light>& lights)
{
	int i, numOfLights;
	numOfLights = lights.size();

	// No lights to load
	if (numOfLights == 0) return;

	std::cout << "Setting up lighting..." << std::endl;

	for (i = 0; i < numOfLights; i++)
	{
		if (i >= 4) break; // Stop adding lights at 4 light limit.

		const Light& targetLight = lights.at(i);
		std::string targetLightIndex = "lightSources[" + std::to_string(i) + "]";

		m_pShaderManager->setVec3Value(targetLightIndex + ".position", targetLight.position);
		m_pShaderManager->setVec3Value(targetLightIndex + ".ambientColor", targetLight.ambientColor);
		m_pShaderManager->setVec3Value(targetLightIndex + ".diffuseColor", targetLight.diffuseColor);
		m_pShaderManager->setVec3Value(targetLightIndex + ".specularColor", targetLight.specularColor);
		m_pShaderManager->setFloatValue(targetLightIndex + ".focalStrength", targetLight.focalStrength);
		m_pShaderManager->setFloatValue(targetLightIndex + ".specularIntensity", targetLight.specularIntensity);
	}

	m_pShaderManager->setBoolValue("bUseLighting", true); // Enable use of lights
}

/***********************************************************
 *  UnloadSceneLights()
 *
 *  This method is called to unload all Scene lights
 *
 *  Modified by: Samuel Williamson
 *  FOR:
 *    CS-499 (Category: Software Design and Engineering) (5/31/2026)
 ***********************************************************/
void SceneManager::UnloadSceneLights()
{
	glm::vec3 zerod = glm::vec3(0.0f, 0.0f, 0.0f);

	// Loop through all four lights and essentially nullify them.
	for (int i = 0; i < 4; i++)
	{
		std::string targetLightIndex = "lightSources[" + std::to_string(i) + "]";

		m_pShaderManager->setVec3Value(targetLightIndex + ".position", zerod);
		m_pShaderManager->setVec3Value(targetLightIndex + ".ambientColor", zerod);
		m_pShaderManager->setVec3Value(targetLightIndex + ".diffuseColor", zerod);
		m_pShaderManager->setVec3Value(targetLightIndex + ".specularColor", zerod);
		m_pShaderManager->setFloatValue(targetLightIndex + ".focalStrength", 0.0f);
		m_pShaderManager->setFloatValue(targetLightIndex + ".specularIntensity", 0.0f);
	}
}

/***********************************************************
*   LoadMesh()
* 
*   This method is used to find the correct load-mesh function,
*   and then load that mesh.
* 
*  AUTHOR: Samuel Williamson
*  FOR:
*    CS-499 (Category: Software Design and Engineering) (5/31/2026)
***********************************************************/
void SceneManager::LoadMesh(const Mesh mesh)
{
	switch (mesh)
	{
	case BOX:
		m_basicMeshes->LoadBoxMesh();
		break;

	case CONE:
		m_basicMeshes->LoadConeMesh();
		break;

	case CYLINDER:
		m_basicMeshes->LoadCylinderMesh();
		break;

	case PLANE:
		m_basicMeshes->LoadPlaneMesh();
		break;

	case PRISM:
		m_basicMeshes->LoadPrismMesh();
		break;

	case SPHERE:
		m_basicMeshes->LoadSphereMesh();
		break;

	case TAPERED_CYLINDER:
		m_basicMeshes->LoadTaperedCylinderMesh();
		break;

	case TORUS:
		m_basicMeshes->LoadTorusMesh();
		break;

	default:
		std::cout << "Impossible mesh load attempt, something has seriously gone wrong!" << std::endl;
		break;
	}
}

/***********************************************************
*  ReloadScene()
*
*  This method is used for unloading the existing Scene, 
*  then re-preparing the new Scene.
*
*  AUTHOR: Samuel Williamson
*  FOR:
*    CS-499 (Category: Software Design and Engineering) (5/31/2026)
***********************************************************/
void SceneManager::ReloadScene()
{
	// Unload existing Scene data
	m_materials.clear();
	DestroyGLTextures();
	m_basicMeshes->UnloadAllMeshes();
	UnloadSceneLights();

	PrepareScene(); // Load new Scene data
}

/***********************************************************
*  PrepareScene()
*
*  This method is used for preparing the 3D scene by loading
*  the unique scene shapes, textures, and materials into memory 
*  to support the 3D scene rendering
* 
*  AUTHOR: Samuel Williamson
*  FOR:
*    CS-499 (Category: Software Design and Engineering) (5/31/2026)
***********************************************************/
void SceneManager::PrepareScene()
{
	// Get all scene data
	m_scene = m_sceneLoader->GetScene();
	std::cout << std::endl << "----- Setting up Scene -----" << std::endl;

	// Find unique scene data
	std::unordered_set<Mesh> sceneMeshes;
	std::unordered_map<std::string, Material> sceneMaterials;
	std::unordered_set<std::string> sceneTextures;

	for (CompositeMeshObject& cmo : m_scene->cmos)
	{
		std::vector<MeshObject>& mos = cmo.GetMeshObjects();
		for (MeshObject& mo : mos)
		{ // Loop through each MeshObject within each CompositeMeshObject within the Scene
			sceneMeshes.insert(mo.mesh);
			sceneMaterials.insert({ mo.material.tag, mo.material });
			sceneTextures.insert(mo.texture);
		}
	}

	// Load unique meshes
	for (Mesh mesh : sceneMeshes)
	{
		std::cout << "Loading mesh: " << MeshToString(mesh) << std::endl;
		LoadMesh(mesh);
	}

	// Load unique materials
	for (std::pair<std::string, Material> material: sceneMaterials)
	{
		std::cout << "Loading material: " << material.second.tag << std::endl;
		m_materials.push_back(material.second);
	}

	// Load unique textures
	for (std::string texture : sceneTextures)
	{
		std::cout << "Loading texture: " << texture << std::endl;
		std::string path = "../../../Scene/Textures/" + texture + ".jpg";
		CreateGLTexture(path.c_str(), texture);
	}
	BindGLTextures();

	// Set up lighting
	LoadSceneLights(m_scene->lights);

	std::cout << "Scene Loaded: " << m_scene->tag << std::endl;
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 * 
 *  AUTHOR: Samuel Williamson
 *  FOR:
 *    CS-499 (Category: Software Design and Engineering) (5/31/2026)
 ***********************************************************/
void SceneManager::RenderScene()
{
	for (CompositeMeshObject& cmo : m_scene->cmos)
	{
		std::vector<MeshObject>& mos = cmo.GetMeshObjects();
		for (MeshObject& mo : mos) 
		{ // Loop through each MeshObject within each CompositeMeshObject within the Scene
			SetTransformations(mo.transform);
			SetShaderTexture(mo.texture);
			SetShaderMaterial(mo.material.tag);

			switch (mo.mesh)
			{
			case BOX:
				m_basicMeshes->DrawBoxMesh();
				break;

			case CONE:
				m_basicMeshes->DrawConeMesh();
				break;

			case CYLINDER:
				m_basicMeshes->DrawCylinderMesh();
				break;

			case PLANE:
				m_basicMeshes->DrawPlaneMesh();
				break;

			case PRISM:
				m_basicMeshes->DrawPrismMesh();
				break;

			case SPHERE:
				m_basicMeshes->DrawSphereMesh();
				break;

			case TAPERED_CYLINDER:
				m_basicMeshes->DrawTaperedCylinderMesh();
				break;

			case TORUS:
				m_basicMeshes->DrawTorusMesh();
				break;

			default:
				std::cout << "Impossible mesh draw attempt, something has seriously gone wrong!" << std::endl;
				break;
			}
		}
	}
}
