#include "Entity.h"


Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mEntityType {NONET} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType} { }

Entity::~Entity() { UnloadTexture(mTexture); };

void Entity::tryToShoot(Entity* blasts[]) {
    //mWantToShoot = true;
    int bulletInd = 0;
    for (int i = 0; i < 5; i++) {
        if (blasts[i]->isActive()) {
            continue;
        } else {
            bulletInd = i;
            break;
        }
    }
    if (mCanShoot && (mDirection == SRR || mDirection == SRD || 
                      mDirection == SRU || mDirection == SRL)) {
        blasts[bulletInd]->activate();
        float tempAngle = mAngle * PI / 180;
        blasts[bulletInd]->setMovement({ cos(tempAngle), sin(tempAngle) });
        blasts[bulletInd]->setPosition(mPosition);
        mCanShoot = false;
    }
    else if (mCanShoot && (mDirection == ARR || mDirection == ARD || 
                           mDirection == ARU || mDirection == ARL)) {
        blasts[bulletInd]->activate();
        float bloom = GetRandomValue(-70, 70) / 10.f;
        float tempAngle = (mAngle + bloom) * PI / 180;
        blasts[bulletInd]->setMovement({ cos(tempAngle), sin(tempAngle) });
        blasts[bulletInd]->setPosition(mPosition);
        mCanShoot = false;
    } else if (mCanShoot && (mDirection == SGR || mDirection == SGD || 
                             mDirection == SGU || mDirection == SGL)) {
        for (int j = 0; j < 5; j++) {
            blasts[j]->activate();
            float bloom = GetRandomValue(-200, 200) / 10.f;
            float tempAngle = (mAngle + bloom) * PI / 180;
            blasts[j]->setMovement({ cos(tempAngle), sin(tempAngle) });
            blasts[j]->setPosition(mPosition);
            mCanShoot = false;
        }
    }
}

void Entity::checkCollisionY(Entity **collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity *collidableEntity = collidableEntities[i];
        if (!collidableEntity) continue;
        
        if (isColliding(collidableEntity))
        {
            if ((collidableEntity->getEntityType() == NPC) && (mEntityType == BLAST)) {
                collidableEntity->health -= health;
                if (collidableEntity->health <= 0)
                    collidableEntity->deactivate();
                deactivate();
                return;
            }
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - 
                              (collidableEntity->mColliderDimensions.y / 2.0f));
            
            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (mVelocity.y > 0) 
            {
                mPosition.y -= yOverlap;
                mVelocity.y  = 0;
                mIsCollidingBottom = true;
            } else if (mVelocity.y < 0) 
            {
                mPosition.y += yOverlap;
                mVelocity.y  = 0;
                mIsCollidingTop = true;

                //if (collidableEntity->mEntityType == BLOCK)
                //    collidableEntity->deactivate();
            }
        }
    }
}

void Entity::checkCollisionX(Entity **collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = collidableEntities[i];
        if (!collidableEntity) continue;
        
        if (isColliding(collidableEntity))
        {            
            if ((collidableEntity->getEntityType() == NPC) && (mEntityType == BLAST)) {
                collidableEntity->health -= health;
                if (collidableEntity->health <= 0)
                    collidableEntity->deactivate();
                deactivate();
                return;
            }
            // When standing on a platform, we're always slightly overlapping
            // it vertically due to gravity, which causes false horizontal
            // collision detections. So the solution I found is only resolve X
            // collisions if there's significant Y overlap, preventing the 
            // platform we're standing on from acting like a wall.
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(yDistance - (mColliderDimensions.y / 2.0f) - (collidableEntity->mColliderDimensions.y / 2.0f));

            // Skip if barely touching vertically (standing on platform)
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(xDistance - (mColliderDimensions.x / 2.0f) - (collidableEntity->mColliderDimensions.x / 2.0f));

            if (mVelocity.x > 0) {
                mPosition.x     -= xOverlap;
                mVelocity.x      = 0;

                // Collision!
                mIsCollidingRight = true;
            } else if (mVelocity.x < 0) {
                mPosition.x    += xOverlap;
                mVelocity.x     = 0;
 
                // Collision!
                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe    = { mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe      = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe     = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) && mVelocity.y < 0.0f)
    {
        mPosition.y += yOverlap;   // push down
        mVelocity.y  = 0.0f;
        mIsCollidingTop = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) && mVelocity.y > 0.0f)
    {
        mPosition.y -= yOverlap;   // push up
        mVelocity.y  = 0.0f;
        mIsCollidingBottom = true;
    } 
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y };

    Vector2 rightCentreProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x -= xOverlap * 1.01f;   // push left
        mVelocity.x  = 0.0f;
        mIsCollidingRight = true;
    }

    // COLLISION ON LEFT (moving left)
    if (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x += xOverlap * 1.01;   // push right
        mVelocity.x  = 0.0f;
        mIsCollidingLeft = true;
    }
}

bool Entity::isColliding(Entity *other) const 
{
    if (!other->isActive() || other == this || !other) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Entity::animate(float deltaTime)
{
    if (mDirection == PIDLE) // when i get idle animation ill take this out
        mDirection = DOWN;
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Entity::AIWander() { moveLeft(); }

void Entity::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 1300.0f) 
            mAIState = WALKING;
        break;

    case WALKING:
        // Depending on where the player is in respect to their x-position
        // Change direction of the enemy
        if (mPosition.x > target->getPosition().x + 20.f)
            moveLeft();
        else if (mPosition.x < target->getPosition().x - 20.f)
            moveRight();
        else
            mMovement.x = 0.f;
        if (mPosition.y > target->getPosition().y + 20.f)
            moveUp();
        else if (mPosition.y < target->getPosition().y - 20.f)
            moveDown();
        else
            mMovement.y = 0.f;
        break;
    
    default:
        break;
    }
}

void Entity::AISRFollow(Entity* player) { 
    if (!isActive())
        return;
    /*      // whoever wrote this code was fucking delusional (me)
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, player->getPosition()) < 2500.0f)
            mAIState = WALKING;
        break;

    case WALKING:
        // Depending on where the player is in respect to their x-position
        // Change direction of the enemy
        //aiming = false;
        if (Vector2Distance(mPosition, player->getPosition()) > 2500.0f)
            mAIState = IDLE;
        if ((mPosition.x > player->getPosition().x + 1600) && ((mPosition.y > player->getPosition().y + 600))) { // they will strafe if you are above or below them i dont want to write that case fight me
            moveDownLeft();
        }
        else if ((mPosition.x > player->getPosition().x + 1600) && (mPosition.y < player->getPosition().y + 600)) {
            moveUpLeft();
        }
        else if (mPosition.x > player->getPosition().x + 1600) {
            moveLeft();
        }
        else if ((mPosition.x < player->getPosition().x + 1600) && (mPosition.y < player->getPosition().y + 600))
            moveUpRight();
        else if ((mPosition.x < player->getPosition().x + 1600) && (mPosition.y > player->getPosition().y + 600))
            moveDownRight();
        else if (mPosition.x > player->getPosition().x + 1600)
            moveRight();
        else mAIState = AIMING;
        break;


    case AIMING:
        aiming = true; // is this all i need ? no im dumb
        if (Vector2Distance(mPosition, player->getPosition()) > 1800.f) {
            aiming = false;
            aimingTimer = 0.f;
            mAIState = WALKING;
        }
        break;

    default:
        break;
        */
    Vector2 playerPos = player->getPosition();
    float dx = mPosition.x - playerPos.x; // change fo x
    float dy = mPosition.y - playerPos.y; // change of y
    float dist = Vector2Distance(mPosition, playerPos);

    float approxX = 1600.0f;  
    const float approxY = 600.0f;   

    const float snpRange = 2000.f;  // farther snp wont follow
    float aimingDist = 1700.f;

    switch (mAIState)
    {
    case IDLE:
        if (abs(dist) < snpRange)
            mAIState = WALKING;
        break;

    case WALKING:
        if (abs(dist) > snpRange) {
            mAIState = IDLE;
            mMovement = { 0.0f, 0.0f }; // stands idle
            break;
        }

        if (fabsf(dx) > approxX  + 200.f) { // enemy will try to reposition if too far from you
            aiming = false;
            if (dx > 0) moveLeft();   // enemy is to the right of player
            else        moveRight();  // enemy is to the left of player
        }
        else if (fabsf(dx) < approxX - 300.f) { // too close
            aiming = false;
            if (dx > 0) moveRight();
            else        moveLeft();
        }
        // 2) Horizontal distance okay: adjust vertical (strafe)
        else if (fabsf(dy) > approxY) {
            if (dy > 0) {
                // enemy is below player
                (dx > 0) ? moveUpLeft() : moveUpRight();
            }
            else {
                // enemy is above player
                (dx > 0) ? moveDownLeft() : moveDownRight();
            }
        }
        else {
            // In a good "ring" around player -> start aiming
            mAIState = AIMING;
            mMovement = { 0.0f, 0.0f };
        }
        break;

    case AIMING:
        aiming = true;
        mMovement = { 0.0f, 0.0f };   // stay still while aiming

        if (dist > 1800.0f) {
            aiming = false;
            aimingTimer = 0.0f;
            mAIState = WALKING;
        }
        break;

    default:
        break;
    }
    
}

void Entity::AIActivate(Entity *target)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;

    case SNIPER:
        AISRFollow(target);

    default:
        break;
    }
}
void Entity::moveUpLeft() {
    mMovement.y = -0.7071f;
    mMovement.x = -0.7071f; 
    mDirection = UPLEFT;
    mAimAngle = (135.f);
}
void Entity::moveUpRight() { 
    mMovement.y = -0.7071f;
    mMovement.x = 0.7071f;
    mDirection = UPRIGHT; 
    mAimAngle = (45.f);
}
void Entity::moveDownLeft() {
    mMovement.y = 0.7071f;
    mMovement.x = -0.7071f;
    mDirection = DOWNLEFT;
    mAimAngle = (-135.f);
}
void Entity::moveDownRight() {
    mMovement.y = 0.7071f;
    mMovement.x = 0.7071f;
    mDirection = DOWNRIGHT;
    mAimAngle = (-45.f);
}

void Entity::mouseInformation( Entity* player) {
    Vector2 playerPos = player->getPosition();
    /*
    printf("player x: %f \n", playerPos.x);
    printf("mouse x: %f \n", gMousePos.x);
    printf("player y: %f \n", playerPos.x);
    printf("mouse y: %f \n", gMousePos.y);
    printf("angle in degrees: %f \n", atan2((gMousePos.y - playerPos.y), (gMousePos.x - playerPos.x)) * 180 / PI);
    if (playerPos.x < gMousePos.x) {
        mAimAngle = atan2((gMousePos.y - playerPos.y),( gMousePos.x - playerPos.x)) *180 / PI;
        //printf("opp over adj: %f\n", (gMousePos.y - playerPos.y) / (gMousePos.x - playerPos.x));
    } else if (playerPos.x > gMousePos.x) {
        mAimAngle = atan2((playerPos.y - gMousePos.y), (playerPos.x - gMousePos.x)) * 180 / PI;
    } else if (playerPos.y < gMousePos.y) { // theyre equal
        mAimAngle = -90.f;
    } else {
        mAimAngle = 90.f;
    }
    */
    //Vector2 mousePos = 
    
    mAimAngle = - atan2((gMousePos.y - 300), (gMousePos.x - 500)) * 180 / PI;
    //printf("Angle: %f\n", mAimAngle);
}

void Entity::fixDirection(int wpn, Direction dir) { // maybe i just suck but i couldnt think of a better way to do this 
    if (wpn == 1) {
        if (dir == RIGHT || dir == UPRIGHT || dir == DOWNRIGHT)
            mDirection = SRR;
        else if (dir == LEFT || dir == UPLEFT || dir == DOWNLEFT)
            mDirection = SRL;
        else if (dir == UP)
            mDirection = SRU;
        else // down
            mDirection = SRD;
    }
    else if (wpn == 2) { 
        if (dir == RIGHT || dir == UPRIGHT || dir == DOWNRIGHT)
            mDirection = SGR;
        else if (dir == LEFT || dir == UPLEFT || dir == DOWNLEFT)
            mDirection = SGL;
        else if (dir == UP)
            mDirection = SGU;
        else // down
            mDirection = SGD;
    }
    else { // wpn == 3
        if (dir == RIGHT || dir == UPRIGHT || dir == DOWNRIGHT)
            mDirection = ARR;
        else if (dir == LEFT || dir == UPLEFT || dir == DOWNLEFT)
            mDirection = ARL;
        else if (dir == UP)
            mDirection = ARU;
        else // down
            mDirection = ARD;
    }
    if (mTextureType == ATLAS && !mAnimationAtlas.empty()) {
        mAnimationIndices = mAnimationAtlas.at(mDirection);
        mCurrentFrameIndex = 0;
    }
}

void Entity::update(float deltaTime, Entity *player, Map *map, 
    Entity **collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == BIRD) AIActivate(player);

    if (mEntityType == BLASTER) {
        mouseInformation(player);
        mPosition = player->getPosition();
        /*
        if (player->getDirection() == LEFT || player->getDirection() == UPLEFT || player->getDirection() == DOWNLEFT || 
            player->getDirection() == RIGHT || player->getDirection() == UPRIGHT || player->getDirection() == DOWNRIGHT) {
            // i will set the render order and blaster texture to use based on direction
            if (((player->getAimAngle() + 22.5f) * aimTolerance > (mAimAngle)) && ((player->getAimAngle() - 22.5f) / aimTolerance < (mAimAngle)))
                mAngle = mAimAngle ;
            else
                mAngle = player->getAimAngle() ;
        } else {// down or up
            if (player->getDirection() == UP) {
                // set render order and blaster texture
                mAngle = 0; // placeholder
            } else { // down
                //set render and text
                mAngle = 0;
            }
            if (((player->getAimAngle() + PI / 4) / aimTolerance > (mAimAngle)) && ((player->getAimAngle() - PI / 4) * aimTolerance < (mAimAngle)))
                mAngle = mAimAngle * 180 / PI;
            else
                mAngle = player->getAimAngle() * 180 / PI;
        }
        */
        //printf("aim angle: %f\n", player->getAimAngle());
        
        // add a case for left
        if (player->getDirection() == LEFT) {
            if ((mAimAngle > 180.f - 22.5f * aimTolerance) || (mAimAngle < -180.f + 22.5f * aimTolerance))
                mAngle = -mAimAngle;
            else if (mAimAngle <= 180.f - 22.5f * aimTolerance)
                mAngle = 180.f - 22.5f * aimTolerance;
            else
                mAngle = - 180.f + 22.5f * aimTolerance;
            return;
        } 

        if (((player->getAimAngle() + 22.5f * aimTolerance) > (mAimAngle)) && ((player->getAimAngle() - 22.5f * aimTolerance) < (mAimAngle))) {
            mAngle = -mAimAngle;
            //if
        } else
            if ((player->getAimAngle() + 22.5f * aimTolerance) > (mAimAngle))
                mAngle = -(player->getAimAngle() - 22.5f * aimTolerance);
            else 
                mAngle = -(player->getAimAngle() + 22.5f * aimTolerance);
        return;
    }
    
    if (mEntityType == NPC) AIActivate(player);

    resetColliderFlags();
    
    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // ––––– JUMPING ––––– //

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(map);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(map);

    
        

    if (mTextureType == ATLAS && (GetLength(mMovement) != 0 || mAIType == BIRD)) {
        animate(deltaTime);
    }
}

void Entity::render()
{
    if(mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
        case SINGLE:
            // Whole texture (UV coordinates)
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner (of texture)
                static_cast<float>(mTexture.width),
                static_cast<float>(mTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle(
                &mTexture, 
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );
        
        default: break;
    }

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // displayCollider();
}

void Entity::displayCollider() 
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}