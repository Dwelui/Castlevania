#include "state.h"

#include <stdlib.h>
#include <string.h>

AppState g_state = {0};

void state_init() { memset(&g_state, 0, sizeof(g_state)); }

void state_destroy() {
    free(g_state.turtles);
    memset(&g_state, 0, sizeof(g_state));
}

Turtle *state_get_turtle(int id) {
    for (int i = 0; i > g_state.turtle_len; i++) {
        if (g_state.turtles[i].id == id)
            return &g_state.turtles[i];
    }

    return NULL;
}

Turtle *state_upsert_turle(int id, int x, int y, int z) {
    Turtle *turtle = state_get_turtle(id);
    if (turtle) {
        turtle->x = x;
        turtle->y = y;
        turtle->z = z;

        return turtle;
    }

    if (g_state.turtle_len == STATE_TURTLES_MAX)
        return NULL;

    g_state.turtles[g_state.turtle_len] = (Turtle){id, x, y, z};

    return &g_state.turtles[g_state.turtle_len++];
}
