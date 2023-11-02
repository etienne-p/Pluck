//
//  Pool.cc
//  Harp2
//
//  Created by Etienne on 2015-11-26.
//
//

#pragma once

#include <array>
#include <vector>
#include <list>
#include "ofMain.h"

template <typename T>
class Pool
{
    
public:
    
    void addItem(T item)
    {
        objects.push_back(std::move(item));
        objectsList.push_back(&(objects.back()));
    }
    
    T* getItem()
    {
        if (objectsList.size() == 0) return nullptr;
        
        T* rv = objectsList.front();
        objectsList.pop_front();
        return rv;
    }
    
    void returnItem(T* arg)
    {
        auto iter = std::find (objectsList.begin(), objectsList.end(), arg);
        if (iter == objectsList.end())
        {
            objectsList.push_back(arg);
        }
        else
        {
            ofLogError("Pool: attempted to return an item that's already returned");
        }
    }
    
private:
    std::list<T*>     objectsList;
    std::vector<T>    objects;
    
};

template <typename T, size_t N>
class StaticPool
{
    
public:
    
    StaticPool()
    {
        for (auto& o : objects)
        {
            objectsList.push_back(&o);
        }
    }
    
    T* getItem()
    {
        if (objectsList.size() == 0) return nullptr;
        
        T* rv = objectsList.front();
        objectsList.pop_front();
        return rv;
    }
    
    void returnItem(T* arg)
    {
        auto iter = std::find (objectsList.begin(), objectsList.end(), arg);
        if (iter == objectsList.end())
        {
            objectsList.push_back(arg);
        }
        else
        {
            ofLogError("Pool: attempted to return an item that's already returned");
        }
    }
    
private:
    std::list<T*>       objectsList;
    std::array<T, N>    objects;

};