#include "Terrain.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <FreeImage.h>

#include "GameScreen.h"

ShaderProgram Terrain::terrain;
Shader Terrain::terrainShaders[NUMTERRAINSHADERS];

Terrain::~Terrain()
{
	destroy();
}

void Terrain::init()
{
	this->translateVector = glm::vec3(0.0f);
	this->rotateAngleX = 0.0f;
	this->rotateAngleY = 0.0f;
	this->rotateAxisX = glm::vec3(1.0f, 0.0f, 0.0f);
	this->rotateAxisY = glm::vec3(0.0f, 1.0f, 0.0f);
	this->scaleVector = glm::vec3(1.0f);

	color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	stableModelMatrix = glm::mat4(1.0f);

	loadHeightMapFromImage("data/worlds/world.tga");

	// Load textures
	std::string textureNames[] = { "snow.jpg", "stone.jpg", "snow.jpg" };

	for(int i = 0; i < 3; ++i)
	{
		textures[i].loadTexture2D("data/textures/" + textureNames[i], true);
		textures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
		textures[i].setSamplerParameter(GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}

}

void Terrain::customRender(ShaderProgram & shaderProgram)
{
	glBindVertexArray(VAO);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(rows*cols);

	int numIndices = (rows-1)*cols*2 + rows-1;
	glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);
}

void Terrain::update(float deltaTime)
{

}

void Terrain::destroy()
{
	releaseHeightmap();
}

bool Terrain::loadTerrainShaderProgram()
{
	bool isOK = true;
	isOK &= terrainShaders[0].loadShader("data/shaders/terrain.vert", GL_VERTEX_SHADER);
	isOK &= terrainShaders[1].loadShader("data/shaders/terrain.frag", GL_FRAGMENT_SHADER);

	terrain.createProgram();
	for(int i = 0; i < NUMTERRAINSHADERS; ++i) terrain.addShaderToProgram(&terrainShaders[i]);
	terrain.linkProgram();

	return isOK;
	
}

void Terrain::setRenderSize(float renderX, float height, float renderZ)
{
	renderScale = glm::vec3(renderX, height, renderZ);
}

void Terrain::setRenderSize(float quadSize, float height)
{
	renderScale = glm::vec3(float(cols)*quadSize, height, float(rows)*quadSize);
}

bool Terrain::loadHeightMapFromImage(std::string imagePath)
{
	if(loaded)
	{
		loaded = false;
		releaseHeightmap();
	}
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(imagePath.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(imagePath.c_str());

	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, imagePath.c_str());
	if(!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data
	rows = FreeImage_GetHeight(dib);
	cols = FreeImage_GetWidth(dib);

	terrainHeightValues.resize(rows);
	for (int i = 0; i < rows; ++i) terrainHeightValues[i].resize(cols);

	if(bDataPointer == nullptr || rows == 0 || cols == 0 || (FreeImage_GetBPP(dib) != 24 && FreeImage_GetBPP(dib) != 16 && FreeImage_GetBPP(dib) != 8))
		return false;

	unsigned int ptr_inc = FreeImage_GetBPP(dib) == 24 ? 3 : FreeImage_GetBPP(dib) == 16 ? 2 : 1;
	unsigned int row_step = ptr_inc*cols;

	std::vector< std::vector< glm::vec3> > vertexData(rows, std::vector<glm::vec3>(cols));
	std::vector< std::vector< glm::vec2> > coordsData(rows, std::vector<glm::vec2>(cols));

	float textureU = float(cols)*0.1f;
	float textureV = float(rows)*0.1f;
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			float scaleC = float(j)/float(cols-1);
			float scaleR = float(i)/float(rows-1);
			float vertexHeight = float(*(bDataPointer + row_step*i + j*ptr_inc)) / 255.0f;
			vertexData[i][j] = glm::vec3(-0.5f+scaleR, vertexHeight, -0.5f+scaleC);
			coordsData[i][j] = glm::vec2(textureU*scaleR, textureV*scaleC);
		}
	}

	// Smoothing terrain
	for (int iteration = 0; iteration < 1; ++iteration);
	{
		float k = 0.75f;
		for (int i = 1; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			vertexData[i][j].y = vertexData[i - 1][j].y * (1 - k) + vertexData[i][j].y * k;

		for (int i = rows - 2; i < -1; --i)
		for (int j = 0; j < cols; ++j)
			vertexData[i][j].y = vertexData[i + 1][j].y * (1 - k) + vertexData[i][j].y * k;

		for (int i = 0; i < rows; ++i)
		for (int j = 1; j < cols; ++j)
			vertexData[i][j].y = vertexData[i][j - 1].y * (1 - k) + vertexData[i][j].y * k;

		for (int i = 0; i < rows; ++i)
		for (int j = cols; j < -1; --j)
		{
			vertexData[i][j].y = vertexData[i][j + 1].y * (1 - k) + vertexData[i][j].y * k;
		}
	}
	// Assigning terrain height values
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			terrainHeightValues[i][j] = vertexData[i][j].y;

	// Normals are here - the heightmap contains ( (rows-1)*(cols-1) quads, each one containing 2 triangles, therefore array of we have 3D array)
	std::vector< std::vector<glm::vec3> > normals[2];
	for(int i = 0; i < 2; ++i)
	{
		normals[i] = std::vector< std::vector<glm::vec3> >(rows-1, std::vector<glm::vec3>(cols-1));
	}

	for(int i = 0; i < rows-1; ++i)
	{
		for(int j = 0; j < cols-1; ++j)
		{
			glm::vec3 triangle0[] =
			{
				vertexData[i][j],
				vertexData[i][j + 1],
				vertexData[i + 1][j + 1]
			};

			glm::vec3 triangle1[] = 
			{
				vertexData[i+1][j+1],
				vertexData[i+1][j],
				vertexData[i][j]
			};

			glm::vec3 triangleNorm0 = glm::cross(triangle0[0]-triangle0[1], triangle0[1]-triangle0[2]);
			glm::vec3 triangleNorm1 = glm::cross(triangle1[0]-triangle1[1], triangle1[1]-triangle1[2]);

			normals[0][i][j] = glm::normalize(triangleNorm0);
			normals[1][i][j] = glm::normalize(triangleNorm1);
		}
	}

	std::vector< std::vector<glm::vec3> > finalNormals = std::vector< std::vector<glm::vec3> >(rows, std::vector<glm::vec3>(cols));

	for(int i = 0; i < rows; ++i)
	for(int j = 0; j < cols; ++j)
	{
		glm::vec3 finalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

		// Look for upper-left triangles
		if(j != 0 && i != 0)
			for(int k = 0; k < 2; ++k) finalNormal += normals[k][i-1][j-1];
		// Look for upper-right triangles
		if(i != 0 && j != cols-1)finalNormal += normals[0][i-1][j];
		// Look for bottom-right triangles
		if(i != rows-1 && j != cols-1)
			for(int k = 0; k < 2; ++k) finalNormal += normals[k][i][j];
		// Look for bottom-left triangles
		if(i != rows-1 && j != 0)
			finalNormal += normals[1][i][j-1];

		finalNormal.x += 0.00001f;
		finalNormal = glm::normalize(finalNormal);

		finalNormals[i][j] = finalNormal;
	}

	// First, create a VBO with only vertex data
	heightmapDataVBO.createVBO(rows*cols*(2*sizeof(glm::vec3)+sizeof(glm::vec2))); // Preallocate memory
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			heightmapDataVBO.addData(&vertexData[i][j], sizeof(glm::vec3));
			heightmapDataVBO.addData(&coordsData[i][j], sizeof(glm::vec2)); 
			heightmapDataVBO.addData(&finalNormals[i][j], sizeof(glm::vec3));
		}
	}

	// Now create a VBO with heightmap indices
	heightmapIndicesVBO.createVBO();
	int primitiveRestartIndex = rows*cols;
	for(int i = 0; i < rows-1; ++i)
	{
		for(int j = 0; j < cols; ++j)
		for(int k = 0; k < 2; ++k)
		{
			int row = i+(1-k);
			int index = row*cols+j;
			heightmapIndicesVBO.addData(&index, sizeof(int));
		}
		// Restart triangle strips
		heightmapIndicesVBO.addData(&primitiveRestartIndex, sizeof(int));
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	heightmapDataVBO.bindVBO();
	heightmapDataVBO.uploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));

	heightmapIndicesVBO.bindVBO(GL_ELEMENT_ARRAY_BUFFER);
	heightmapIndicesVBO.uploadDataToGPU(GL_STATIC_DRAW);

	FreeImage_Unload(dib);

	loaded = true; // If get here, we succeeded with generating heightmap
	return true;
}

void Terrain::releaseHeightmap()
{
	if(!loaded)
		return; // Heightmap must be loaded
	heightmapDataVBO.deleteVBO();
	heightmapIndicesVBO.deleteVBO();
	glDeleteVertexArrays(1, &VAO);
	loaded = false;
}

ShaderProgram* Terrain::getShaderProgram()
{
	return &terrain;
}

void Terrain::releaseTerrainShaderProgram()
{
	terrain.deleteProgram();
	for(int i = 0; i < NUMTERRAINSHADERS; ++i) terrainShaders[i].deleteShader();
}

int Terrain::getNumHeightmapRows()
{
	return rows;
}

int Terrain::getNumHeightmapCols()
{
	return cols;
}

GLfloat Terrain::getTerrainHeight(GLfloat xCoordWorldspace, GLfloat zCoordWorldspace)
{
	// TODO: LERP
	GLfloat xS = xCoordWorldspace / renderScale.x;
	GLfloat zS = zCoordWorldspace / renderScale.z;
	int i = (int)((xS + 0.5f) * (rows - 1));
	int j = (int)((zS + 0.5f) * (cols - 1));
	if (i < 0 || i > cols - 1 || j < 0 || j > rows - 1) return 0.0f;
	return terrainHeightValues[i][j] * renderScale.y;
}