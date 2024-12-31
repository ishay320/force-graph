#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "msl/graph.h"
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

void update_nodes_pos(struct graph *g)
{
    {  // gravity toward the middle of the screen
        const float scale = 0.1;
        for (size_t i = 0; i < graph_nodes_count(g); i++) {
            ((struct payload *)g->nodes[i].data)->force = force_to_middle(
                ((struct payload *)g->nodes[i].data)->position, scale);
            // TODO: damp the value to reduce oscillations
        }
    }

    // apply repulsive force between nodes
    float distance = 50;
    for (size_t i = 0; i < graph_nodes_count(g); i++) {
        for (size_t j = 0; j < graph_nodes_count(g); j++) {
            if (i == j) continue;
            struct payload *point_i = g->nodes[i].data;
            struct payload *point_j = g->nodes[j].data;
            force_repulse_points(point_j->position, point_i->position,
                                 &point_j->force, &point_i->force, distance);
        }
    }

    // apply forces between connected connections
    {
        for (size_t i = 0; i < graph_edges_count(g); i++) {
            struct payload *edge_a =
                graph_node_get(g, g->edges[i].id_node_from)->data;
            struct payload *edge_b =
                graph_node_get(g, g->edges[i].id_node_to)->data;

            Vector2 dis = Vector2MultiplyVal(
                Vector2Subtract(edge_a->position, edge_b->position), 0.2);
            edge_a->force = Vector2Subtract(edge_a->force, dis);
            edge_b->force = Vector2Add(edge_b->force, dis);
        }
    }

    // update the position
    for (size_t i = 0; i < graph_nodes_count(g); i++) {
        struct payload *point_i = g->nodes[i].data;
        Vector2 velocity  = Vector2DivideVal(point_i->force, point_i->mass);
        point_i->position = Vector2Add(point_i->position, velocity);
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
    struct graph *graph = graph_init();

    for (size_t i = 0; i < vertices_len; i++) {
        struct payload *p = malloc(sizeof(*p));
        *p = payload_create("r", (Vector2){rand() % 500, rand() % 500}, 1);
        graph_node_push(graph, p);
    }

    // create connections
    size_t edges_len = 200;
    for (size_t i = 0; i < edges_len; i++) {
        graph_eadge_create(graph, rand() % graph_nodes_count(graph),
                           rand() % graph_nodes_count(graph));
    }

    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        update_nodes_pos(graph);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        // draw
        for (size_t i = 0; i < graph_nodes_count(graph); i++) {
            struct payload *vert_i = graph->nodes[i].data;
            DrawCircleV(vert_i->position, 5, BLUE);
        }
        for (size_t i = 0; i < graph_edges_count(graph); i++) {
            struct payload *edge_a =
                graph_node_get(graph, graph->edges[i].id_node_from)->data;
            struct payload *edge_b =
                graph_node_get(graph, graph->edges[i].id_node_to)->data;

            DrawLineV(edge_a->position, edge_b->position, BLUE);
        }

        EndDrawing();
    }

    return 0;
}
