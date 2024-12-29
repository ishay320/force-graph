#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "msl/array.h"
#include "rayext.h"

struct payload {
    char *name;

    Vector2 force;
    Vector2 position;
    float mass;
};

struct connection {
    struct payload *a;
    struct payload *b;
};

Vector2 middle_of_screen()
{
    return (Vector2){GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
}

struct payload payload_create(char *name, Vector2 position, float size)
{
    float mass = (2 * PI * size) / 1.5;
    return (struct payload){
        .name     = name,
        .force    = {0, 0},
        .position = position,
        .mass     = mass,
    };
}

// gravity toward the middle of the screen
Vector2 force_to_middle(Vector2 position, float scale)
{
    Vector2 middle = middle_of_screen();
    float distance = Vector2Distance(position, middle);
    Vector2 force  = Vector2PullForce(middle, position, distance * scale);
    // TODO: damp the value to reduce oscillations
    return force;
}

// apply repulsive force between nodes
void force_repulse_points(const Vector2 pos_a, const Vector2 pos_b,
                          Vector2 *force_a, Vector2 *force_b, float distance)
{
    Vector2 dir = Vector2Subtract(pos_a, pos_b);
    // TODO: the length of dir can get close to 0 so the divide
    // close to infinity- cap it with some value
    Vector2 force =
        Vector2DivideVal(dir, Vector2Length(dir) * Vector2Length(dir));
    force = Vector2MultiplyVal(force, distance);

    *force_a = Vector2Add(*force_a, force);
    *force_b = Vector2Add(*force_b, Vector2Negate(force));
}

void update_nodes_pos(struct payload points[], size_t points_size,
                      struct connection eadges[], size_t eadges_size)
{
    {  // gravity toward the middle of the screen
        const float scale = 0.1;
        for (size_t i = 0; i < points_size; i++) {
            points[i].force = force_to_middle(points[i].position, scale);
            // TODO: damp the value to reduce oscillations
        }
    }

    // apply repulsive force between nodes
    float distance = 50;
    for (size_t i = 0; i < points_size; i++) {
        for (size_t j = 0; j < points_size; j++) {
            if (i == j) continue;
            force_repulse_points(points[j].position, points[i].position,
                                 &points[j].force, &points[i].force, distance);
        }
    }

    // apply forces between connected connections
    {
        for (size_t i = 0; i < eadges_size; i++) {
            Vector2 dis = Vector2MultiplyVal(
                Vector2Subtract(eadges[i].a->position, eadges[i].b->position),
                0.2);
            eadges[i].a->force = Vector2Subtract(eadges[i].a->force, dis);
            eadges[i].b->force = Vector2Add(eadges[i].b->force, dis);
        }
    }

    // update the position
    for (size_t i = 0; i < points_size; i++) {
        Vector2 velocity   = Vector2DivideVal(points[i].force, points[i].mass);
        points[i].position = Vector2Add(points[i].position, velocity);
    }
}

int main(void)
{
    srand(time(NULL));
    const int screen_width  = 1280;
    const int screen_height = 720;

    InitWindow(screen_width, screen_height, "graph");
    SetTargetFPS(60);

    // create fake graph
    size_t vertices_len = 200;
    struct payload *vertices;
    vertices = array_init_sized(struct payload, 10);

    for (size_t i = 0; i < vertices_len; i++) {
        array_push_rval(
            vertices,
            payload_create("r", (Vector2){rand() % 500, rand() % 500}, 1));
    }

    // create connections
    size_t edges_len = 200;
    struct connection *edges;
    edges = array_init_sized(struct connection, edges_len);
    for (size_t i = 0; i < edges_len; i++) {
        edges[i] = (struct connection){&vertices[rand() % array_len(vertices)],
                                       &vertices[rand() % array_len(vertices)]};
    }

    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        update_nodes_pos(vertices, array_len(vertices), edges, edges_len);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        // draw
        for (size_t i = 0; i < edges_len; i++) {
            DrawCircleV(vertices[i].position, 5, BLUE);
        }
        for (size_t i = 0; i < edges_len; i++) {
            DrawLineV(edges[i].a->position, edges[i].b->position, BLUE);
        }

        EndDrawing();
    }

    return 0;
}
