#pragma once
#include <deque>
#include <set>

// 용도 : 
// 분류 :
// 첨언 :

/////////////////////////// GameServerObjectPool ///////////////////////////
template <typename DataType>
class GameServerObjectPool
{
private: // Member Var
	std::deque<DataType*>	m_QueuePool;
	std::mutex				m_QueueLock;

public: // Default
	GameServerObjectPool() = default;
	~GameServerObjectPool();

	GameServerObjectPool(const GameServerObjectPool& other) = delete;
	GameServerObjectPool(GameServerObjectPool&& other) noexcept = default;

public:
	GameServerObjectPool& operator=(const GameServerObjectPool& other) = delete;
	GameServerObjectPool& operator=(GameServerObjectPool&& other) = delete;

public: // Member Function
	DataType*	Pop();
	void		Push(DataType* data);
	bool		IsEmpty();
};

template <typename DataType>
GameServerObjectPool<DataType>::~GameServerObjectPool()
{
	for (const DataType* data : m_QueuePool)
	{
		delete data;
	}
}

template <typename DataType>
DataType* GameServerObjectPool<DataType>::Pop()
{
	std::lock_guard lock(m_QueueLock);
	if (true == m_QueuePool.empty())
	{
		return new DataType;
	}

	DataType* newData = m_QueuePool.front();
	m_QueuePool.pop_front();
	return newData;
}

template <typename DataType>
void GameServerObjectPool<DataType>::Push(DataType* data)
{
	std::lock_guard lock(m_QueueLock);
	m_QueuePool.push_front(data);
}

template <typename DataType>
bool GameServerObjectPool<DataType>::IsEmpty()
{
	std::lock_guard lock(m_QueueLock);
	return m_QueuePool.empty();
}



/////////////////////////// GameServerObjectFindPool ///////////////////////////
template <typename DataType>
class GameServerObjectFindPool
{
private: // Member Var
	std::set<DataType*>		m_QueuePool;
	std::mutex				m_QueueLock;

public: // Default
	GameServerObjectFindPool() = default;
	~GameServerObjectFindPool();

	GameServerObjectFindPool(const GameServerObjectFindPool& other) = delete;
	GameServerObjectFindPool(GameServerObjectFindPool&& other) noexcept = default;

public:
	GameServerObjectFindPool& operator=(const GameServerObjectFindPool& other) = delete;
	GameServerObjectFindPool& operator=(GameServerObjectFindPool&& other) = delete;

public: // Member Function
	void Push(DataType* data);
	void Erase(DataType* data);
	bool IsEmpty();
};

template <typename DataType>
GameServerObjectFindPool<DataType>::~GameServerObjectFindPool()
{
	for (const DataType* data : m_QueuePool)
	{
		delete data;
	}
}

template <typename DataType>
void GameServerObjectFindPool<DataType>::Push(DataType* data)
{
	std::lock_guard lock(m_QueueLock);
	m_QueuePool.insert(data);
}

template <typename DataType>
void GameServerObjectFindPool<DataType>::Erase(DataType* data)
{
	std::lock_guard lock(m_QueueLock);
	m_QueuePool.erase(data);
}

template <typename DataType>
bool GameServerObjectFindPool<DataType>::IsEmpty()
{
	std::lock_guard lock(m_QueueLock);
	return m_QueuePool.empty();
}