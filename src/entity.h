#pragma once
#include <windows.h>

enum EntityType {
    ENTITY_PLAYER,
    ENTITY_ENEMY,
    ENTITY_BULLET
};

struct Entity {
    EntityType type;
    double x, y;
    double vx = 0, vy = 0;
    int w = 50;
    int h = 50;
    bool onGround = false;
    COLORREF color;

    Entity(){};
    Entity(EntityType type, double x, double y, COLORREF color)
        : type(type), x(x), y(y), color(color) {}
};

extern Entity entities[];
extern int entityCount;

void AddEntity(EntityType type, double x, double y, COLORREF color);
void SpawnBullet(double x, double y);
void UpdateEntities(double dt);
void RenderEntities(HDC hdc);
bool CheckCollision(const Entity& a, const Entity& b);