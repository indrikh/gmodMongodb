#ifndef GMODMONGODB_FINDQUERY_H
#define GMODMONGODB_FINDQUERY_H

#include "IQuery.h"
#include "Collection.h"
#include <vector>

// Данные запроса поиска
class FindQueryData : public IQueryData {
public:
    FindQueryData(const std::string &filter, mongocxx::options::find options);

    std::string filter; // BSON-фильтр для запроса
    mongocxx::options::find options; // Опции запроса
    std::vector<bsoncxx::document::value> results; // Результаты запроса
};

// Класс для запросов поиска документов
class FindQuery : public IQuery {
public:
    FindQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Настройка фильтра для поиска
    void setFilter(const std::string &filter);

    // Настройка сортировки
    void setSort(const std::string &sort);

    // Настройка проекции (выборки полей)
    void setProjection(const std::string &projection);

    // Ограничение количества результатов
    void setLimit(int limit);

    // Установка смещения
    void setSkip(int skip);

    // Получение результатов
    std::vector<bsoncxx::document::value>& getResults();

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

private:
    std::shared_ptr<Collection> m_collection;
    std::string m_filter = "{}";
    std::string m_sort = "";
    std::string m_projection = "";
    int m_limit = 0;
    int m_skip = 0;
    std::vector<bsoncxx::document::value> m_results;
};

#endif // GMODMONGODB_FINDQUERY_H