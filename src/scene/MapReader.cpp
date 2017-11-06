#include "MapReader.h"
#include "Game.h"
#include "Urho3D/Resource/Image.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <iostream>

MapReader::MapReader() {
}


MapReader::~MapReader() {
}

float MapReader::getValue(Image* image, int i, int j) {
	int height = image->GetHeight();
	int width = image->GetWidth();
	if(i<0) {
		i = 0;
	}else if(i>=width) {
		i = width - 1;
	}
	if (j<0) {
		j = 0;
	}
	else if (j >= height) {
		j = height - 1;
	}
	unsigned result= image->GetPixelInt(i, j) & 0x000000FF;
	return result / 50.f;
}

void MapReader::append(Image* image, float* vertexData, int& index, int i, int j) {
	vertexData[index++] = i;
	vertexData[index++] = getValue(image, i, j);
	vertexData[index++] = j;

	vertexData[index++] = 0.0f;
	vertexData[index++] = -1.0f;
	vertexData[index++] = 0.0f;
	cout << vertexData[index - 6] <<  " " << vertexData[index - 4] << endl;

}

Model* MapReader::read(Urho3D::String path) {
	Image* image = new Image(Game::get()->getContext());
	bool succes = image->LoadFile(path);

	int height = image->GetHeight();
	int width = image->GetWidth();

	const unsigned numVertices = height * width * 6;//TODO dwa poligony razy 6 danych
	float* vertexData = new float[numVertices * 6];
	int index = 0;
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			append(image, vertexData, index, i, j);
			append(image, vertexData, index, i, j-1);
			append(image, vertexData, index, i-1, j);

			append(image, vertexData, index, i, j);
			append(image, vertexData, index, i+1, j-1);
			append(image, vertexData, index, i, j-1);

		}
	}

	int* indexData = new int[numVertices];
	for (int i = 0; i < numVertices; ++i) {
		indexData[i] = i;
	}

	// Calculate face normals now
	for (unsigned i = 0; i < numVertices; i += 3) {
		Vector3& v1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i]));
		Vector3& v2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1)]));
		Vector3& v3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2)]));
		Vector3& n1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i + 3]));
		Vector3& n2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1) + 3]));
		Vector3& n3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2) + 3]));

		Vector3 edge1 = v1 - v2;
		Vector3 edge2 = v1 - v3;
		n1 = n2 = n3 = edge1.CrossProduct(edge2).Normalized();
		//cout << v1.ToString().CString() << " " << v2.ToString().CString() << " " << v3.ToString().CString() << "||"<<
		//	n1.ToString().CString() << endl;
	}

	Model* fromScratchModel(new Model(Game::get()->getContext()));
	SharedPtr<VertexBuffer> vertexBuffer(new VertexBuffer(Game::get()->getContext()));
	SharedPtr<IndexBuffer> indexBuffer(new IndexBuffer(Game::get()->getContext()));
	SharedPtr<Geometry> geom(new Geometry(Game::get()->getContext()));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	//vertexBuffer->SetShadowed(true);
	// We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
	// defining the vertex elements explicitly to allow any element types and order
	PODVector<VertexElement> elements;
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
	vertexBuffer->SetSize(numVertices, elements);
	vertexBuffer->SetData(vertexData);

	indexBuffer->SetShadowed(true);
	indexBuffer->SetSize(numVertices, true);
	indexBuffer->SetData(indexData);

	geom->SetVertexBuffer(0, vertexBuffer);
	geom->SetIndexBuffer(indexBuffer);
	geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);

	fromScratchModel->SetNumGeometries(1);
	fromScratchModel->SetGeometry(0, 0, geom);
	fromScratchModel->SetBoundingBox(BoundingBox(Vector3(-600.0f, -50.0f, -600.0f), Vector3(500.0f, 0.5f, 600.f)));

	//	// Though not necessary to render, the vertex & index buffers must be listed in the model so that it can be saved properly
	//	Vector<SharedPtr<VertexBuffer> > vertexBuffers;
	//	Vector<SharedPtr<IndexBuffer> > indexBuffers;
	//	vertexBuffers.Push(vertexBuffer);
	//	indexBuffers.Push(indexBuffer);
	//	// Morph ranges could also be not defined. Here we simply define a zero range (no morphing) for the vertex buffer
	//	PODVector<unsigned> morphRangeStarts;
	//	PODVector<unsigned> morphRangeCounts;
	//	morphRangeStarts.Push(0);
	//	morphRangeCounts.Push(0);
	//	fromScratchModel->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
	//	fromScratchModel->SetIndexBuffers(indexBuffers);
	delete[] vertexData;
	delete[] indexData;
	return fromScratchModel;
}
