// entity.cpp
#include "entity.h"
#include "input.h"

#define MAX_ENTITIES 128
#define SPEED 200.0
#define GRAVITY 500.0
#define JUMP_VELOCITY -300.0

Entity entities[MAX_ENTITIES];
int entityCount = 0;

void AddEntity(EntityType type, double x, double y, COLORREF color){
    if(entityCount >= MAX_ENTITIES) return;
    entities[entityCount++]= {type, x, y, color};
}

void SpawnBullet(double x, double y) {
    AddEntity(ENTITY_BULLET, x, y, RGB(255, 255, 0));
    Entity& b = entities[entityCount - 1];
    b.vx = 400;
    b.vy = 0;
    b.w = 12;
    b.h = 4;
}

void UpdateEntities(double dt){
    for(int i = 0; i < entityCount; i++){
        Entity& e = entities[i];

        if(e.type == ENTITY_PLAYER){
            e.vx = 0;
            if(keyLeft)  e.vx = -SPEED;
            if(keyRight) e.vx = SPEED;
            e.vy += GRAVITY * dt;

            if(keySpace && e.onGround){
                PlaySound(TEXT("assets/sound/jump.wav"), NULL, SND_FILENAME | SND_ASYNC);
                e.vy = JUMP_VELOCITY;
                e.onGround = false;
            }

            e.x += e.vx * dt;
            e.y += e.vy * dt;

            double floorY = 500.0;
            if(e.y + e.h >= floorY){
                e.y = floorY - e.h;
                e.vy = 0;
                e.onGround = true;
            }
            else {
                e.onGround = false;
            }
        }
        else if(e.type == ENTITY_ENEMY){
            e.vy += GRAVITY * dt;
            e.y += e.vy * dt;

            double floorY = 500.0;
            if(e.y + e.h >= floorY){
                e.y = floorY - e.h;
                e.vy = 0;
                e.onGround = true;
            }
            else {
                e.onGround = false;
            }
        }
        else if(e.type == ENTITY_BULLET){
            e.x += e.vx * dt;
            e.y += e.vy * dt;

            for (int j = 0; j < entityCount; j++) {
                Entity& target = entities[j];

                if (target.type == ENTITY_ENEMY && CheckCollision(e, target)) {
                    PlaySound(TEXT("assets/sound/explosion.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    for (int k = j; k < entityCount - 1; k++)
                        entities[k] = entities[k + 1];
                    entityCount--;
                    if (j < i) i--;

                    for (int k = i; k < entityCount - 1; k++)
                        entities[k] = entities[k + 1];
                    entityCount--;
                    i--;
                    break;
                }
            }

            if(e.x > 800){
                for(int j = i; j < entityCount - 1; j++)
                    entities[j] = entities[j + 1];
                entityCount--;
                i--;
            }
        }
    }

    if(keyShoot){
        for(int i = 0; i < entityCount; i++){
            if(entities[i].type == ENTITY_PLAYER){
                SpawnBullet(entities[i].x + entities[i].w, entities[i].y + entities[i].h/2 - 2);
                PlaySound(TEXT("assets/sound/shoot.wav"), NULL, SND_FILENAME | SND_ASYNC);
                break;
            }
        }
        keyShoot = false;
    }
}

void RenderEntities(HDC hdc){
    for(int i = 0; i < entityCount; ++i){
        Entity& e = entities[i];
        HBRUSH brush = CreateSolidBrush(e.color);
        RECT r = { static_cast<int>(e.x), static_cast<int>(e.y), static_cast<int>(e.x + e.w), static_cast<int>(e.y + e.h) };
        FillRect(hdc, &r, brush);
        DeleteObject(brush);
    }
}

bool CheckCollision(const Entity& a, const Entity& b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
    );
}
