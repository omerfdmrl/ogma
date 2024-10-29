#ifndef OGMA_DB_MONGO_H

#define OGMA_DB_MONGO_H

#include "ogma.h"

mongoc_client_t *client;
mongoc_server_api_t *api;
mongoc_database_t *database;

mongoc_database_t *init_db(const char *url, const char *dbName) {
    bson_error_t error = {0};
    bson_t reply = BSON_INITIALIZER;

    bool ok = true;
    mongoc_init();

    client = mongoc_client_new(url);
    assert(!client, Error, "Failed to create a MongoDB client");

    api = mongoc_server_api_new(MONGOC_SERVER_API_V1);
    assert(!api, Error, "Failed to create a MongoDB server API.");

    ok = mongoc_client_set_server_api(client, api, &error);
    assert(!ok, Error, "%s", error.message);

    database = mongoc_client_get_database(client, dbName);
    assert(!database, Error, "Failed to get a MongoDB database handle.");

    bson_t *command = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_database_command_simple(
        database, command, NULL, &reply, &error
    );
    assert(!ok, Error, "%s", error.message);
    
    bson_destroy(&reply);
    logger("Database connection initialized.");
    bson_destroy(command);
    return database;
}
void close_db() {
    mongoc_database_destroy(database);
    mongoc_server_api_destroy(api);
    mongoc_client_destroy(client);
    mongoc_cleanup();
    logger("Database connection successfully closed.");
}
#endif // !OGMA_DB_MONGO_H