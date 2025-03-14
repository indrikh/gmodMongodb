#ifndef GMODMONGODB_DELETEQUERY_H
#define GMODMONGODB_DELETEQUERY_H

#include "IQuery.h"
#include "Collection.h"

// Данные запроса удаления
class DeleteQueryData : public IQueryData {
public:
    DeleteQueryData(const std::string &filter, bool multi);

    std::string filter; // BSON-фильтр для выбора документов на удаление
    bool multi; // Удалить все соответствующие документы или только первый
    int64_t deletedCount = 0; // Количество удаленных документов
};

// Класс для запросов удаления документов
class DeleteQuery : public IQuery {
public:
    DeleteQuery(std::shared_ptr<Database> database, std::shared_ptr<Collection> collection);

    // Выполнение запроса
    void execute(std::shared_ptr<IQueryData> data) override;

    // Установка фильтра
    void setFilter(const std::string &filter);

    // Установка режима мультиудаления
    void setMulti(bool multi);

    // Получение коллекции
    std::shared_ptr<Collection> getCollection() const;

    // Получение количества удаленных документов
    int64_t getDeletedCount() const;

private:
    std::shared_ptr<Collection> m_collection;
    std::string m_filter = "{}";
    bool m_multi = false;
    int64_t m_deletedCount = 0;
};

#endif // GMODMONGODB_DELETEQUERY_H