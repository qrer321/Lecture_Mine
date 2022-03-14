#pragma once
#include <memory>
#include <vector>

// �뵵 : 
// �з� :
// ÷�� : 
class GameServerObjectBase : public std::enable_shared_from_this<GameServerObjectBase>
{
private: // Member Var

public: // Default
	GameServerObjectBase();
	virtual ~GameServerObjectBase();

	GameServerObjectBase(const GameServerObjectBase& other) = delete;
	GameServerObjectBase(GameServerObjectBase&& other) noexcept;

public:
	GameServerObjectBase& operator=(const GameServerObjectBase& other) = delete;
	GameServerObjectBase& operator=(GameServerObjectBase&& other) = delete;

private:
	GameServerObjectBase*								m_Parent;
	std::vector<std::shared_ptr<GameServerObjectBase>>	m_LinkObject;

public:
	void SetParent(GameServerObjectBase* parent) { m_Parent = parent; }
	void SetLink(std::shared_ptr<GameServerObjectBase> link) { m_LinkObject.push_back(std::move(link)); }

public:
	template<typename ParentType>
	ParentType* GetParent() { return dynamic_cast<ParentType*>(m_Parent); }

	template<typename LinkType>
	LinkType* GetLink(int index = 0) { return dynamic_cast<LinkType>(m_LinkObject[index]); }

public: // Member Function
	bool IsValidLowLevel();
};

