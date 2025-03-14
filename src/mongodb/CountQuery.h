#ifndef GMODMONGODB_COUNTQUERY_H
#define GMODMONGODB_COUNTQUERY_H

#include "IQuery.h"
#include "Collection.h"

// Данные запроса на подсчет документов
class CountQueryData : public IQueryData {
public:
    CountQueryData(const std::string &filter);

    std::string filter; // BSON-фильтр для выбора документов
    int64_t count = 0; // Результат подсчета
};

// Класс для запросов на подсчет документов
class CountQuery : public IQuery {
public:
    CountQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Установка фильтра
    void setFilter(const std::string &filter);

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

    // Получение результата подсчета
    int64_t getCount() const;

private:
    std::shared_ptr<Collection> m_collection;
    std::string m_filter = "{}";
    int64_t m_count = 0;
};

#endif // GMODMONGODB_COUNTQUERY_H