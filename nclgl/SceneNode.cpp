#include "SceneNode.h"

SceneNode::SceneNode(Mesh* m, Vector4 colour)
{
	this->mesh		   = m;
	this->colour	   = colour;
	parent			   = NULL;
	modelScale		   = Vector3(1, 1, 1);
	//Scene Management
	boundingRadius     = 1.0f;
	distanceFromCamera = 0.0f;
	texture			   = 0;
}

SceneNode::~SceneNode()
{
	for (unsigned int i = 0; i < children.size(); i++) { delete children[i]; }
}

void SceneNode::AddChild(SceneNode* s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw(const OGLRenderer& r)
{
	if (mesh) { mesh->Draw(); }
}

void SceneNode::Update(float dt)
{
	if (parent) { worldTransform = parent->worldTransform * transform; }
	else		{ worldTransform = transform; }

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) { (*i)->Update(dt); }
}