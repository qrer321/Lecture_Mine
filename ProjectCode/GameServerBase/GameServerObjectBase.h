#pragma once
#include <memory>
#include <vector>
#include "GameServerDebug.h"

class GameServerObjectBase : public std::enable_shared_from_this<GameServerObjectBase>
{
private: // Member Var
	GameServerObjectBase*								m_Parent;
	std::vector<std::shared_ptr<GameServerObjectBase>>	m_LinkObject{};

	std::atomic<bool>									m_IsUpdate;
	std::atomic<bool>									m_IsDeath;
	float												m_AccTime;
	int													m_Frame;

public: // Default
	GameServerObjectBase();
	virtual ~GameServerObjectBase();

	GameServerObjectBase(const GameServerObjectBase& other) = delete;
	GameServerObjectBase(GameServerObjectBase&& other) noexcept;
	GameServerObjectBase& operator=(const GameServerObjectBase& other) = delete;
	GameServerObjectBase& operator=(GameServerObjectBase&& other) = delete;

public:
	template <typename CastType>
	std::shared_ptr<CastType> DynamicCast()
	{
		return std::dynamic_pointer_cast<CastType>(shared_from_this());
	}

	void SetParent(GameServerObjectBase* parent) { m_Parent = parent; }

	template <typename CreateType, typename EnumData>
	std::shared_ptr<CreateType> CreateLink(EnumData index)
	{
		std::shared_ptr<CreateType> new_object = std::make_shared<CreateType>();
		SetLink<EnumData>(index, new_object);

		return new_object;
	}

	template <typename CreateType>
	std::shared_ptr<CreateType> CreateLink(size_t index)
	{
		std::shared_ptr<CreateType> new_object = std::make_shared<CreateType>();
		SetLink(index, new_object);

		return new_object;
	}

	template <typename EnumData>
	void SetLink(EnumData index, std::shared_ptr<GameServerObjectBase> object)
	{
		SetLink(static_cast<size_t>(index), std::move(object));
	}

	void SetLink(size_t index, std::shared_ptr<GameServerObjectBase> object)
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

		m_LinkObject[index] = std::move(object);
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
	[[nodiscard]] bool IsDeath() const { return m_IsDeath; }
	[[nodiscard]] bool IsUpdate() const { return m_IsUpdate && false == m_IsDeath; }

	void AccTimeReset() { m_AccTime = 0.f; }
	void AccTimeUpdate(float delta_time) { m_AccTime += delta_time; ++m_Frame; }
	float GetAccTime() const { return m_AccTime; }
	bool IsFrame(int frame) const { return (m_Frame % frame) == 0; }

	virtual void SetDeath(bool is_death = true) { m_IsDeath = is_death; }
};

