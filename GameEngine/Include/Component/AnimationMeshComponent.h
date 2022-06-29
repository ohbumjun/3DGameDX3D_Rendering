#pragma once

#include "SceneComponent.h"
#include "../Resource/Mesh/AnimationMesh.h"
#include "../Resource/Material/Material.h"
#include "../Resource/Animation/Skeleton.h"
#include "../Animation/AnimationSequenceInstance.h"

class CAnimationMeshComponent :
    public CSceneComponent
{
    friend class CGameObject;

protected:
    CAnimationMeshComponent();
    CAnimationMeshComponent(const CAnimationMeshComponent& com);
    virtual ~CAnimationMeshComponent();

protected:
    CSharedPtr<CAnimationMesh> m_Mesh;
    std::vector<CSharedPtr<CMaterial>> m_vecMaterialSlot;
    CSharedPtr<CSkeleton>   m_Skeleton;
    class CAnimationSequenceInstance* m_Animation;
    int         m_InstanceID;

public:
    void SetInstanceID(int ID)
    {
        m_InstanceID = ID;
    }

    int GetInstanceID() const
    {
        return m_InstanceID;
    }

    CMaterial* GetMaterial(int Index = 0)    const
    {
        return m_vecMaterialSlot[Index];
    }

    int GetMaterialSlotCount()  const
    {
        return (int)m_vecMaterialSlot.size();
    }

    class CAnimationSequenceInstance* GetAnimationInstance()  const
    {
        return m_Animation;
    }

public:
    void SetMesh(const std::string& Name);
    void SetMesh(CAnimationMesh* Mesh);
    void SetMaterial(CMaterial* Material, int Index = 0);
    void AddMaterial(CMaterial* Material);

public:
    void SetBaseColor(const Vector4& Color, int Index = 0);
    void SetBaseColor(float r, float g, float b, float a, int Index = 0);
    void SetAmbientColor(const Vector4& Color, int Index = 0);
    void SetAmbientColor(float r, float g, float b, float a, int Index = 0);
    void SetSpecularColor(const Vector4& Color, int Index = 0);
    void SetSpecularColor(float r, float g, float b, float a, int Index = 0);
    void SetEmissiveColor(const Vector4& Color, int Index = 0);
    void SetEmissiveColor(float r, float g, float b, float a, int Index = 0);
    void SetSpecularPower(float Power, int Index = 0);
    void SetRenderState(class CRenderState* State, int Index = 0);
    void SetRenderState(const std::string& Name, int Index = 0);
    void SetTransparency(bool Enable, int Index = 0);
    void SetOpacity(float Opacity, int Index = 0);
    void AddOpacity(float Opacity, int Index = 0);

public:
    void AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void AddTextureFullPath(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void AddTexture(int MaterialIndex, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);


    void SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void SetTextureFullPath(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void SetTexture(int MaterialIndex, int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);

public:
    virtual void AddChild(CSceneComponent* Child, const std::string& SocketName = "");
    virtual void AddChild(class CGameObject* Child, const std::string& SocketName = "");
    virtual void Start();
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual void PrevRender();
    virtual void Render();
    virtual void RenderShadowMap();
    virtual void PostRender();
    virtual CAnimationMeshComponent* Clone();
    virtual void Save(FILE* File);
    virtual void Load(FILE* File);

public:
    template <typename T>
    void CreateAnimationInstance()
    {
        T* Anim = new T;

        Anim->SetScene(m_Scene);
        Anim->SetOwner(this);

        if (!Anim->Init())
        {
            SAFE_DELETE(Anim);
            return;
        }

        SAFE_DELETE(m_Animation);

        m_Animation = Anim;

        if (m_Skeleton)
            m_Animation->SetSkeleton(m_Skeleton);

        if (m_Mesh)
            m_Animation->SetInstancingBoneBuffer(m_Mesh->GetBoneBuffer());
    }


    template <typename T>
    void LoadAnimationInstance()
    {
        T* Anim = new T;

        Anim->SetScene(m_Scene);
        Anim->SetOwner(this);

        SAFE_DELETE(m_Animation);

        m_Animation = Anim;

        if (m_Skeleton)
            m_Animation->SetSkeleton(m_Skeleton);
    }
};

