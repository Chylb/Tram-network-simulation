#include <list>
#include <vector>

template <typename T, int L>
class MultilineQueue
{
    struct Node;
    struct PresenceTable;

    std::list<Node> m_queues[L];

    struct Node
    {
        T* m_item;
        PresenceTable* m_presenceTable;

        Node(T* item, PresenceTable* presenceTable) : m_item(item), m_presenceTable(presenceTable)
        {
        }
    };

    struct LineIterator
    {
        int line;
        typename std::list<Node>::iterator it;
    };

    struct PresenceTable
    {
        LineIterator* m_iterators;
        int m_iteratorCount;

        PresenceTable(int count)
        {
            m_iteratorCount = count;
            m_iterators = new LineIterator[count];
        }

        ~PresenceTable()
        {
            delete[] m_iterators;
        }
    };

public:
    void enqueue(T* item, const std::list<int>& lines)
    {
        PresenceTable* presenceTable = new PresenceTable(lines.size());

        auto line = lines.begin();
        for (int i = 0; i < lines.size(); i++)
        {
            m_queues[*line].emplace_front(item, presenceTable);
            presenceTable->m_iterators[i] = { *line, m_queues[*line].begin() };
            line++;
        }
    }

    T* dequeue(int line)
    {
        if (size(line) == 0)
            return nullptr;

        Node node = m_queues[line].back();
        PresenceTable* presenceTable = node.m_presenceTable;

        for (int i = 0; i < presenceTable->m_iteratorCount; i++)
        {
            auto lineIterator = presenceTable->m_iterators[i];
            m_queues[lineIterator.line].erase(lineIterator.it);
        }

        delete presenceTable;
        return node.m_item;
    }

    int size(int line)
    {
        return m_queues[line].size();
    }
};
