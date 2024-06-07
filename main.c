#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "da.h"
#include "raylib.h"

struct payload {
    char *name;

    float pos_x;
    float pos_y;

    float vel_x;
    float vel_y;
};

struct node {
    struct node **data;
    size_t len;
    size_t cap;
    struct payload payload;
};

struct node *node_create(struct payload payload)
{
    struct node *root = malloc(sizeof(*root));
    root->payload     = payload;
    root->len         = 0;
    root->cap         = 0;
    root->data        = 0;
    return root;
}

void node_push(struct node *root, struct payload payload)
{
    struct node *node = node_create(payload);
    da_push(*root, node);
    root->data[root->len++] = node;
}

void destroy_graph(struct node *node)
{
    assert(false && "not implemented yet");
}

void graph_draw(struct node *root)
{
    DrawCircle(root->payload.pos_x, root->payload.pos_y, 5, RED);
    for (size_t i = 0; i < root->len; i++) {
        DrawLine(root->payload.pos_x, root->payload.pos_y,
                 root->data[i]->payload.pos_x, root->data[i]->payload.pos_y,
                 BLUE);
        graph_draw(root->data[i]);
    }
}

Vector2 middle_of_screen()
{
    return (Vector2){GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
}

void graph_update(struct node *root)
{
    const float scale = 0.00001;

    for (size_t i = 0; i < root->len; i++) {
        root->payload.vel_x +=
            (root->data[i]->payload.pos_x - root->payload.pos_x) * scale;
        root->payload.vel_y +=
            (root->data[i]->payload.pos_y - root->payload.pos_y) * scale;

        root->data[i]->payload.vel_x +=
            (root->payload.pos_x - root->data[i]->payload.pos_x) * scale;
        root->data[i]->payload.vel_y +=
            (root->payload.pos_y - root->data[i]->payload.pos_y) * scale;
        graph_update(root->data[i]);
    }

    // pull to the middle of the screen
    Vector2 middle = middle_of_screen();
    root->payload.vel_x += (middle.x - root->payload.pos_x) * scale;
    root->payload.vel_y += (middle.y - root->payload.pos_y) * scale;

    // update pos
    root->payload.pos_x += root->payload.vel_x;
    root->payload.pos_y += root->payload.vel_y;
}

void add_random_node(struct node *root, size_t number)
{
    for (size_t i = 0; i < number; i++) {
        node_push(root,
                  (struct payload){"random", rand() % 500, rand() % 500, 0, 0});
    }
}

int main(void)
{
    const int screen_width  = 800;
    const int screen_height = 450;

    struct node *root = node_create((struct payload){"foo", 50, 30, 0, 0});
    add_random_node(root, 10);
    add_random_node(root->data[0], 10);

    InitWindow(screen_width, screen_height, "graph");

    SetTargetFPS(60);
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        graph_update(root);
        graph_draw(root);

        DrawCircleV(middle_of_screen(), 2, BLACK);

        EndDrawing();
    }
    return EXIT_SUCCESS;
}
