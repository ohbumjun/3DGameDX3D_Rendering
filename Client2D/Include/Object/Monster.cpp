
#include "Monster.h"
#include "Scene/Scene.h"
#include "Resource/Material/Material.h"
#include "MonsterAnimation.h"
#include "../Widget/SimpleHUD.h"
#include "Engine.h"

CMonster::CMonster()
{
	SetTypeID<CMonster>();
}

CMonster::CMonster(const CMonster& obj) :
	CGameObject(obj)
{
	m_Sprite = (CSpriteComponent*)FindComponent("PlayerSprite");
	m_Body = (CColliderCircle*)FindComponent("Body");
	m_SimpleHUDWidget = (CWidgetComponent*)FindComponent("SimpleHUD");
	m_PaperBurn = (CPaperBurnComponent*)FindComponent("PaperBurn");
}

CMonster::~CMonster()
{
}

void CMonster::Start()
{
	CGameObject::Start();

	m_PaperBurn->SetFinishCallback<CMonster>(this, &CMonster::PaperBurnEnd);
}

bool CMonster::Init()
{
	m_Sprite = CreateComponent<CSpriteComponent>("PlayerSprite");
	m_Body = CreateComponent<CColliderCircle>("Body");
	m_PaperBurn = CreateComponent<CPaperBurnComponent>("PaperBurn");

	m_SimpleHUDWidget = CreateComponent<CWidgetComponent>("SimpleHUD");

	m_SimpleHUD = m_SimpleHUDWidget->CreateWidgetWindow<CSimpleHUD>("SimpleHUDWidget");

	SetRootComponent(m_Sprite);

	m_Sprite->AddChild(m_Body);
	m_Sprite->AddChild(m_SimpleHUDWidget);

	m_PaperBurn->SetMaterial(m_Sprite->GetMaterial());

	m_SimpleHUDWidget->SetRelativePos(-50.f, 50.f, 0.f);

	m_SimpleHUD->SetText(TEXT("띠모아님"));
	m_SimpleHUD->SetHPPercent(0.5f);
	m_SimpleHUD->SetHPDir(ProgressBar_Dir::BottomToTop);

	m_Body->SetCollisionProfile("Monster");

	m_Sprite->SetTransparency(true);
	//m_Sprite->SetOpacity(0.5f);

	m_Sprite->CreateAnimationInstance<CMonsterAnimation>();

	m_Sprite->SetRelativeScale(100.f, 100.f, 1.f);
	m_Sprite->SetRelativePos(500.f, 300.f, 0.f);
	m_Sprite->SetPivot(0.5f, 0.5f, 0.f);

	m_Body->AddCollisionMouseCallback(Collision_State::Begin, this, &CMonster::OnMouseBegin);
	m_Body->AddCollisionMouseCallback(Collision_State::End, this, &CMonster::OnMouseEnd);
	
	m_Body->AddCollisionCallback(Collision_State::Begin, this, &CMonster::OnCollisionBegin);
	m_Body->AddCollisionCallback(Collision_State::End, this, &CMonster::OnCollisionEnd);

	m_HP = 3;

	return true;
}

void CMonster::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CMonster::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CMonster* CMonster::Clone()
{
	return new CMonster(*this);
}

void CMonster::OnMouseBegin(const CollisionResult& result)
{
	CEngine::GetInst()->SetMouseState(Mouse_State::State1);
}

void CMonster::OnMouseEnd(const CollisionResult& result)
{
	CEngine::GetInst()->SetMouseState(Mouse_State::Normal);
}

void CMonster::OnCollisionBegin(const CollisionResult& result)
{
	--m_HP;

	if (m_HP <= 0)
	{
		// 죽는 모션을 실행하고 죽는 모션이 끝났을때 끝 프레임에 고정시켜놓은 후에
		// PaperBurn을 진행한다.
		m_PaperBurn->StartPaperBurn();
		m_Body->Enable(false);
	}
}

void CMonster::OnCollisionEnd(const CollisionResult& result)
{
}

void CMonster::PaperBurnEnd()
{
	Destroy();
}
