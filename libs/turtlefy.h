#pragma once

#include "cJSON.h"

enum TurtleState {
    TURTLE_STATE_STANDBY = 0,
    TURTLE_STATE_CHOPPING = 1,
    TURTLE_STATE_REPLANTING = 2,
};

enum TurtleDirection {
    TURTLE_DIRECTION_NONE = 0,
    TURTLE_DIRECTION_FORWARD = 1,
    TURTLE_DIRECTION_UP = 2,
    TURTLE_DIRECTION_DOWN = 3,
};

void turtlefy_state_set(cJSON *turtle, enum TurtleState action);
enum TurtleState turtlefy_state_get(cJSON *turtle);

int turtlefy_blocks_contain_tag(cJSON *block, const char* target_name, enum TurtleDirection direction);
