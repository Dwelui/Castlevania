#include "turtlefy.h"
#include "cJSON.h"
#include "logify.h"
#include <string.h>

const char *turtlefy_turtle_action_get_string(enum TurtleAction action) {
    switch (action) {
    case 0:
        return "standby";
    case 1:
        return "chopping";
    case 2:
        return "replanting";
    }
}

enum TurtleAction turtlefy_turtle_action_enum(const char *action) {
    if (strcmp(action, "standby") == 0) {
        return TURTLE_ACTION_STANDBY;
    } else if (strcmp(action, "chopping") == 0) {
        return TURTLE_ACTION_CHOPPING;
    } else if (strcmp(action, "replanting") == 0) {
        return TURTLE_ACTION_REPLANTING;
    }

    return TURTLE_ACTION_STANDBY;
}

const char *turtlefy_turtle_direction_get(enum TurtleDirection direction) {
    switch (direction) {
    case 0:
        return "none";
    case 1:
        return "forward";
    case 2:
        return "up";
    case 3:
        return "down";
    }
}

void turtlefy_action_set(cJSON *turtle, enum TurtleAction action) {
    cJSON *action_item = cJSON_GetObjectItem(turtle, "action");
    if (action_item) {
        cJSON_SetValuestring(action_item, turtlefy_turtle_action_get_string(action));
    } else {
        action_item = cJSON_CreateString(turtlefy_turtle_action_get_string(action));
        cJSON_AddItemToObject(turtle, "action", action_item);
    }
}

enum TurtleAction turtlefy_action_get(cJSON *turtle) {
    cJSON *action_item = cJSON_GetObjectItem(turtle, "action");
    if (cJSON_IsString(action_item)) {
        return turtlefy_turtle_action_enum(action_item->valuestring);
    }

    return TURTLE_ACTION_STANDBY;
}

int turtlefy_blocks_contain_tag(cJSON *blocks, const char *target_name, enum TurtleDirection direction) {
    cJSON *block = cJSON_GetObjectItem(blocks, turtlefy_turtle_direction_get(direction));
    cJSON *tags = cJSON_GetObjectItem(block, "tags");
    if (tags) {
        cJSON *target = cJSON_GetObjectItem(tags, target_name);
        if (cJSON_IsBool(target)) {
            return 1;
        }
    }

    return 0;
}
