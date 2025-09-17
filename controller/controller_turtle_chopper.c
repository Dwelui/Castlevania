#include "../libs/cJSON.h"
#include "../libs/logify.h"
#include "../libs/netify.h"
#include "../state.h"

#include <stdlib.h>

// TODO: Move this to agent directory. Because it does not fit the resource controller pattern.
char *controller_turtle_chopper_handler(const char *resource_buf, const char *header_buf, const char *body_buf) {
    char *computer_id = netify_request_header_get("computer-id", header_buf);
    Turtle *turtle = state_upsert_turle(1, 123, 123, 123);

    // 1. Save turtle state. So the server can keep track of what is happening between requests.
    // Save id, position, direction, updated_at, actions(a structure stack) (Later will need to save fuel and inventory)
    // 2. If conditions are meet push action to actions. Action should contain needed data so that if new actions if pushed and later resolved it can come back to finish previous action. Example need fuel, full inventory, etc...
    // 3. "chopper-cut" action is pushed to actions stack if turtle of type "chopper" is closest to dedicated block for "tree-chopping" that is in state "tree-grown"
    // 4. Dedicated block has "tree-growing", "tree-grown", "tree-felling", "tree-planting"
    // 5. "chopper-check" action is pushed to action stack if turtle of type "chopper" is closest to dedicated block for "tree-chopping" that in in state "tree-growing" and enough time elapsed since last "updated_at"





    cJSON *response = cJSON_CreateObject();

    cJSON *root = cJSON_Parse(body_buf);
    if (root == NULL) {
        logify_log(ERROR, "Failed to parse json %s", body_buf);
    }

    cJSON *actions = cJSON_CreateArray();
    cJSON_AddItemToObject(response, "actions", actions);

    cJSON *tags = cJSON_GetObjectItem(root, "tags");
    if (tags) {
        cJSON *logs = cJSON_GetObjectItem(tags, "minecraft:logs");
        if (cJSON_IsBool(logs)) {
            cJSON_AddItemToArray(actions, cJSON_CreateString("turtle.dig()"));
            cJSON_AddItemToArray(actions, cJSON_CreateString("turtle.inspect()"));
        }
    }

    char *result = cJSON_Print(response);
    cJSON_Delete(response);

    return result;
}
