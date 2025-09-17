#include "../libs/cJSON.h"
#include "controller_dedicated_block.h"

char *controller_dedicated_block_tree_chopping_store_action(const char *resource_buf, const char *header_buf, const char *body_buf) {
    cJSON *response = cJSON_CreateObject();

    char *result = cJSON_Print(response);
    cJSON_Delete(response);

    return result;
}
