#include "turtlefy.h"
#include "cJSON.h"
#include "logify.h"
#include <string.h>

const char *turtlefy_turtle_state_get_string(enum TurtleState state) {
    switch (state) {
    case 0:
        return "standby";
    case 1:
        return "chopping";
    case 2:
        return "replanting";
    default:
        logify_log(WARNING, "TURTLEFY::Failed to get string for turtle state %d", state);
        return "standby";
    }
}

enum TurtleState turtlefy_turtle_state_enum(const char *state) {
    if (strcmp(state, "standby") == 0) {
        return TURTLE_STATE_STANDBY;
    } else if (strcmp(state, "chopping") == 0) {
        return TURTLE_STATE_CHOPPING;
    } else if (strcmp(state, "replanting") == 0) {
        return TURTLE_STATE_REPLANTING;
    }

    logify_log(WARNING, "TURTLEFY::Failed to get enum for turtle state %s", state);
    return TURTLE_STATE_STANDBY;
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
    default:
        logify_log(WARNING, "TURTLEFY::Failed to get turtle direction %d", direction);
        return "none";
    }
}

void turtlefy_state_set(cJSON *turtle, enum TurtleState state) {
    cJSON *item = cJSON_GetObjectItem(turtle, "state");
    if (item) {
        cJSON_SetValuestring(item, turtlefy_turtle_state_get_string(state));
    } else {
        item = cJSON_CreateString(turtlefy_turtle_state_get_string(state));
        cJSON_AddItemToObject(turtle, "state", item);
    }
}

enum TurtleState turtlefy_state_get(cJSON *turtle) {
    cJSON *action_item = cJSON_GetObjectItem(turtle, "action");
    if (cJSON_IsString(action_item)) {
        return turtlefy_turtle_state_enum(action_item->valuestring);
    }

    return TURTLE_STATE_STANDBY;
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
