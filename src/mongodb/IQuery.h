#ifndef GMODMONGODB_IQUERY_H
#define GMODMONGODB_IQUERY_H

#include "MongoHeader.h"
#include <memory>
#include <mutex>
#include <string>
#include <functional>

class Database;
class IQueryData;

// Базовый интерфейс для всех типов запросов
class IQuery : public std::enable_shared_from_this<IQuery> {
public:
    // Конструктор
    explicit IQuery(std::shared_ptr<Database> database);

    // Виртуальный деструктор
    virtual ~IQuery() = default;

    // Выполнение запроса
    virtual void execute(std::shared_ptr<IQueryData> data) = 0;

    // Сеттеры для callback-функций
    virtual void setSuccessCallback(int callback);
    virtual void setFailCallback(int callback);
    virtual void setAbortCallback(int callback);
    virtual void setDataCallback(int callback);

    // Настройки запроса
    virtual void setTimeout(unsigned int timeout);
    virtual void setOption(unsigned int option);

    // Запуск запроса
    virtual void start();

    // Прерывание запроса
    virtual void abort();

    // Ожидание выполнения запроса
    virtual bool wait();

    // Геттеры для состояния запроса
    virtual MongoDBQueryStatus status() const;
    virtual std::string error() const;
    virtual bool isRunning() const;
    virtual bool isSuccess() const;
    virtual bool hasCallback() const;
    virtual unsigned int affectedRows() const;

    // Получение базы данных
    std::shared_ptr<Database> getDatabase() const;

    // Доступ к связанным с запросом callback-ам
    int getSuccessCallback() const;
    int getFailCallback() const;
    int getAbortCallback() const;
    int getDataCallback() const;

    // Установка ошибки
    void setError(const std::string &error);

protected:
    // Защищенные методы для потомков
    void setStatus(MongoDBQueryStatus status);
    void setAffectedRows(unsigned int affectedRows);
    virtual void onSuccess();
    virtual void onFail();
    virtual void onAbort();

    // Защищенные переменные
    std::shared_ptr<Database> m_database;
    std::atomic<MongoDBQueryStatus> m_status{QUERY_NOT_RUNNING};
    std::string m_error;
    unsigned int m_affectedRows = 0;
    unsigned int m_timeout = 0;
    unsigned int m_options = OPTION_DEFAULT;
    std::mutex m_mutex;

    // Callback-и для Lua
    int m_successCallback = 0;
    int m_failCallback = 0;
    int m_abortCallback = 0;
    int m_dataCallback = 0;
};

// Интерфейс для данных запроса
class IQueryData {
public:
    virtual ~IQueryData() = default;
};

#endif // GMODMONGODB_IQUERY_H