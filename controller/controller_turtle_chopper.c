#include "../libs/cJSON.h"
#include "../libs/logify.h"

char *controller_turtle_chopper_handler(const char *resource_buf, const char *header_buf, const char *body_buf) {
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
