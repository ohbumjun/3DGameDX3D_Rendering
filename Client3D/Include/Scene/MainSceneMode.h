#pragma once

#include "Scene/SceneMode.h"

class CMainSceneMode :
    public CSceneMode
{
public:
    CMainSceneMode();
    ~CMainSceneMode();

private:
    //CSharedPtr<CMainWidget> m_MainWidget;
    std::function<void(bool, float)> m_LoadingFunction;

public:
    virtual bool Init();

private:
    void LoadMesh();
    void CreateMaterial();
    void CreateAnimationSequence();
    void CreateParticle();

public:
    template <typename T>
    void SetLoadingFunction(T* Obj, void(T::* Func)(bool, float))
    {
        m_LoadingFunction = std::bind(Func, Obj, std::placeholders::_1, std::placeholders::_2);
    }
};

