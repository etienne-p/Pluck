//
//  Transform2D.hpp
//  Harp2
//
//  Created by Etienne on 2015-11-19.
//
//

#pragma once

//#include <stack>
#include "ofMain.h"

class Transform2D
{
    
public:
    
    virtual ~Transform2D();
    
    bool addChild(Transform2D * transform);
    bool removeChild(Transform2D * transform);
    void setParent(Transform2D * transform);
    Transform2D * getParent() const;
    void setChildIndex(Transform2D * child, size_t index);
    
    ofMatrix4x4 getLocalTransformMatrix();
    ofMatrix4x4 getGlobalTransformMatrix();
    
    ofVec2f getPosition() const noexcept;
    //ofVec2f getAnchoredPosition() const noexcept;
    ofVec2f getScale() const noexcept;
    ofVec2f getSize() const noexcept;
    ofVec2f getPivot() const noexcept;
    
    size_t getNumChildren() const noexcept;
    size_t getDepth() const noexcept;
    
    void setPivot(const ofVec2f& p);
    //void setAnchoredPosition(const ofVec2f& v);
    void setPosition(const ofVec2f& v);
    void setScale(const ofVec2f& v);
    virtual void setSize(const ofVec2f& v); // ie TextBox, setSize has to trigger a fit
   
    ofVec2f globalToLocal(ofVec2f input);
    ofVec2f localToGlobal(ofVec2f input);
    
    //void traversePreOrder(const std::function<bool(Transform2D*, int depth)>& predicate);
    //void traversePostOrder(const std::function<bool(Transform2D*, int depth)>& predicate);
    void traverse(const std::function<bool(Transform2D*)>& predicate);
    void traverseBackward(const std::function<bool(Transform2D*)>& predicate);

    // void return as the traversal is not meant to be interrupted when pushing matrices
    //void traversePreOrderPushingMatrices(const std::function<void(Transform2D*, int depth)>& predicate);
    
    template <typename T>
    class Transform2DChildForwardIterator
    {
    public:
        Transform2DChildForwardIterator(Transform2D* transform_)
        {
            transform = transform_;
            it = transform->children.begin();
        }
        
        inline void operator++(){ ++it; }
        
        bool isDone() const { return it == transform->children.end(); };
        
        T* operator->(){ return dynamic_cast<T*>(*it); }
    private:
        vector<Transform2D*>::iterator it;
        Transform2D* transform;
    };
    /*
    template <typename T>
    class Transform2DChildBackwardIterator
    {
    public:
        Transform2DChildBackwardIterator(Transform2D* transform_)
        {current = transform_->lastChild;}
        void operator++(){current = current->prevSibling;}
        bool isDone() const {return current == nullptr;};
        T* operator->(){return dynamic_cast<T*>(current);}
    private:
        Transform2D* current{nullptr};
    };*/
    
    template <typename T>
    Transform2DChildForwardIterator<T>
    getForwardChildIterator()
    {return Transform2DChildForwardIterator<T>(static_cast<T*>(this));}
    
    /*
    template <typename T>
    Transform2DChildBackwardIterator<T>
    getBackwardChildIterator()
    {return Transform2DChildBackwardIterator<T>(static_cast<T*>(this));}
     */
    
protected:
    
    void setLocalDirty();
    void setGlobalDirty();
    
private:
    
    void updateDepth();
    bool localMatrixDirty{true};
    bool globalMatrixDirty{true};
    
    ofMatrix4x4 localTransformMatrix;
    ofMatrix4x4 globalTransformMatrix;
    
    static const float ANCHOR_RATIOS[9][2];
    
    ofVec2f position{.0f, .0f};
    ofVec2f scale{1.0f, 1.0f};
    ofVec2f pivot;
    ofVec2f size{.0f, .0f};
    size_t depth{0};
    
    Transform2D* parent{nullptr};
    vector<Transform2D*> children;
};