#include "state.h"
#include "libs/cJSON.h"
#include <sys/time.h>
#include <time.h>

char *get_direction(enum WorldEntityDirection direction) {
    switch (direction) {
    case 0:
        return "unknown";
    case 1:
        return "north";
    case 2:
        return "east";
    case 3:
        return "south";
    case 4:
        return "west";
    case 5:
        return "up";
    case 6:
        return "down";
    }
}

char *get_turtle_type(enum TurtleType type) {
    switch (type) {
    case 0:
        return "chopper";
    }
}

AppState g_state = {0};

void state_init() {
    g_state.root = cJSON_CreateObject();

    cJSON_AddItemToObject(g_state.root, "turtles", cJSON_CreateObject());
}

void state_destroy() { cJSON_Delete(g_state.root); }

cJSON *state_turtle_get(const char *id) {
    cJSON *turtles = cJSON_GetObjectItem(g_state.root, "turtles");

    cJSON *turtle = cJSON_GetObjectItem(turtles, id);
    if (cJSON_IsObject(turtle)) {
        return turtle;
    }

    return NULL;
}

cJSON *state_turle_upsert(enum TurtleType type, const char *id, const char *position, enum WorldEntityDirection direction) {
    // TODO: Move this to Timeify
    struct timeval tv;
    struct tm *tm_info;
    char timestamp[64];

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    cJSON *turtle = state_turtle_get(id);
    if (turtle) {
        cJSON_SetValuestring(cJSON_GetObjectItem(turtle, "type"), get_turtle_type(type));
        cJSON_SetValuestring(cJSON_GetObjectItem(turtle, "position"), position);
        cJSON_SetValuestring(cJSON_GetObjectItem(turtle, "direction"), get_direction(direction));
        cJSON_SetValuestring(cJSON_GetObjectItem(turtle, "updated_at"), timestamp);

        return turtle;
    }

    cJSON *turtles = cJSON_GetObjectItem(g_state.root, "turtles");

    turtle = cJSON_CreateObject();
    cJSON_AddStringToObject(turtle, "type", get_turtle_type(type));
    cJSON_AddStringToObject(turtle, "position", position);
    cJSON_AddStringToObject(turtle, "direction", get_direction(direction));
    cJSON_AddStringToObject(turtle, "updated_at", timestamp);
    cJSON_AddStringToObject(turtle, "created_at", timestamp);

    cJSON_AddItemToObject(turtles, id, turtle);

    return turtle;
}
