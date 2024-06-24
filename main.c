#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void Vector2Print(const Vector2 v) { printf("(%f,%f)\n", v.x, v.y); }

Vector2 Vector2DivideVal(Vector2 v, float s)
{
    Vector2 result = {v.x / s, v.y / s};

    return result;
}

Vector2 Vector2MultiplyVal(Vector2 v, float s)
{
    Vector2 result = {v.x * s, v.y * s};

    return result;
}

int main(void)
{
    srand(time(NULL));
    const int screen_width  = 1280;
    const int screen_height = 720;

    struct node *root = node_create((struct payload){"foo", {50, 30}, {0, 0}});
    add_random_node(root, 10);
    add_random_node(root->data[0], 10);

    InitWindow(screen_width, screen_height, "graph");

    size_t p_len = 200;
    struct payload p[p_len];
    for (size_t i = 0; i < p_len; i++) {
        p[i] = (struct payload){"r", {rand() % 500, rand() % 500}, {0, 0}};
    }

    struct connection {
        struct payload *a;
        struct payload *b;
    };

    size_t connection_len = 200;
    struct connection c_list[connection_len];
    for (size_t i = 0; i < p_len; i++) {
        c_list[i] = (struct connection){&p[rand() % p_len], &p[rand() % p_len]};
    }

    SetTargetFPS(60);
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        {  // gravity toward the middle of the screen
            const float scale = 0.1;
            Vector2 middle    = middle_of_screen();
            for (size_t i = 0; i < p_len; i++) {
                float distance = Vector2Distance(p[i].pos, middle);
                Vector2 vel =
                    Vector2PullVel(middle, p[i].pos, distance * scale);
                p[i].vel = vel;
            }
        }

        // apply repulsive force between nodes
        for (size_t i = 0; i < p_len; i++) {
            for (size_t j = 0; j < p_len; j++) {
                if (i == j) continue;

                Vector2 dir   = Vector2Subtract(p[j].pos, p[i].pos);
                Vector2 force = Vector2DivideVal(
                    dir, Vector2Length(dir) * Vector2Length(dir));
                force = Vector2MultiplyVal(force, 100);

                p[j].vel = Vector2Add(p[j].vel, force);
                p[i].vel = Vector2Add(p[i].vel, Vector2Negate(force));
            }
        }

        // apply forces between connected connections
        {
            for (size_t i = 0; i < connection_len; i++) {
                Vector2 dis = Vector2MultiplyVal(
                    Vector2Subtract(c_list[i].a->pos, c_list[i].b->pos), 0.2);
                c_list[i].a->vel = Vector2Subtract(c_list[i].a->vel, dis);
                c_list[i].b->vel = Vector2Add(c_list[i].b->vel, dis);
            }
        }

        // update the position
        for (size_t i = 0; i < p_len; i++) {
            p[i].pos = Vector2Add(p[i].pos, p[i].vel);
        }

        // draw
        for (size_t i = 0; i < p_len; i++) {
            DrawCircleV(p[i].pos, 5, BLUE);
        }
        for (size_t i = 0; i < connection_len; i++) {
            DrawLineV(c_list[i].a->pos, c_list[i].b->pos, BLUE);
        }

        // graph_update(root);
        // graph_push(root);
        // graph_draw(root);
        EndDrawing();
    }
    return EXIT_SUCCESS;
}
