#include "MapReader.h"
#include "Game.h"
#include "Urho3D/Resource/Image.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/IndexBuffer.h>

MapReader::MapReader() {
}


MapReader::~MapReader() {
}

Model* MapReader::read(Urho3D::String path) {
	Image* image = new Image(Game::get()->getContext());
	bool succes = image->LoadFile(path);

	int height = image->GetHeight();
	int width = image->GetWidth();
	int chanels = image->GetComponents();
	
//	for (int i = 0; i < height; ++i) {
//		for (int j = 0; j < width; ++j) {
//			unsigned value =image->GetPixelInt(i, j) & 0x000000FF;
//
//
//		}
//	}

	const unsigned numVertices = 18;

	float vertexData[] = {
		// Position             Normal
		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f,

		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,

		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 0.0f
	};

	const unsigned short indexData[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17
	};

	// Calculate face normals now
	for (unsigned i = 0; i < numVertices; i += 3)
	{
		Vector3& v1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i]));
		Vector3& v2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1)]));
		Vector3& v3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2)]));
		Vector3& n1 = *(reinterpret_cast<Vector3*>(&vertexData[6 * i + 3]));
		Vector3& n2 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1) + 3]));
		Vector3& n3 = *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2) + 3]));

		Vector3 edge1 = v1 - v2;
		Vector3 edge2 = v1 - v3;
		n1 = n2 = n3 = edge1.CrossProduct(edge2).Normalized();
	}

	Model* fromScratchModel(new Model(Game::get()->getContext()));
	SharedPtr<VertexBuffer> vertexBuffer(new VertexBuffer(Game::get()->getContext()));
	SharedPtr<IndexBuffer> indexBuffer(new IndexBuffer(Game::get()->getContext()));
	SharedPtr<Geometry> geom(new Geometry(Game::get()->getContext()));

	// Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
	vertexBuffer->SetShadowed(true);
	// We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
	// defining the vertex elements explicitly to allow any element types and order
	PODVector<VertexElement> elements;
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
	elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
	vertexBuffer->SetSize(numVertices, elements);
	vertexBuffer->SetData(vertexData);

	indexBuffer->SetShadowed(true);
	indexBuffer->SetSize(numVertices, false);
	indexBuffer->SetData(indexData);

	geom->SetVertexBuffer(0, vertexBuffer);
	geom->SetIndexBuffer(indexBuffer);
	geom->SetDrawRange(TRIANGLE_LIST, 0, numVertices);

	fromScratchModel->SetNumGeometries(1);
	fromScratchModel->SetGeometry(0, 0, geom);
	fromScratchModel->SetBoundingBox(BoundingBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f)));

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

	Node* node = Game::get()->getScene()->CreateChild("FromScratchObject");
	node->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
	StaticModel* object = node->CreateComponent<StaticModel>();
	object->SetModel(fromScratchModel);
	return fromScratchModel;
}
