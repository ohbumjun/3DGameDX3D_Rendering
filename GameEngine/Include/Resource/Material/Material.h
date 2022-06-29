#pragma once

#include "../Shader/GraphicShader.h"
#include "../Texture/Texture.h"
#include "../Shader/MaterialConstantBuffer.h"

struct MaterialTextureInfo
{
    std::string     Name;
    Sampler_Type    SamplerType;
    CSharedPtr<CTexture>    Texture;
    int             Register;
    int             ShaderType;

    MaterialTextureInfo()   :
        SamplerType(Sampler_Type::Linear),
        Register(0),
        ShaderType((int)Buffer_Shader_Type::Pixel)
    {
    }
};

struct RenderCallback
{
    std::function<void()>   Func;
    void* Obj;
};


class CMaterial :
    public CRef
{
    friend class CMaterialManager;
    friend class CMesh;
    friend class CStaticMeshComponent;
    friend class CAnimationMeshComponent;

protected:
    CMaterial();
    CMaterial(const CMaterial& Material);
    virtual ~CMaterial();

private:
    class CScene* m_Scene;

public:
    void SetScene(class CScene* Scene)
    {
        m_Scene = Scene;
    }

protected:
    CSharedPtr<CGraphicShader>  m_Shader;
    std::vector<MaterialTextureInfo>    m_TextureInfo;
    Vector4     m_BaseColor;
    Vector4		m_AmbientColor;
    Vector4		m_SpecularColor;
    Vector4     m_EmissiveColor;
    float       m_Opacity;
    bool        m_Animation3D;
    bool        m_SpecularTex;
    bool        m_EmissiveTex;
    bool        m_Bump;
    bool        m_ReceiveDecal;
    CMaterialConstantBuffer* m_CBuffer;
    CSharedPtr<class CRenderState>  m_RenderStateArray[(int)RenderState_Type::Max];
    std::list<RenderCallback*>    m_RenderCallback;

public:
    CMaterialConstantBuffer* GetCBuffer()   const
    {
        return m_CBuffer;
    }

private:
    void SetConstantBuffer(class CMaterialConstantBuffer* Buffer)
    {
        m_CBuffer = Buffer->Clone();
    }

    void CreateConstantBuffer();

public:
    CTexture* GetTexture(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture;
    }

    unsigned int GetTextureWidth(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture->GetWidth();
    }

    unsigned int GetTextureHeight(int TextureIndex = 0)  const
    {
        return m_TextureInfo[TextureIndex].Texture->GetHeight();
    }

    bool EmptyTexture() const
    {
        return m_TextureInfo.empty();
    }

    void EnableBump();
    void EnableAnimation3D();
    void EnableSpecularTex();
    void EnableEmissiveTex();
    void EnableDecal(bool Decal);

public:
    void SetRenderState(class CRenderState* State);
    void SetRenderState(const std::string& Name);
    void SetTransparency(bool Enable);
    void SetOpacity(float Opacity);
    void AddOpacity(float Opacity);

public:
    void SetBaseColor(const Vector4& Color);
    void SetBaseColor(float r, float g, float b, float a);
    void SetAmbientColor(const Vector4& Color);
    void SetAmbientColor(float r, float g, float b, float a);
    void SetSpecularColor(const Vector4& Color);
    void SetSpecularColor(float r, float g, float b, float a);
    void SetEmissiveColor(const Vector4& Color);
    void SetEmissiveColor(float r, float g, float b, float a);
    void SetSpecularPower(float Power);

public:
    void AddTexture(int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void AddTexture(int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void AddTextureFullPath(int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void AddTexture(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);
    void AddTextureArrayFullPath(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
    void AddTextureArray(int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);


    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, class CTexture* Texture);
    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FileName, const std::string& PathName = TEXTURE_PATH);
    void SetTextureFullPath(int Index, int Register, int ShaderType, const std::string& Name, const TCHAR* FullPath);
    void SetTexture(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);
    void SetTextureArrayFullPath(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFullPath);
    void SetTextureArray(int Index, int Register, int ShaderType, const std::string& Name, const std::vector<TCHAR*>& vecFileName, const std::string& PathName = TEXTURE_PATH);

public:
    void SetPaperBurn(bool Enable);
    bool CheckMaterial(CMaterial* Material);

public:
    void SetShader(const std::string& Name);
    void Render();
    void RenderTexture();
    void Reset();
    CMaterial* Clone()  const;
    void Save(FILE* File);
    void Load(FILE* File);

public:
    template <typename T>
    void AddRenderCallback(T* Obj, void(T::* Func)())
    {
        RenderCallback* Callback = new RenderCallback;
        Callback->Obj = Obj;
        Callback->Func = std::bind(Func, Obj);
        m_RenderCallback.push_back(Callback);
    }

    void DeleteRenderCallback(void* Obj)
    {
        auto    iter = m_RenderCallback.begin();
        auto    iterEnd = m_RenderCallback.end();

        for (; iter != iterEnd; ++iter)
        {
            if ((*iter)->Obj == Obj)
            {
                SAFE_DELETE((*iter));
                m_RenderCallback.erase(iter);
                break;
            }
        }
    }
};

