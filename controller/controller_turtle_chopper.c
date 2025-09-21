#include "../libs/cJSON.h"
#include "../libs/logify.h"
#include "../libs/netify.h"
#include "../libs/turtlefy.h"
#include "../state.h"
#include <string.h>

// TODO: Make the headers to lower case or search into case insensitive
struct HttpResponse *controller_turtle_chopper_handler(const struct HttpRequest *request) {
    struct HttpResponse *response = netify_response_create(HTTP_OK);

    char *response_body_buf = NULL;

    const char *id = netify_request_header_get("id", request);
    const char *position = netify_request_header_get("position", request);

    logify_log(DEBUG, "Extracted headers id: %s, position %s", id, position);

    return response;

    cJSON *turtle = state_turle_upsert(TURTLE_CHOPPER, id, position, DIRECTION_NORTH);

    cJSON *response_json = cJSON_CreateObject();
    cJSON *action = cJSON_CreateObject();
    cJSON *action_name = cJSON_CreateString("none");
    cJSON_AddItemToObject(response_json, "action", action);
    cJSON_AddItemToObject(action, "name", action_name);

    cJSON *request_body_json = cJSON_Parse(request->body);
    logify_log(DEBUG, "Request: %s", request->body);
    if (!request_body_json) {
        logify_log(DEBUG, "ERROR Request: %s\n", request->body);
        cJSON_Delete(response_json);

        response->status = HTTP_BAD_REQUEST;

        return response;
    }

    logify_log(DEBUG, "Request: %s\n", cJSON_Print(request_body_json));

    cJSON *blocks = cJSON_GetObjectItem(request_body_json, "blocks");
    if (turtlefy_blocks_contain_tag(blocks, "minecraft:logs", TURTLE_DIRECTION_FORWARD)) {
        turtlefy_state_set(turtle, TURTLE_STATE_CHOPPING);
    }

    if (blocks && turtlefy_state_get(turtle) == TURTLE_STATE_CHOPPING) {
        if (turtlefy_blocks_contain_tag(blocks, "minecraft:logs", TURTLE_DIRECTION_FORWARD)) {
            cJSON_SetValuestring(action_name, "turtle.dig()");
        } else if (turtlefy_blocks_contain_tag(blocks, "minecraft:leaves", TURTLE_DIRECTION_UP)) {
            cJSON_SetValuestring(action_name, "turtle.digUp()");
        } else if (!turtlefy_blocks_contain_tag(blocks, "minecraft:leaves", TURTLE_DIRECTION_UP)) {
            // 1. Need to save action history
            // 2. If previous action was up it means there is no more wood to harvest and set action to replanting
            cJSON_SetValuestring(action_name, "turtle.up()");
        } else {
            turtlefy_state_set(turtle, TURTLE_STATE_REPLANTING);
        }
    }

    if (blocks && turtlefy_state_get(turtle) == TURTLE_STATE_REPLANTING) {
        if (turtlefy_blocks_contain_tag(blocks, "minecraft:dirt", TURTLE_DIRECTION_DOWN)) {
            turtlefy_state_set(turtle, TURTLE_STATE_STANDBY);
        } else {
            cJSON_SetValuestring(action_name, "turtle.down()");
        }
    }

    logify_log(DEBUG, "STATE: %s\n", cJSON_Print(g_state.root));

    response_body_buf = cJSON_Print(response_json);
    if (response_body_buf) {
        strncpy(response->body, response_body_buf, strlen(response_body_buf) + 1);
    }

    cJSON_Delete(blocks);
    cJSON_Delete(response_json);

    return response;
}
