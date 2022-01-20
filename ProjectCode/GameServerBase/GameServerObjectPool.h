#pragma once

// 용도 : 
// 분류 :
// 첨언 :
template <typename DataType>
class GameServerObjectPool
{
private: // Member Var
	std::deque<DataType*>	m_QueuePool;
	std::mutex				m_QueueLock;

public: // Default
	GameServerObjectPool() = default;
	~GameServerObjectPool() = default;

	GameServerObjectPool(const GameServerObjectPool& other) = delete;
	GameServerObjectPool(GameServerObjectPool&& other) noexcept = default;

public:
	GameServerObjectPool& operator=(const GameServerObjectPool& other) = delete;
	GameServerObjectPool& operator=(GameServerObjectPool&& other) = delete;

private:

public: // Member Function
	DataType*	Pop();
	void		Push(DataType* data);
};

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