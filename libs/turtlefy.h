#pragma once

#include "cJSON.h"

enum TurtleAction {
    TURTLE_ACTION_STANDBY = 0,
    TURTLE_ACTION_CHOPPING = 1,
    TURTLE_ACTION_REPLANTING = 2,
};

enum TurtleDirection {
    TURTLE_DIRECTION_NONE = 0,
    TURTLE_DIRECTION_FORWARD = 1,
    TURTLE_DIRECTION_UP = 2,
    TURTLE_DIRECTION_DOWN = 3,
};

void turtlefy_action_set(cJSON *turtle, enum TurtleAction action);
enum TurtleAction turtlefy_action_get(cJSON *turtle);

int turtlefy_blocks_contain_tag(cJSON *block, const char* target_name, enum TurtleDirection direction);
