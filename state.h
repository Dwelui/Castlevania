#pragma once

typedef struct {
    int id;
    int x, y, z;
} Turtle;

#define STATE_TURTLES_MAX 128

typedef struct {
    int total_requests;

    Turtle turtles[STATE_TURTLES_MAX];
    int turtle_len;
} AppState;

extern AppState g_state;

void state_init();
void state_destroy();

Turtle *state_get_turtle(int id);
Turtle *state_upsert_turle(int id, int x, int y, int z);
