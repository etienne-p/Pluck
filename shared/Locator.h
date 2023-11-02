#pragma once

#include <typeinfo>
#include <map>
#include <memory>
#include <assert.h>

class Locator
{
public:
    
    template <typename T, typename... TArgs>
    static std::shared_ptr<T> create(TArgs&&... mArgs)
    {
        assert (!entries.count( typeid(T).hash_code() )); // one instance per type
        auto uPtr = std::make_shared<T>(std::forward<TArgs>(mArgs)...);
        entries[typeid(T).hash_code()] = uPtr;
        return uPtr;
    }
    
    template <typename T>
    static std::shared_ptr<T> get()
    {
        return entries.count( typeid(T).hash_code() ) ?
        std::static_pointer_cast<T>(entries[typeid(T).hash_code()]) :
        nullptr;
    }
    
private:
    
    static std::map<std::size_t, std::shared_ptr<void>> entries;
};