#pragma once
#include "Module.h"

class Application
{
public:
    virtual void add(Module *mod) = 0;
    virtual void setup() = 0;
    virtual void startRendering() = 0;
    virtual void close() = 0;
};
