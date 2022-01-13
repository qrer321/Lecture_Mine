#pragma once
#include <memory>
#include <vector>

// 용도 : 
// 분류 :
// 첨언 : 
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
	GameServerObjectBase*				m_Parent;
	std::vector<GameServerObjectBase*>	m_LinkObject;

public:
	void SetParent(GameServerObjectBase* parent) { m_Parent = parent; }
	void SetLink(GameServerObjectBase* link) { m_LinkObject.push_back(link); }

public:
	template<typename ParentType>
	ParentType* GetParent() { return dynamic_cast<ParentType>(m_Parent); }

	template<typename LinkType>
	LinkType* GetLink(int index = 0) { return dynamic_cast<LinkType>(m_LinkObject[index]); }

public: // Member Function
	bool IsLowLevelValid();
};

