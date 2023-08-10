#include "HeightMap.h"

HeightMap::HeightMap(const std::string& name)
{
	int iWidth;
	int iHeight;
	int iChans;
	unsigned char* data = stbi_load(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data)
	{
		std::cout << "Heightmap can't be loaded" << std::endl;
		return;
	}
	numVertices   = iWidth * iHeight;
	numIndices	  = (iWidth - 1) * (iHeight - 1) * 6;
	vertices	  = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices		  = new unsigned int[numIndices];

	Vector3 vertexScale  = Vector3(32.0f, 1.0f, 32.0f);
	Vector2 textureScale = Vector2(1.0f / 32.0f, 1.0f / 32.0f);

	for (int z = 0; z < iHeight; z++)
	{
		for (int x = 0; x < iWidth; x++)
		{
			int offset			  = (z * iWidth) + x;
			vertices[offset]	  = Vector3((float)x, data[offset], (float)z) * vertexScale;
			textureCoords[offset] = Vector2((float)x, (float)z) * textureScale;
		}
	}

	stbi_image_free(data);

	int i = 0;

	for (int z = 0; z < iHeight - 1; z++)
	{
		for (int x = 0; x < iWidth - 1; x++)
		{
			int a = (z * iWidth) + x;
			int b = (z * iWidth) + (x + 1);
			int c = ((z + 1) * iWidth) + (x + 1);
			int d = ((z + 1) * iWidth) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}

	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);

}

HeightMap::~HeightMap()
{
}