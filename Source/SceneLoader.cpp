///////////////////////////////////////////////////////////////////////////////
// SceneLoader.cpp
// ============
// A singleton class for the handling of loading and parsing files
// related to a Scene (.SCENE), (.CMO), (.MAT), (.LIGHT)
// 
//  AUTHOR: Samuel Williamson
//  FOR:
//    CS-499 (Category: Software Design and Engineering) (5/31/2026)
//    CS-499 (Category: Data Structures and Algorithms)  (6/2/2026)
///////////////////////////////////////////////////////////////////////////////

#include "SceneLoader.h"

#include <fstream>
#include <iostream>

SceneLoader* SceneLoader::m_sceneLoaderInstance; // singleton-instance

/***********************************************************
*  SceneFileTypeToString()
*
*  This is a simple helper method for finding and returning
*  the string representation of a SceneFileType.
***********************************************************/
std::string SceneLoader::SceneFileTypeToString(const SceneFileType fileType)
{
	switch (fileType)
	{
	case SCENE:
		return ".SCENE";
		break;

	case CMO:
		return ".CMO";
		break;

	case LIGHT:
		return ".LIGHT";
		break;

	case MAT:
		return ".MAT";
		break;
	}
}

/***********************************************************
*  StringToFloat()
*
*  This is a simple helper method for converting a string
*  that is a float (such as: "1.0") into a float. This also
*  provides a default float if an error occurs.
***********************************************************/
float SceneLoader::StringToFloat(const std::string& strFloat)
{
	float val;

	try
	{
		val = std::stof(strFloat);
	}
	catch (...)
	{
		std::cout << "Could not convert " << strFloat << " to usable data. Using default (0.0)" << std::endl;
		return 0.0f;
	}

	return val;
}

/***********************************************************
*  StringToVec3()
*
*  This is a simple helper method for converting a string
*  that is a glm::vec3 (such as: "1.0,1.0,1.0") into a glm::vec3.
*  This also provides a default glm::vec3 if an error occurs.
***********************************************************/
glm::vec3 SceneLoader::StringToVec3(const std::string& strVec)
{
	glm::vec3 constructedVec3 = glm::vec3();
	std::string floatVal;

	int constructedCount = 0;
	for (char c : strVec)
	{
		// There should only be 3 values found, anything extra is ignored
		if (constructedCount > 2) break;

		if (c == ',')
		{
			if (constructedCount == 0) constructedVec3.x = StringToFloat(floatVal);
			else if (constructedCount == 1) constructedVec3.y = StringToFloat(floatVal);

			constructedCount++;
			floatVal.clear();
		}
		else
		{
			floatVal += c;
		}
	}

	// Make sure at least three values were declared
	if (constructedCount < 2)
	{
		std::cout << "Could not convert " << strVec << " to usable data. Using default (0.0, 0.0, 0.0)" << std::endl;
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	constructedVec3.z = StringToFloat(floatVal);

	return constructedVec3;
}

/***********************************************************
*  HandleMatTokens()
*
*  This method handles the parsed out tokens in a .MAT file.
*  It finds every Material attribute and applies it to the
*  Material in the most recently created MeshObject in the
*  most recently created CompositeMeshObject in m_constructedScene.
***********************************************************/
void SceneLoader::HandleMatTokens(const std::vector<std::string>& attributeTokens, const std::vector<std::string>& valueTokens)
{
	int len = attributeTokens.size();
	bool foundAmbient = false;
	bool foundDiffuse = false;
	bool foundSpecular = false;
	bool foundAmbientStrength = false;
	bool foundShininess = false;

	// Get most recent CMO's most recent MeshObject's material for assignment
	Material& mat = m_constructedScene->cmos.back().GetMeshObjects().back().material;

	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "ambient")
		{
			foundAmbient = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			mat.ambientColor = val;
		}
		else if (attributeTokens.at(i) == "diffuse")
		{
			foundDiffuse = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			mat.diffuseColor = val;
		}
		else if (attributeTokens.at(i) == "specular")
		{
			foundSpecular = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			mat.specularColor = val;
		}
		else if (attributeTokens.at(i) == "ambient_strength")
		{
			foundAmbientStrength = true;
			float val = StringToFloat(valueTokens.at(i));
			mat.ambientStrength = val;
		}
		else if (attributeTokens.at(i) == "shininess")
		{
			foundShininess = true;
			float val = StringToFloat(valueTokens.at(i));
			mat.shininess = val;
		}
	}

	// Error if missing even one
	if (!foundAmbient || !foundDiffuse || !foundSpecular ||
		!foundAmbientStrength || !foundShininess)
	{
		throw std::string("Missing or misnamed attribute in .MAT file");
	}
}

/***********************************************************
*  HandleLightTokens()
*
*  This method handles the parsed out tokens in a .LIGHT file.
*  It finds every Light attribute and applies it to the
*  most recently created Light in m_constructedScene.
***********************************************************/
void SceneLoader::HandleLightTokens(const std::vector<std::string>& attributeTokens, const std::vector<std::string>& valueTokens)
{
	int len = attributeTokens.size();
	bool foundAmbient = false;
	bool foundDiffuse = false;
	bool foundSpecular = false;
	bool foundFocalStrength = false;
	bool foundSpecularIntensity = false;

	// Get most recent Light for assignment
	Light& light = m_constructedScene->lights.back();

	// Find all attributes and assign to light
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "ambient")
		{
			foundAmbient = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			light.ambientColor = val;
		}
		else if (attributeTokens.at(i) == "diffuse")
		{
			foundDiffuse = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			light.diffuseColor = val;
		}
		else if (attributeTokens.at(i) == "specular")
		{
			foundSpecular = true;
			glm::vec3 val = StringToVec3(valueTokens.at(i));
			light.specularColor = val;
		}
		else if (attributeTokens.at(i) == "focal_strength")
		{
			foundFocalStrength = true;
			float val = StringToFloat(valueTokens.at(i));
			light.focalStrength = val;
		}
		else if (attributeTokens.at(i) == "specular_intensity")
		{
			foundSpecularIntensity = true;
			float val = StringToFloat(valueTokens.at(i));
			light.specularIntensity = val;
		}
	}

	// Error if missing even one
	if (!foundAmbient || !foundDiffuse || !foundSpecular ||
		!foundFocalStrength || !foundSpecularIntensity)
	{
		throw std::string("Missing or misnamed attribute in .LIGHT file");
	}
}

/***********************************************************
*  HandleCMOTokens()
*
*  This method handles the parsed out tokens in a .CMO file.
*  It finds each avaliable attribute and applies it to the
*  most recently created MeshObject in the most recently
*  created CompositeMeshObject in m_constructedScene.
***********************************************************/
void SceneLoader::HandleCMOTokens(const std::vector<std::string>& attributeTokens, const std::vector<std::string>& valueTokens)
{
	MeshObject mo = MeshObject(); // MeshObject to be constructed
	int len = attributeTokens.size();

	// Find mesh attribute
	bool noMesh = true;
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "mesh")
		{
			Mesh meshFound = StringToMesh(valueTokens.at(i));

			if (meshFound == INVALID) break; // Found non-existant mesh

			// Assign found mesh
			noMesh = false;
			mo.mesh = meshFound;
			break;
		}
	}

	// No mesh error
	if (noMesh) throw std::string("No mesh or Invalid mesh found for declared mesh");

	// Find position attribute (if declared)
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "position")
		{
			// Assign found position
			mo.transform.positionXYZ = StringToVec3(valueTokens.at(i));
			break;
		}
	}

	// Find scale attribute (if declared)
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "scale")
		{
			// Assign found scale
			mo.transform.scaleXYZ = StringToVec3(valueTokens.at(i));
			break;
		}
	}

	// Find rotation attribute (if declared)
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "rotation")
		{
			// Assign found rotation
			mo.transform.rotDegXYZ = StringToVec3(valueTokens.at(i));
			break;
		}
	}

	// Find texture attribute (if declared)
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "texture")
		{
			// Assign found texture
			mo.texture = valueTokens.at(i);
			break;
		}
	}

	// Add newly constructed MeshObject to newest CMO
	m_constructedScene->cmos.back().AddMeshObject(mo);

	// Find material attribute (if declared)
	// (This has to be after the MeshObject is added to the most recent CMO
	// because the .MAT needs to be parsed, meaning the material will be added
	// to the most recent MeshObject in the most recent CMO in HandleMatTokens()
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "material")
		{
			// Open and parse found .MAT file
			std::string fileName = valueTokens.at(i);
			m_constructedScene->cmos.back().GetMeshObjects().back().material.tag = fileName; // Set new material name
			std::string matContents = GetFileContents(matsPath, fileName + ".mat"); // Read file
			ParseFileContents(matContents, SceneFileType::MAT); // Parse file
			break;
		}
	}
}

/***********************************************************
*  HandleSceneTokens()
*
*  This method handles the parsed out tokens in a .SCENE file.
*  It handles the calls to parse the .LIGHT and .CMO files
*  associated with the scene, as well as set their positions.
***********************************************************/
void SceneLoader::HandleSceneTokens(const std::vector<std::string>& attributeTokens, const std::vector<std::string>& valueTokens)
{
	int len = attributeTokens.size();
	std::string typeToken, positionToken;

	// Find the main type of the declaration, either Light or CMO
	for (int i = 0; i < len; i++)
	{
		std::string token = attributeTokens.at(i);

		// Open and parse the declared file
		if (token == "light")
		{
			m_constructedScene->lights.push_back(Light()); // New Light to be defined

			std::string fileName = valueTokens.at(i);
			std::string lightContents = GetFileContents(lightsPath, fileName + ".light");
			ParseFileContents(lightContents, SceneFileType::LIGHT);

			typeToken = token;
			break;
		}
		else if (token == "cmo")
		{
			m_constructedScene->cmos.push_back(CompositeMeshObject()); // New CMO to be defined

			std::string fileName = valueTokens.at(i);
			std::string cmoContents = GetFileContents(cmosPath, fileName + ".cmo");
			ParseFileContents(cmoContents, SceneFileType::CMO);

			typeToken = token;
			break;
		}
	}

	// No Lights or CMOs declared in .SCENE file
	if (typeToken.empty())
		throw std::string("No LIGHT or CMO declared in .SCENE file");

	// Find the position attribute
	for (int i = 0; i < len; i++)
	{
		if (attributeTokens.at(i) == "position")
		{
			positionToken = valueTokens.at(i);
			break;
		}
	}

	// Set newest type's position if declared, otherwise it will retain the default
	if (!positionToken.empty())
	{
		if (typeToken == "light")
			m_constructedScene->lights.back().position = StringToVec3(positionToken);
		else
		{
			Transform transform = Transform();
			transform.positionXYZ = StringToVec3(positionToken);
			m_constructedScene->cmos.back().SetTransform(transform);
		}
	}
}

/***********************************************************
*  HandleTokens()
*
*  This method simply determines the proper handler method
*  for the passed in file type
***********************************************************/
void SceneLoader::HandleTokens(const SceneFileType fileType, const std::vector<std::string>& attributeTokens, const std::vector<std::string>& valueTokens)
{
	switch (fileType)
	{
	case SCENE:
		HandleSceneTokens(attributeTokens, valueTokens);
		break;

	case CMO:
		HandleCMOTokens(attributeTokens, valueTokens);
		break;

	case LIGHT:
		HandleLightTokens(attributeTokens, valueTokens);
		break;

	case MAT:
		HandleMatTokens(attributeTokens, valueTokens);
		break;
	}
}

/***********************************************************
*  ParseFileContents()
*
*  This method takes in a file's contents (one-lined, lowercased, no whitespace or comments)
*  and the file type to be parsed, then begins extracting the declared
*  attributes and their values, all while ensuring the files are in the
*  proper format. 
*  After parsing, it then passes the attributes and values to the
*  proper handler method for further processing.
***********************************************************/
void SceneLoader::ParseFileContents(const std::string& contents, const SceneFileType fileType)
{
	bool goodFormat = true; // Is the file formatting good so far?
	bool declaringAttribute = false; // Is an attribute getting declared at the moment?
	bool definingValue = false; // Is a value getting defined at the moment?
	bool readyForValue = false; // Has a attribute been properly declared before attempting to define a value?

	std::string tokenStr; // used for construction of tokens
	std::vector<std::string> attributeTokens, valueTokens; // stores the constructed tokens, in the order found

	// Begin parsing
	for (char c : contents)
	{
		if (!goodFormat) break; // Broken formatting detected, quit parsing

		switch (c)
		{
		// Start of attributes
		case '{':
			// Ensure we aren't trying to start declaring attributes in the middle of already declaring attributes
			if (declaringAttribute || definingValue || readyForValue)
			{
				goodFormat = false;
				break;
			}

			declaringAttribute = true;
			attributeTokens.clear();
			valueTokens.clear();
			break;

		// End of attributes
		case '}':
			// Ensure we had already started declaring attributes, and we weren't in the middle of defining a value
			if (!declaringAttribute || definingValue || readyForValue)
			{
				goodFormat = false;
				break;
			}

			declaringAttribute = false; // Finished declaring attributes

			// There should be a value per attribute
			if (attributeTokens.size() != valueTokens.size())
			{
				throw std::string("PARSING ERROR: Number of attributes doesn't match the number of values!");
			}

			HandleTokens(fileType, attributeTokens, valueTokens);

			break;

		// Start defining value token
		case '(':
			// Ensure attribute was properly declared and we weren't already defining a value
			if (!readyForValue || definingValue)
			{
				goodFormat = false;
				break;
			}

			definingValue = true;
			readyForValue = false;

			break;

		// Finished defining value token
		case ')':
			// Ensure an value was defined when it was supposed to be
			if (!definingValue || tokenStr.empty())
			{
				goodFormat = false;
				break;
			}

			valueTokens.push_back(tokenStr); // Save defined value
			tokenStr.clear(); // Prepare for next token construction

			definingValue = false; // Finished defining value
			declaringAttribute = true; // Resume declaring attributes (if any)

			break;

		// Attribute token completed constructing
		case ':':
			// Ensure an attribute was declared when it was supposed to be
			if (!declaringAttribute || tokenStr.empty())
			{
				goodFormat = false;
				break;
			}

			attributeTokens.push_back(tokenStr); // Save declared attribute
			tokenStr.clear(); // Prepare for next token construction

			// Finished declaring attribute token
			declaringAttribute = false;
			readyForValue = true;

			break;

		// Ignore commas unless defining a value
		case ',':
			if (definingValue) // add comma to token if building a value
			{
				tokenStr += c;
				break;
			}
			break;

		// Constructing token
		default:
			// Check if we're constructing when we're not supposed to
			if ((!declaringAttribute && !definingValue) || 
				(declaringAttribute && definingValue))
			{
				goodFormat = false;
				break;
			}

			tokenStr += c;
			break;
		}
	}
	
	// Bad format detected while parsing
	if (!goodFormat)
		throw SceneFileTypeToString(fileType).append(" file format error detected!");
}

/***********************************************************
*  GetFileContents()
*
*  Attempts to open the specified file, in order to
*  load its contents into one line, without comments or whitespace,
*  all lower-cased.
***********************************************************/
std::string SceneLoader::GetFileContents(const std::string filePath, const std::string fileName)
{
	// Open file
	std::ifstream file(path + filePath + fileName);

	// Check if file opened
	if (!file.is_open())
	{
		// Handle error
		file.close();
		throw "Couldn't open " + fileName;
	}

	// Check if file is empty
	if (file.eof())
	{
		// Handle error
		file.close();
		throw fileName + " file is empty";
	}

	// Read entire file into one string, omitting whitespace and comments, lowercasing each character
	std::string contents;

	while (!file.eof())
	{
		std::string line, trimmed;

		std::getline(file, line); // Read line

		for (int i = 0, len = line.length(); i < len; i++)
		{
			if (line[i] == '/') break; // Omit comments, go to next line

			if (std::isspace((unsigned char)line[i])) continue; // Omit whitespace, go to next character

			trimmed += std::tolower(line[i]); // store lowercased character
		}
		contents.append(trimmed);
	}

	file.close();
	return contents;
}

/***********************************************************
*  LoadScene()
*
*  Starts the recursive process of loading the scene related files
*  into m_constructedScene
***********************************************************/
void SceneLoader::LoadScene()
{
	// Get scene contents without comments or whitespace, and lowercased
	std::string fileName = m_constructedScene->tag;
	std::string sceneContents = GetFileContents(scenesPath, fileName + ".scene");

	ParseFileContents(sceneContents, SceneFileType::SCENE);
}

/***********************************************************
*  LoadSceneConfig()
*
*  Attempts to open the scene config file, in order to
*  get the name of the scene needing to be loaded.
***********************************************************/
void SceneLoader::LoadSceneConfig()
{
	// Open scene config file
	std::ifstream sceneConfigFile(path + sceneConfigFileName);

	// Check if file opened
	if (!sceneConfigFile.is_open())
	{
		// Handle error
		sceneConfigFile.close();
		throw std::string("Couldn't open scene_config.txt");
	}

	// Check if file is empty
	if (sceneConfigFile.eof())
	{
		// Handle error
		sceneConfigFile.close();
		throw sceneConfigFileName + " file is empty";
	}

	// Get scene name
	sceneConfigFile >> m_constructedScene->tag; // Only reads first line

	sceneConfigFile.close(); // Finished
}

/***********************************************************
*  GetInstance()
*
*  Returns the singleton instance if it exists,
*  or creates the instance then returns it.
***********************************************************/
SceneLoader* SceneLoader::GetInstance()
{
	if (m_sceneLoaderInstance == nullptr)
	{
		m_sceneLoaderInstance = new SceneLoader();
	}

	return m_sceneLoaderInstance;
}

/***********************************************************
*  GetScene()
*
*  This method tries to construct the Scene from the user-created files 
*  (with help from other methods), then returns the result.
***********************************************************/
Scene* SceneLoader::GetScene()
{
	std::cout << std::endl << "----- Loading Scene -----" << std::endl;
	m_constructedScene = new Scene();

	try
	{
		LoadSceneConfig();
		LoadScene();
	}
	catch (std::string msg)
	{
		std::cout << msg << std::endl;
	}

	return m_constructedScene;
}
