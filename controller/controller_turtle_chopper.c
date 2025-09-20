#include "../libs/cJSON.h"
#include "../libs/logify.h"
#include "../libs/netify.h"
#include "../libs/turtlefy.h"
#include "../state.h"

// TODO: Move this to agent directory. Because it does not fit the resource controller pattern.
char *controller_turtle_chopper_handler(const char *resource_buf, const char *header_buf, const char *body_buf) {
    const char *id = netify_request_header_get("id", header_buf);
    const char *position = netify_request_header_get("position", header_buf);

    cJSON *turtle = state_turle_upsert(TURTLE_CHOPPER, id, position, DIRECTION_NORTH);

    cJSON *response = cJSON_CreateObject();
    cJSON *action = cJSON_CreateObject();
    cJSON *action_name = cJSON_CreateString("none");
    cJSON_AddItemToObject(response, "action", action);
    cJSON_AddItemToObject(action, "name", action_name);

    cJSON *request_body_json = cJSON_Parse(body_buf);

    logify_log(DEBUG, "Request: %s\n", cJSON_Print(request_body_json));
    if (!request_body_json) {
        char *result = cJSON_Print(response);
        cJSON_Delete(response);

        return result;
    }

    cJSON *blocks = cJSON_GetObjectItem(request_body_json, "blocks");
    if (turtlefy_blocks_contain_tag(blocks, "minecraft:logs", TURTLE_DIRECTION_FORWARD)) {
        turtlefy_action_set(turtle, TURTLE_ACTION_CHOPPING);
    }

    if (blocks && turtlefy_action_get(turtle) == TURTLE_ACTION_CHOPPING) {
        if (turtlefy_blocks_contain_tag(blocks, "minecraft:logs", TURTLE_DIRECTION_FORWARD)) {
            cJSON_SetValuestring(action_name, "turtle.dig()");
        } else if (turtlefy_blocks_contain_tag(blocks, "minecraft:leaves", TURTLE_DIRECTION_UP)) {
            cJSON_SetValuestring(action_name, "turtle.digUp()");
        } else if (!turtlefy_blocks_contain_tag(blocks, "minecraft:leaves", TURTLE_DIRECTION_UP)) {
            // 1. Need to save action history
            // 2. If previous action was up it means there is no more wood to harvest and set action to replanting
            cJSON_SetValuestring(action_name, "turtle.up()");
        } else {
            turtlefy_action_set(turtle, TURTLE_ACTION_REPLANTING);
        }
    }

    if (blocks && turtlefy_action_get(turtle) == TURTLE_ACTION_REPLANTING) {
        if (turtlefy_blocks_contain_tag(blocks, "minecraft:dirt", TURTLE_DIRECTION_DOWN)) {
            turtlefy_action_set(turtle, TURTLE_ACTION_STANDBY);
        } else {
            cJSON_SetValuestring(action_name, "turtle.down()");
        }
    }

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

    // cJSON *root = cJSON_Parse(body_buf);
    // if (root == NULL) {
    //     logify_log(ERROR, "Failed to parse json %s", body_buf);
    // }
    //
    // cJSON *actions = cJSON_CreateArray();
    // cJSON_AddItemToObject(response, "actions", actions);
    //
    // cJSON *tags = cJSON_GetObjectItem(root, "tags");
    // if (tags) {
    //     cJSON *logs = cJSON_GetObjectItem(tags, "minecraft:logs");
    //     if (cJSON_IsBool(logs)) {
    //         cJSON_AddItemToArray(actions, cJSON_CreateString("turtle.dig()"));
    //         cJSON_AddItemToArray(actions, cJSON_CreateString("turtle.inspect()"));
    //     }
    // }

    char *result = cJSON_Print(response);
    cJSON_Delete(response);

    return result;
}
