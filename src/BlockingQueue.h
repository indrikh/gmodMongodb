#ifndef GMODMONGODB_BLOCKINGQUEUE_H
#define GMODMONGODB_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>

// Потокобезопасная блокирующая очередь для безопасного обмена данными между потоками
template<typename T>
class BlockingQueue {
public:
    // Добавление элемента в очередь
    void push(const T &item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(item);
        m_condition.notify_one();
    }

    // Извлечение элемента из очереди с блокировкой, если очередь пуста
    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty(); });
        T item = m_queue.front();
        m_queue.pop_front();
        return item;
    }

    // Попытка получить элемент без блокировки
    bool tryPop(T &item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        item = m_queue.front();
        m_queue.pop_front();
        return true;
    }

    // Очистка очереди и получение всех элементов
    std::deque<T> clear() {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::deque<T> result;
        m_queue.swap(result);
        return result;
    }

    // Проверка на пустоту
    bool empty() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    // Получение размера очереди
    size_t size() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::deque<T> m_queue;
};

#endif // GMODMONGODB_BLOCKINGQUEUE_H