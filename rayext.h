#ifndef RAYEXT_H
#define RAYEXT_H
#include <stdio.h>

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
Vector2 Vector2PullForce(const Vector2 puller, const Vector2 pullee,
                         float force_mult)
{
    Vector2 direction           = Vector2Subtract(puller, pullee);
    Vector2 direction_normalize = Vector2Normalize(direction);
    Vector2 force               = Vector2Scale(direction_normalize, force_mult);

    return force;
}

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

void Vector2Print(const Vector2 v) { printf("(%f,%f)\n", v.x, v.y); }

#endif  // RAYEXT_H
