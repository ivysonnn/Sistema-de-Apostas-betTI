#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <ctime>
#include <pqxx/pqxx>
#include "SportService.h"
#include "ParticipantsService.h"
#include "../repository/EventRepository.h"
#include "../entities/EventEntity.h"
#include "../entities/ParticipantsEntity.h"

using namespace std;

class EventService {
public:
    void save(pqxx::connection *conn, EventEntity *entity);

    void update(pqxx::connection *conn, EventEntity *entity);

    optional<EventEntity> findById(pqxx::connection *conn, size_t id);

    optional<vector<EventEntity>> findAll(pqxx::connection *conn);

private:
    void setTableName(QueryMetaData *queryMetaData);

    EventEntity createEntityFromResult(pqxx::connection *conn, const pqxx::row& row);

    std::vector<EventEntity> processFindAll(pqxx::connection *conn, pqxx::result res);

    std::array<double, 3> parseNumbers(const std::string& str);

    void calculateOdds(EventEntity *entity, ParticipantsEntity teamA, ParticipantsEntity teamB);

    std::array<double, 3> calculateOddsHelper(size_t winsTeamA, size_t winsTeamB);
};

#endif
