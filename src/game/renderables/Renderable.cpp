#include "Renderable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <FreeImage.h>
#include "Sphere.h"

Renderable::Renderable(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY)
{
	this->loadModel(modelPath);
	this->init(scaleVector, rotateAngleX, rotateAngleY, rotateAxisX, rotateAxisY, translateVector);
	this->stableModelMatrix = glm::scale(stableModelMatrix, scaleVector);
}

Renderable::Renderable(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY)
{
	this->loadModel(modelPath);
	this->init(scaleVector, rotateAngleX, rotateAngleY, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), translateVector);
	this->stableModelMatrix = glm::scale(stableModelMatrix, scaleVector);
}

Renderable::~Renderable()
{
	destroy();
}

void Renderable::init(GLchar* modelPath, glm::vec3 translateVector, glm::vec3 scaleVector)
{
	this->loadModel(modelPath);
	this->init(scaleVector, 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), translateVector);
	this->stableModelMatrix = glm::scale(glm::mat4(1.0f), scaleVector);
}

void Renderable::init(glm::vec3 position)
{
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->init(glm::vec3(1.0f), 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), position);
}

void Renderable::init(glm::vec3 position, glm::vec3 scale)
{
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->init(scale, 0.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), position);
	this->stableModelMatrix = glm::scale(glm::mat4(1.0f), scale);
}

void Renderable::init(glm::vec3 scaleVector, GLfloat rotateAngleX, GLfloat rotateAngleY, glm::vec3 rotateAxisX, glm::vec3 rotateAxisY, glm::vec3 translateVector)
{
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->translateVector = translateVector;
	this->scaleVector = scaleVector;
	this->rotateAngleX = rotateAngleX;
	this->rotateAngleY = rotateAngleY;
	this->rotateAxisX = rotateAxisX;
	this->rotateAxisY = rotateAxisY;
	this->castingShadow = true;
}

void Renderable::calculateMatricesAndUniforms(Camera & camera, ShaderProgram & shaderProgram)
{
	glm::mat4 projectionMatrix = camera.getProjectionMatrix();
	glm::mat4 viewMatrix = camera.getViewMatrix();

	modelMatrix = glm::translate(glm::mat4(1.0f), translateVector) *
		glm::rotate(glm::mat4(1.0f), rotateAngleX, rotateAxisX) *
		glm::rotate(glm::mat4(1.0f), rotateAngleY, rotateAxisY) * 
		stableModelMatrix;

	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	shaderProgram.setUniform("MVP", MVP);
	shaderProgram.setUniform("M", modelMatrix);
	shaderProgram.setUniform("V", viewMatrix);
}

bool Renderable::hasSubObjects()
{
	return !subObjects.empty();
}

void Renderable::loadModel(std::string modelPath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	this->directory = modelPath.substr(0, modelPath.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
}
void Renderable::processNode(aiNode* node, const aiScene* scene)
{
	for (GLuint i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	for (GLuint i = 0; i < node->mNumChildren; ++i)
	{
		this->processNode(node->mChildren[i], scene);
	}
}
std::unique_ptr<Mesh> Renderable::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Processing vertices
	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
			);
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
			);
		if (mesh->mTextureCoords[0])
		{
			vertex.textureCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
				);
		}
		else
		{
			vertex.textureCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	// Processing indices
	for (GLuint i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Processing textures
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures));
}

std::vector<Texture> Renderable::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		std::string texturePath = "data/models/" + std::string(str.C_Str());
		Texture texture;
		texture.loadTexture2D(texturePath, true);
		texture.type = typeName;
		texture.path = texturePath;
		textures.push_back(texture);
	}
	return textures;
}

bool Renderable::isTransparent() const
{
	float opacity = this->color.a;
	if (opacity == 1.0f) return false;
	return true;
}

void Renderable::render(ShaderProgram & shaderProgram)
{
	if (isTransparent())
	{
		glEnable(GL_BLEND);
		glDepthMask(0);
	}

	// Option1: rendering meshes
	for (GLuint i = 0; i < this->meshes.size(); ++i)
	{
		this->meshes[i]->render(shaderProgram);
	}
	// Option2: custom rendering (customRender must be overriden)
	customRender(shaderProgram);

	if (isTransparent())
	{
		glDisable(GL_BLEND);
		glDepthMask(1);
	}
}

void Renderable::destroy()
{
	glBindVertexArray(this->VAO);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &uvVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &elementVBO);
	glDeleteVertexArrays(1, &VAO);
	glBindVertexArray(0);
}

BoundingSphere Renderable::getBoundingSphere()
{
	return BoundingSphere(translateVector, radius);
}

std::vector<AABB> & Renderable::getBoundingBoxes()
{
	return boundingBoxes;
}