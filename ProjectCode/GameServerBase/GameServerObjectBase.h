#pragma once
#include <memory>
#include <vector>
#include "GameServerDebug.h"

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

	template <typename EnumData>
	void SetLink(EnumData index, std::shared_ptr<GameServerObjectBase> link) { SetLink(static_cast<size_t>(index), std::move(link)); }
	void SetLink(size_t index, std::shared_ptr<GameServerObjectBase> link)
	{
		if (index >= 128)
		{
			GameServerDebug::AssertDebugMsg("Index Can Not Exceed 128 Maximum");
			return;
		}

		if (index >= m_LinkObject.size())
		{
			m_LinkObject.resize(index + 1);
		}

		if (nullptr != m_LinkObject[index])
		{
			GameServerDebug::AssertDebugMsg("Link Data Already Exists");
			return;
		}

		m_LinkObject[index] = std::move(link);
	}

	void ClearLinkObject() { m_LinkObject.clear(); }

	template<typename ParentType>
	ParentType* GetParent() { return dynamic_cast<ParentType*>(m_Parent); }

	template <typename LinkType, typename EnumType>
	std::shared_ptr<LinkType> GetLink(EnumType index)
	{
		return GetLink<LinkType>(static_cast<size_t>(index));
	}

	template<typename LinkType>
	std::shared_ptr<LinkType> GetLink(const size_t index)
	{
		if (index >= m_LinkObject.size())
		{
			return nullptr;
		}

		return std::dynamic_pointer_cast<LinkType>(m_LinkObject[index]);
	}

	template<typename LinkType>
	LinkType* GetLink(const int index = 0)
	{
		return dynamic_cast<LinkType*>(m_LinkObject[index]);
	}

public: // Member Function
	bool IsValidLowLevel();
};

