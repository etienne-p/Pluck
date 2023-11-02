//
//  Transform2D.cpp
//  Harp2
//
//  Created by Etienne on 2015-11-19.
//
//

#include "Transform2D.hpp"

ofVec2f Transform2D::getPosition() const noexcept { return position; }
ofVec2f Transform2D::getScale() const noexcept { return scale; };
ofVec2f Transform2D::getSize() const noexcept { return size; };
ofVec2f Transform2D::getPivot() const noexcept { return pivot; };
size_t Transform2D::getNumChildren() const noexcept  { return children.size(); };

size_t Transform2D::getDepth() const noexcept { return depth; };


void Transform2D::setPivot(const ofVec2f& p) { pivot = p; setLocalDirty(); }

void Transform2D::setPosition(const ofVec2f& v) { position = v; setLocalDirty(); }

void Transform2D::setScale(const ofVec2f& v) { scale = v; setLocalDirty(); }

void Transform2D::setSize(const ofVec2f& v) { size = v; setLocalDirty(); };

void Transform2D::updateDepth()
{
    depth = 0;
    auto p = getParent();
    while(p)
    {
        ++depth;
        p = p->getParent();
    }
}

bool Transform2D::addChild(Transform2D* child)
{
    assert(child);
    if (child->getParent() == this)
    {
        return false;
    }
    else if (child->getParent() != nullptr)
    {
        child->getParent()->removeChild(child);
    }
    child->parent = this;
    children.push_back(child);
    // update depth:
    child->updateDepth();
    return true;
}

bool Transform2D::removeChild(Transform2D* child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        (*it)->parent = nullptr;
        (*it)->updateDepth();
        children.erase(it);
        return true;
    }
    return false;
}

void Transform2D::setParent(Transform2D * transform)
{
    if (parent != nullptr) parent->removeChild(this);
    if (transform != nullptr) transform->addChild(this);
}

Transform2D * Transform2D::getParent() const
{
    return parent;
}

void Transform2D::setChildIndex(Transform2D * child, size_t index)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        children.erase(it);
        children.insert(children.begin() + std::min<size_t>(index, children.size()), child);
    }
}

ofMatrix4x4 Transform2D::getLocalTransformMatrix()
{
    if (localMatrixDirty)
    {
        const ofVec3f pivotPos
        {
            pivot.x * size.x,
            pivot.y * size.y,
            0
        };
        
        const ofVec3f pos3d
        {
            position.x,
            position.y,
            0
        };
        
        localTransformMatrix.makeIdentityMatrix();
        localTransformMatrix.postMultTranslate(-pivotPos);
        localTransformMatrix.postMultScale(ofVec3f(scale.x, scale.y, 1.0f));
        localTransformMatrix.postMultTranslate(pos3d + pivotPos);
        localMatrixDirty = false;
    }
    return localTransformMatrix;
}

ofMatrix4x4 Transform2D::getGlobalTransformMatrix()
{
    if (globalMatrixDirty)
    {
        if (parent == nullptr) globalTransformMatrix = getLocalTransformMatrix();
        else globalTransformMatrix = getLocalTransformMatrix() * parent->getGlobalTransformMatrix(); // inverse glm order;
        globalMatrixDirty = false;
    }
    return globalTransformMatrix;
}

ofVec2f Transform2D::globalToLocal(ofVec2f input)
{
    return ofVec2f(ofVec3f(input) * getGlobalTransformMatrix().getInverse());
}

ofVec2f Transform2D::localToGlobal(ofVec2f input)
{
    return ofVec2f(ofVec3f(input) * getGlobalTransformMatrix());
}

void Transform2D::traverse(const std::function<bool(Transform2D*)>& predicate)
{
    if (!predicate(this)) return;
    //for (auto& c : children) c->traverse(predicate); return;
    for (int i = 0; i < children.size(); ++i)
    {
        children[i]->traverse(predicate);
    }
}

void Transform2D::traverseBackward(const std::function<bool(Transform2D*)>& predicate)
{
    if (!predicate(this)) return;
    //for (auto& c : children) c->traverse(predicate);
    /*for(auto rit = children.rbegin(); rit != children.rend(); ++rit)
    {
        (*rit)->traverseBackward(predicate);
    }
    return;*/
    for (int i = children.size() - 1; i > -1; i = std::min<int>(i - 1, children.size()))
    {
        children[i]->traverseBackward(predicate);
    }
}

Transform2D::~Transform2D()
{
    if (parent != nullptr) parent->removeChild(this);
    for (auto& c : children) c->setParent(nullptr);
}

void Transform2D::setLocalDirty()
{
    localMatrixDirty = true;
    setGlobalDirty();
}

void Transform2D::setGlobalDirty()
{
    globalMatrixDirty = true;
    for (auto c : children) c->setGlobalDirty();
}


///