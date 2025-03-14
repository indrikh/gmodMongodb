#ifndef GMODMONGODB_AGGREGATEQUERY_H
#define GMODMONGODB_AGGREGATEQUERY_H

#include "IQuery.h"
#include "Collection.h"
#include <vector>

// Данные запроса агрегации
class AggregateQueryData : public IQueryData {
public:
    AggregateQueryData(const std::string &pipeline);

    std::string pipeline; // JSON-строка с конвейером агрегации
    std::vector<bsoncxx::document::value> results; // Результаты агрегации
};

// Класс для запросов агрегации
class AggregateQuery : public IQuery {
public:
    AggregateQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Установка конвейера агрегации
    void setPipeline(const std::string &pipeline);

    // Добавление стадии конвейера
    void addStage(const std::string &stage);

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

    // Получение результатов
    std::vector<bsoncxx::document::value>& getResults();

private:
    std::shared_ptr<Collection> m_collection;
    std::string m_pipeline = "[]";
    std::vector<std::string> m_stages;
    std::vector<bsoncxx::document::value> m_results;
};

#endif // GMODMONGODB_AGGREGATEQUERY_H