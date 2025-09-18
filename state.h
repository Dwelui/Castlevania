#pragma once

#include "libs/cJSON.h"

enum TurtleType { TURTLE_CHOPPER = 0 };

enum WorldEntityDirection {
    DIRECTION_UNKNOWN = 0,
    DIRECTION_NORTH = 1, // -z
    DIRECTION_EAST = 2,  // +x
    DIRECTION_SOUTH = 3, // +z
    DIRECTION_WEST = 4,  // -x
};

typedef struct {
    int total_requests;

    cJSON *root;
} AppState;

extern AppState g_state;

void state_init();
void state_destroy();

cJSON *state_turtle_get(const char *id);
cJSON *state_turle_upsert(enum TurtleType type, const char *id, const char *position, enum WorldEntityDirection direction);
