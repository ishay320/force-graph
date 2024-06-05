#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

// struct node;
struct node {
    struct node *nodes;
    size_t len;
    size_t cap;
    void *data;
};

struct data {
    char *name;
};

struct node *create_graph(struct data *root_data)
{
    struct node *root = calloc(1, sizeof(struct node));
    root->data        = root_data;
    return root;
}

void destroy_graph(struct node *node)
{
    assert(false && "not implemented yet");
}

int main(int argc, char *argv[])
{
    const int screenWidth  = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "image");

    SetTargetFPS(60);
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        /* draw here */
        EndDrawing();
    }
    return EXIT_SUCCESS;
}
