#include "turtle_handler.h"
#include "turtle_chopper.h"

#include "../../libs/logify.h"
#include "../../libs/netify.h"
#include <string.h>

char *turtle_handler_handle(char *resource_buf, char *header_buf, char *body_buf)
{
    char *specialization_buf = netify_request_header_get("specialization", header_buf);
    if (!specialization_buf) {
        logify_log(ERROR, "Missing `specialization` header");
    }

    char *response = NULL;

    if (strcmp(specialization_buf, "chopper-1") == 0) {
        response = turtle_chopper_handler_v1(body_buf);
    } else {
        logify_log(ERROR, "Unsupported specialization provided: %s", specialization_buf);
        return NULL;
    }

    /*
        cJSON *root = cJSON_Parse(req_body_buf);
        if (root == NULL) {
            logify_log(ERROR, "Failed to parse json\n");
        }

        cJSON *name = cJSON_GetObjectItem(root, "name");
        if (cJSON_IsString(name) && (name->valuestring != NULL)) {
            logify_log(DEBUG, "Name: %s\n", name->valuestring);
        } else {
            logify_log(ERROR, "Could not find 'name' string\n");
        }

        cJSON_Delete(root);
*/

    return response;
}
