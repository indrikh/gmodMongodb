#ifndef GMODMONGODB_DATABASE_H
#define GMODMONGODB_DATABASE_H

#include "MongoHeader.h"
#include <vector>
#include <deque>
#include <thread>
#include <future>
#include <memory>
#include <mutex>
#include <sstream>
#include <unordered_set>
#include <condition_variable>
#include "../BlockingQueue.h"

// Предварительные объявления классов
class IQuery;
class IQueryData;
class Collection;
class Document;
class BSONDocument;

// Основной класс для работы с MongoDB
class Database : public std::enable_shared_from_this<Database> {
    friend class IQuery;

public:
    // Создание экземпляра базы данных с указанными параметрами подключения
    static std::shared_ptr<Database> createDatabase(const std::string &uri, const std::string &dbName);

    // Деструктор
    ~Database();

    // Постановка запроса в очередь на выполнение
    void enqueueQuery(const std::shared_ptr<IQuery> &query, const std::shared_ptr<IQueryData> &data);

    // Настройка автоматического переподключения
    void setShouldAutoReconnect(bool autoReconnect);

    // Получение коллекции из базы данных
    std::shared_ptr<Collection> getCollection(const std::string &collectionName);

    // Проверка соединения
    bool ping();

    // Подключение к базе данных
    void connect();

    // Ожидание выполнения текущего запроса
    void wait();

    // Прерывание всех ожидающих запросов
    std::deque<std::pair<std::shared_ptr<IQuery>, std::shared_ptr<IQueryData>>> abortAllQueries();

    // Получение текущего статуса соединения
    MongoDBStatus status() const;

    // Получение информации о сервере
    std::string serverInfo();

    // Получение размера очереди запросов
    size_t queueSize();

    // Отключение от базы данных
    void disconnect(bool wait);

    // Установка таймаута соединения
    void setConnectTimeout(unsigned int timeout);

    // Проверка завершения соединения
    bool isConnectionDone() { return m_connectionDone; }

    // Проверка успешности соединения
    bool connectionSuccessful() { return m_success; }

    // Попытка переподключения
    bool attemptReconnect();

    // Получение ошибки соединения
    std::string connectionError() { return m_connection_err; }

    // Получение завершенных запросов
    std::deque<std::pair<std::shared_ptr<IQuery>, std::shared_ptr<IQueryData>>> takeFinishedQueries() {
        return finishedQueries.clear();
    }

    // Проверка был ли разрыв соединения
    bool wasDisconnected();

private:
    // Приватный конструктор для создания через factory-метод
    Database(std::string uri, std::string dbName);

    // Завершение работы базы данных
    void shutdown();

    // Основной цикл обработки запросов
    void run();

    // Выполнение запроса
    void runQuery(const std::shared_ptr<IQuery> &query, const std::shared_ptr<IQueryData> &data, bool retry);

    // Процесс подключения
    void connectRun();

    // Прерывание ожидающего запроса
    void abortWaitingQuery();

    // Пометка ожидающего запроса как неудачного
    void failWaitingQuery(const std::shared_ptr<IQuery> &query, const std::shared_ptr<IQueryData> &data, std::string reason);

    // Ожидание выполнения запроса
    void waitForQuery(const std::shared_ptr<IQuery> &query, const std::shared_ptr<IQueryData> &data);

    // Применение настроек таймаута
    void applyTimeoutSettings();

    // Очереди запросов
    BlockingQueue<std::pair<std::shared_ptr<IQuery>, std::shared_ptr<IQueryData>>> finishedQueries{};
    BlockingQueue<std::pair<std::shared_ptr<IQuery>, std::shared_ptr<IQueryData>>> queryQueue{};

    // MongoDB клиент и инстанс
    mongocxx::client *m_client = nullptr;
    mongocxx::database m_database;
    static mongocxx::instance m_instance; // Статический экземпляр MongoDB

    // Потоки и мьютексы
    std::thread m_thread;
    std::mutex m_connectMutex; // Мьютекс для синхронизации подключения
    std::mutex m_queryMutex;   // Мьютекс для синхронизации запросов
    std::mutex m_queryWaitMutex; // Мьютекс для предотвращения блокировок при вызове wait()
    
    // Переменные условия для синхронизации
    std::condition_variable m_connectWakeupVariable;
    std::condition_variable m_queryWakeupVariable;
    std::condition_variable m_queryWaitWakeupVariable;

    // Информация о сервере
    std::string m_serverInfo;
    std::string m_connection_err;

    // Флаги состояния
    bool shouldAutoReconnect = true;
    bool startedConnecting = false;
    bool m_canWait = false;
    std::pair<std::shared_ptr<IQuery>, std::shared_ptr<IQueryData>> m_waitingQuery = {nullptr, nullptr};
    std::atomic<bool> m_success{true};
    std::atomic<bool> disconnected { false };
    std::atomic<bool> m_connectionDone{false};

    // Данные подключения
    std::string dbName;
    std::string uri;
    unsigned int connectTimeout = 0;
    std::atomic<MongoDBStatus> m_status{MONGODB_NOT_CONNECTED};
};

#endif // GMODMONGODB_DATABASE_H