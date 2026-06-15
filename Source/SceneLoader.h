///////////////////////////////////////////////////////////////////////////////
// SceneLoader.h
// ============
// A singleton class for the handling of loading and parsing files
// related to a Scene (.SCENE), (.CMO), (.MAT), (.LIGHT)
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
//    CS-499 (Category: Data Structures and Algorithms)  (6/2/2026)
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Scene.h"

class SceneLoader
{
private:
	enum SceneFileType
	{
		SCENE,
		CMO,
		LIGHT,
		MAT
	};

	std::string SceneFileTypeToString(const SceneFileType fileType);

private:
	static SceneLoader* m_sceneLoaderInstance; // singleton-instance

	Scene* m_constructedScene = nullptr;

	// Enables singleton functionality
	SceneLoader() {} 
	SceneLoader(const SceneLoader&) = delete;
	SceneLoader& operator=(const SceneLoader&);

	// Paths
	const std::string path = "../../../Scene/"; // path to all scene files
	const std::string scenesPath = "Scenes/"; // path to .SCENE files
	const std::string cmosPath = "CMOs/"; // path to .CMO files
	const std::string lightsPath = "Lights/"; // path to .LIGHT files
	const std::string matsPath = "Materials/"; // path to .MAT files

	const std::string sceneConfigFileName = "scene_config.txt"; // default scene config filename

	// converts string to float
	float StringToFloat(const std::string& strFloat);

	// converts string to glm::vec3
	glm::vec3 StringToVec3(const std::string& strVec);

	// handles parsed .MAT tokens
	void HandleMatTokens(const std::vector<std::string>& attributeTokens,
						 const std::vector<std::string>& valueTokens);

	// handles parsed .LIGHT tokens
	void HandleLightTokens(const std::vector<std::string>& attributeTokens,
						   const std::vector<std::string>& valueTokens);

	// handles parsed .CMO tokens
	void HandleCMOTokens(const std::vector<std::string>& attributeTokens,
					     const std::vector<std::string>& valueTokens);

	// handles parsed .SCENE tokens
	void HandleSceneTokens(const std::vector<std::string>& attributeTokens,
						   const std::vector<std::string>& valueTokens);

	// finds proper handler method for the file type
	void HandleTokens(const SceneFileType fileType, 
					  const std::vector<std::string>& attributeTokens, 
					  const std::vector<std::string>& valueTokens);

	// parses the various scene-related file types into tokens, thens passes to a handler method
	void ParseFileContents(const std::string& contents, const SceneFileType fileType);

	// opens the file and puts the contents into a single string, lowercased, with no whitespace or comments
	std::string GetFileContents(const std::string filePath, const std::string fileName);

	// starts the process of parsing the scene and any files attached to it
	void LoadScene();

	// gets scene name from config file
	void LoadSceneConfig();

public:
	~SceneLoader()
	{
		delete m_constructedScene;
		delete m_sceneLoaderInstance;
	}

	static SceneLoader* GetInstance(); // gets singleton-instance

	Scene* GetScene(); // returns Scene generated from Scene-related files
};