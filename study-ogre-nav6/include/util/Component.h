#pragma once
#include <vector>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <functional>

using CreatorFunc = std::function<std::any()>;

class Component
{
public:
    using CreatorFunc = std::function<Component *()>;

protected:
    struct Wrapper
    {
        std::type_index type;
        std::any any;

        Wrapper(std::type_index type, std::any any_) : type(type), any(any_)
        {
        }
        ~Wrapper()
        {
        }
    };

    std::unordered_map<std::type_index, std::vector<Wrapper *>> children;
    Component *parent = nullptr;
    std::vector<Component *> comps;

    //
    std::unordered_map<std::type_index, std::function<void()>> creators;

public:
    Component()
    {
    }

    ~Component()
    {
        deleteChildren();
    }
    void deleteChildren()
    {
        for (auto it = comps.rbegin(); it != comps.rend(); ++it)
        {
            delete (*it);
        }
        comps.clear();
        children.clear();
    }

    void doAdd(std::type_index &type, std::any any, Component *comp)
    {
        Wrapper *wrapper = new Wrapper(type, any);
        children[type].push_back(wrapper);
        if (comp)
        {
            comps.push_back(comp);
        }
    }

    template <typename T>
    void registerCreator(CreatorFunc func)
    {
        std::type_index type = typeid(T);
        creators->emplace(type, [func, this]() -> void
                          { this->addComponent<T>(func()); });
    }

    template <typename T>
    void addComponent(T *comp)
    {
        std::type_index type = typeid(T);

        doAdd(type, comp, comp);
        Component *c = (Component *)comp;
        c->parent = this;
    }

    template <typename T>
    void addObject(std::any obj)
    {
        std::type_index type = typeid(T);
        doAdd(type, obj, nullptr);
    }

    template <typename T>
    T *find(bool resolve = false)
    {
        std::type_index type = typeid(T);
        std::vector<Wrapper *> &vec = children[type];
        return doFind<T>(type, vec, resolve);
    }

    template <typename T>
    T *doFind(std::type_index &type, std::vector<Wrapper *> &vec, bool resolve)
    {
        if (!vec.empty())
        {
            return std::any_cast<T *>(vec[0]->any);
        }

        if (!resolve)
        {
            return nullptr;
        }

        auto it = this->creators.find(type);
        if (it == this->creators.end())
        {
            return nullptr;
        }

        std::function<void()> func = it->second;
        func();
        return doFind<T>(type, vec, false);
    }

    virtual void init()
    {
        this->initChildrens();
    }

    void initChildrens()
    {
        for (auto it = comps.begin(); it != comps.end(); ++it)
        {
            Component *comp = (*it);
            if (comp)
            {
                comp->init();
            }
        }
    }
};