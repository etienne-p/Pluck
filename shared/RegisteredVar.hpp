//
//  RegisteredVar.hpp
//  Harp2
//
//  Created by etienne cella on 2016-01-07.
//
//

#pragma once

template<typename T>
class RegisteredVar
{
public:
    
    
    bool isDirty() const noexcept { return true; }
    
private:
    
    bool dirty{true};

};


struct ShaderParameters
{
    RegisteredVar<float> p0;
    RegisteredVar<int> p1;
    
    bool isDirty() const { p0.isDirty() || p1.isDirty(); };
};