#pragma once

#include <list>
#include <vector>
#include <map>

template <typename T, typename K>
class MultilineQueue
{
private:
	struct Node;
	struct PresenceTable;

	std::list<Node>* m_queues;
	std::map<K, std::list<Node>*> m_map;

	struct Node
	{
		T m_item;
		PresenceTable* m_presenceTable;

		Node(T item, PresenceTable* presenceTable) : m_item(item), m_presenceTable(presenceTable) {}
	};

	struct LineIterator
	{
		std::list<Node>* queue;
		typename std::list<Node>::iterator it;
	};

	struct PresenceTable
	{
		LineIterator* m_iterators;
		int m_nodeCount;

		PresenceTable(int count)
		{
			m_nodeCount = count;
			m_iterators = new LineIterator[count];
		}

		~PresenceTable()
		{
			delete[] m_iterators;
		}
	};

public:
	void enqueue(T item, const std::list<K>& lines)
	{
		PresenceTable* presenceTable = new PresenceTable(lines.size());

		auto queue_key = lines.begin();
		for (int i = 0; i < lines.size(); i++)
		{
			_ASSERT(assertKeyExists(*queue_key));

			auto queue = m_map[*queue_key];

			queue->emplace_front(item, presenceTable);
			presenceTable->m_iterators[i] = { queue, queue->begin() };
			queue_key++;
		}
	}

	T dequeue(K queue_key)
	{
		_ASSERT(assertKeyExists(queue_key));
		_ASSERT(size(queue_key) > 0);

		Node node = m_map[queue_key]->back();
		PresenceTable* presenceTable = node.m_presenceTable;

		for (int i = 0; i < presenceTable->m_nodeCount; i++)
		{
			auto lineIterator = presenceTable->m_iterators[i];
			lineIterator.queue->erase(lineIterator.it);
		}

		delete presenceTable;
		return node.m_item;
	}

	int size(K queue_key)
	{
		_ASSERT(assertKeyExists(queue_key));
		return m_map[queue_key]->size();
	}

	MultilineQueue() {}

	MultilineQueue(const std::list<K>& lines)
	{
		m_queues = new std::list<Node>[lines.size()];

		auto queue_key = lines.begin();
		for (int i = 0; i < lines.size(); i++)
		{
			m_queues[i] = std::list<Node>();
			m_map[*queue_key] = &m_queues[i];
			queue_key++;
		}
	}

#ifdef _DEBUG
	bool assertKeyExists(K queue_key) {
		auto it = m_map.find(queue_key);
		if (it == m_map.end())
			return false;
		return true;
	}
#endif
};
