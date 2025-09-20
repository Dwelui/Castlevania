#include "../libs/cJSON.h"
#include "../libs/logify.h"
#include "../libs/netify.h"
#include "../libs/turtlefy.h"
#include "../state.h"

// TODO: Move this to agent directory. Because it does not fit the resource controller pattern.
char *controller_turtle_chopper_handler(const struct HttpRequest *request) {
    const char *id = netify_request_header_get("id", request);
    const char *position = netify_request_header_get("position", request);

    cJSON *turtle = state_turle_upsert(TURTLE_CHOPPER, id, position, DIRECTION_NORTH);
    logify_log(DEBUG, "TEST 1");

    cJSON *response = cJSON_CreateObject();
    cJSON *action = cJSON_CreateObject();
    cJSON *action_name = cJSON_CreateString("none");
    cJSON_AddItemToObject(response, "action", action);
    cJSON_AddItemToObject(action, "name", action_name);

    logify_log(DEBUG, "TEST 2");
    cJSON *request_body_json = cJSON_Parse(request->body);

    logify_log(DEBUG, "Request: %s\n", cJSON_Print(request_body_json));
    if (!request_body_json) {
        logify_log(DEBUG, "ERROR Request: %s\n", request->body);
        char *result = cJSON_Print(response);
        cJSON_Delete(response);

        return result;
    }
    logify_log(DEBUG, "TEST 3");

    cJSON *blocks = cJSON_GetObjectItem(request_body_json, "blocks");
    if (turtlefy_blocks_contain_tag(blocks, "minecraft:logs", TURTLE_DIRECTION_FORWARD)) {
        turtlefy_state_set(turtle, TURTLE_STATE_CHOPPING);
    }

    logify_log(DEBUG, "TEST 4");

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

    logify_log(DEBUG, "TEST 5");

    if (blocks && turtlefy_state_get(turtle) == TURTLE_STATE_REPLANTING) {
        if (turtlefy_blocks_contain_tag(blocks, "minecraft:dirt", TURTLE_DIRECTION_DOWN)) {
            turtlefy_state_set(turtle, TURTLE_STATE_STANDBY);
        } else {
            cJSON_SetValuestring(action_name, "turtle.down()");
        }
    }

    logify_log(DEBUG, "TEST 6");

    logify_log(DEBUG, "STATE: %s\n", cJSON_Print(g_state.root));

    // 1. Save turtle state. So the server can keep track of what is happening between requests.
    // Save id, position, direction, updated_at, actions(a structure stack) (Later will need to save fuel and inventory)
    // 2. If conditions are meet push action to actions. Action should contain needed data so that if new actions if pushed and later
    // resolved it can come back to finish previous action. Example need fuel, full inventory, etc...
    // 3. "chopper-cut" action is pushed to actions stack if turtle of type "chopper" is closest to dedicated block for "tree-chopping" that
    // is in state "tree-grown"
    // 4. Dedicated block has "tree-growing", "tree-grown", "tree-felling", "tree-planting"
    // 5. "chopper-check" action is pushed to action stack if turtle of type "chopper" is closest to dedicated block for "tree-chopping"
    // that in in state "tree-growing" and enough time elapsed since last "updated_at"

    logify_log(DEBUG, "TEST 7");

    char *result = cJSON_Print(response);
    cJSON_Delete(response);

    return result;
}
