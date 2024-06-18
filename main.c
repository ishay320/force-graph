#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "da.h"
#include "raylib.h"
#include "raymath.h"

/**
 * @description 2D pull 1 point to another.
 *
 * @param puller the stationary one, the one who pules
 * @param pullee the one in motion, the one who gets pull
 * @param force pull force
 * @return the velocity of the pull that needs to apply
 */
Vector2 Vector2PullVel(const Vector2 puller, const Vector2 pullee, float force)
{
    Vector2 direction           = Vector2Subtract(puller, pullee);
    Vector2 direction_normalize = Vector2Normalize(direction);
    Vector2 vel                 = Vector2Scale(direction_normalize, force);

    return vel;
}

struct payload {
    char *name;

    Vector2 pos;
    Vector2 vel;
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
}

void destroy_graph(struct node *node)
{
    (void)node;
    assert(false && "not implemented yet");
}

void graph_draw(struct node *root)
{
    DrawCircleV(root->payload.pos, 5, RED);
    for (size_t i = 0; i < root->len; i++) {
        struct node *child = root->data[i];
        DrawLineV(root->payload.pos, child->payload.pos, BLUE);
        graph_draw(child);
    }
}

Vector2 middle_of_screen()
{
    return (Vector2){GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
}

void graph_update(struct node *root)
{
    const float scale         = 0.01;
    const float keep_distance = 60;
    const float decay         = 0.9;

    for (size_t i = 0; i < root->len; i++) {
        struct payload *a = &root->payload;
        struct payload *b = &root->data[i]->payload;

        const float force = Vector2Distance(b->pos, a->pos) - keep_distance;
        Vector2 a_vel     = Vector2PullVel(b->pos, a->pos, force * scale);
        Vector2 b_vel     = Vector2PullVel(a->pos, b->pos, force * scale);

        // also multiply decay
        a->vel = Vector2Scale(Vector2Add(a->vel, a_vel), decay);
        b->vel = Vector2Scale(Vector2Add(a->vel, b_vel), decay);

        graph_update(root->data[i]);
    }

    // update position of the node
    root->payload.pos = Vector2Add(root->payload.pos, root->payload.vel);
}

void add_random_node(struct node *root, size_t count)
{
    static size_t num = 0;

    for (size_t i = 0; i < count; i++) {
        char *node_name = malloc(15);
        sprintf(node_name, "rand - %ld", num++);
        node_push(root, (struct payload){
                            node_name, {rand() % 500, rand() % 500}, {0, 0}});
    }
}

struct node_list {
    struct node **data;
    size_t len;
    size_t cap;
};

void graph_to_list(struct node *root, struct node_list *node_list)
{
    da_push(*node_list, root);
    for (size_t i = 0; i < root->len; i++) {
        graph_to_list(root->data[i], node_list);
    }
}

void graph_push(struct node *root)
{
    // TODO: in progress
    const float scale = 1;
    const float decay = 0.4;

    struct node_list node_list = {0};
    graph_to_list(root, &node_list);

    for (size_t i = 0; i < node_list.len; i++) {
        for (size_t j = 0; j < node_list.len; j++) {
            if (i == j) {
                continue;
            }

            struct payload *a = &node_list.data[j]->payload;
            struct payload *b = &node_list.data[i]->payload;

            const float force =
                25 / (Vector2Distance(b->pos, a->pos) * 0.4 + 2);
            Vector2 a_vel = Vector2PullVel(b->pos, a->pos, force * scale);
            Vector2 b_vel = Vector2PullVel(a->pos, b->pos, force * scale);

            // also multiply decay
            a->vel = Vector2Scale(Vector2Subtract(a->vel, a_vel), decay);
            b->vel = Vector2Scale(Vector2Subtract(b->vel, b_vel), decay);
        }
    }
    for (size_t i = 0; i < node_list.len; i++) {
        struct node *node = node_list.data[i];
        node->payload.pos = Vector2Add(node->payload.pos, node->payload.vel);
    }
}

int main(void)
{
    const int screen_width  = 800;
    const int screen_height = 450;

    struct node *root = node_create((struct payload){"foo", {50, 30}, {0, 0}});
    add_random_node(root, 10);
    add_random_node(root->data[0], 10);

    InitWindow(screen_width, screen_height, "graph");

    SetTargetFPS(60);
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        {  // pull root to the middle of the screen
            const float scale = 0.1;
            Vector2 middle    = middle_of_screen();
            const float force = Vector2Distance(middle, root->payload.pos);
            Vector2 a_vel =
                Vector2PullVel(middle, root->payload.pos, force * scale);
            root->payload.vel = a_vel;
        }

        graph_update(root);
        graph_push(root);
        graph_draw(root);

        EndDrawing();
    }
    return EXIT_SUCCESS;
}
