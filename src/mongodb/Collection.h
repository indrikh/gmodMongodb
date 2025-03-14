#ifndef GMODMONGODB_COLLECTION_H
#define GMODMONGODB_COLLECTION_H

#include "MongoHeader.h"
#include <memory>
#include <string>

// Предварительные объявления
class Database;
class IQuery;
class Document;
class FindQuery;
class InsertQuery;
class UpdateQuery;
class DeleteQuery;
class CountQuery;
class AggregateQuery;

// Класс для работы с коллекцией MongoDB
class Collection : public std::enable_shared_from_this<Collection> {
public:
    // Конструктор
    Collection(std::shared_ptr<Database> database, const std::string &name);

    // Получение имени коллекции
    std::string getName() const;

    // Поиск документов в коллекции
    std::shared_ptr<FindQuery> find(const std::string &filter = "{}");

    // Вставка документов в коллекцию
    std::shared_ptr<InsertQuery> insert(const std::string &document);

    // Обновление документов в коллекции
    std::shared_ptr<UpdateQuery> update(const std::string &filter, const std::string &update, bool multi = false);

    // Удаление документов из коллекции
    std::shared_ptr<DeleteQuery> remove(const std::string &filter, bool multi = false);

    // Подсчет документов в коллекции
    std::shared_ptr<CountQuery> count(const std::string &filter = "{}");

    // Агрегация данных в коллекции
    std::shared_ptr<AggregateQuery> aggregate(const std::string &pipeline);

    // Создание индекса
    bool createIndex(const std::string &keys, bool unique = false);

    // Удаление индекса
    bool dropIndex(const std::string &indexName);

    // Получение базы данных, к которой принадлежит коллекция
    std::shared_ptr<Database> getDatabase() const;

    // Доступ к нативной коллекции MongoDB
    mongocxx::collection& getNativeCollection();

private:
    std::shared_ptr<Database> m_database;
    std::string m_name;
    mongocxx::collection m_collection;
};

#endif // GMODMONGODB_COLLECTION_H