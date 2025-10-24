#include "Entity.h"

/*
Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mSpriteState {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mEntityType {NONE} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
    EntityType entityType) : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mSpriteState {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f}, mEntityType {entityType} { }

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<SpriteState, 
        std::vector<int>> animationAtlas, EntityType entityType) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mSpriteState {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED }, mEntityType {entityType} { }
*/

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mSpriteState {GOOD}, 
                   mFrameSpeed {0}, mEntityType {RIDGE} { }

Entity::Entity(Vector2 position, Vector2 scale, const char* textureFilepath, 
               EntityType entityType, Vector2 spriteSheetDimensions) : mPosition{ position }, mVelocity{ 0.0f, 0.0f },
               mAcceleration{ 0.0f, 0.0f }, mScale{ scale }, mMovement{ 0.0f, 0.0f },
               mColliderDimensions{ scale.x *0.7f , scale.y *0.7f }, mTexture{ LoadTexture(textureFilepath) },
               mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mAngle{ 0.0f }, 
               mEntityType{ entityType }, mSpriteSheetDimensions{ spriteSheetDimensions },
               mSpriteState {GOOD} { }

Entity::~Entity() { UnloadTexture(mTexture); };

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity, and resolves any vertical overlap by adjusting the 
 * player's position and velocity accordingly.
 * 
 * @param collidableEntities An array of pointers to `Entity` objects that 
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */
void Entity::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // STEP 1: For every entity that our player can collide with...
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity))
        {
            // STEP 2: Calculate the distance between its centre and our centre
            //         and use that to calculate the amount of overlap between
            //         both bodies.
            //float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
	    //printf("yDistance: %.2f\n", yDistance);
            //float yOverlap  = fabs(yDistance - fmaxf(0.0f ,(mColliderDimensions.y / 2.0f)) - fmaxf(0.0f,(collidableEntity->mColliderDimensions.y / 2.0f)));
            
	    //printf("yOverlap: %.2f\n", yOverlap);
            // STEP 3: "Unclip" ourselves from the other entity, and zero our
            //         vertical velocity.
            if (collidableEntity->mEntityType == GOAL) // hit the goal
            {
                if ((sqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y) < 22) && (mAngle < 10 || mAngle < -10)) {
                    mAcceleration.x = 0;
                    mAcceleration.y = 0;

                    // stop game 
                    //display win
                    isDone = true;
                }
                else {
                    mSpriteState = BAD;
                    // stop game
                    // display loss
                    isDone = true;
                }
            }

            else if (collidableEntity->mEntityType == RIDGE) // hit the wall
            {
                mSpriteState = BAD;
             // stop game
             // display loss
                isDone = true;
            }
            else if (collidableEntity->mEntityType == PLAYER) // hit the wall
            {
                collidableEntity->mSpriteState = BAD;
                // stop game
                // display loss
                isDone = true;
                collidableEntity->isDone = true;
            }

        
        }
    }
}

void Entity::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        

        if (isColliding(collidableEntity))
        {
            if (collidableEntity->mEntityType == GOAL) // hit the goal
            {
                //win condition 
                mAcceleration.x = 0;
                mAcceleration.y = 0;

                // stop game 
                //display win
                isDone = true;
                mWasJumping = false;
            }

            else if (collidableEntity->mEntityType == RIDGE) // hit the wall
            {
                mSpriteState = BAD;
                // stop game
                // display loss
                isDone = true;
                mWasJumping = false;
            }
            else if (collidableEntity->mEntityType == PLAYER) // hit the wall
            {
                collidableEntity->mSpriteState = BAD;
                // stop game
                // display loss
                isDone = true;
                mWasJumping = false;
                collidableEntity->isDone = true;
            }
        }
        
    }
}

/**
 * Checks if two entities are colliding based on their positions and collider 
 * dimensions.
 * 
 * @param other represents another Entity with which you want to check for 
 * collision. It is a pointer to the Entity class.
 * 
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool Entity::isColliding(Entity *other) const 
{
    //if (!other->isActive()) return false;
    if (other->mAngle == 90.f) {

        float xDistance = fabs(mPosition.x - other->getPosition().x) -
            ((mColliderDimensions.x + other->getColliderDimensions().y - 10.f) / 2.0f);
        float yDistance = fabs(mPosition.y - other->getPosition().y) -
            ((mColliderDimensions.y + other->getColliderDimensions().x) / 2.0f);

        if (xDistance < 0.0f && yDistance < 0.0f) return true;

        return false;
    }

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

/**
 * Updates the current frame index of an entity's animation based on the 
 * elapsed time and frame speed.
 * 
 * @param deltaTime represents the time elapsed since the last frame update.
 */
/* // i dont think i need this
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mSpriteState);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}
*/

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

void Entity::updateFire(Entity* alienShip) {
    mAngle = alienShip->mAngle;
    mPosition.x = alienShip->mPosition.x + 50.f * -sin((mAngle * PI / 180) ); // float is distance between the origins of the fire and ship
    mPosition.y = alienShip->mPosition.y + 50.f * cos(mAngle * PI / 180);
    return;
}


void Entity::update(float deltaTime, Entity *collidableEntities, 
    int collisionCheckCount, Entity* blocks, int blockCount)
{

    //if(mEntityStatus == INACTIVE) return;
    if (isDone) {
        return;
    }

    if (mEntityType == FIRE) {
        updateFire(collidableEntities);
        return;
    }

    if (mEntityType == RIDGE || mEntityType == GOAL) {
        return;
    }

    if (mEntityType == ASTEROID) {
        if (!isDone && !collidableEntities->isDone) {
            mAngle += 2;
            mPosition.x -= 2.f;
            if (mPosition.x <= 0.f)
                mPosition.x = 1920.f;
            checkCollisionY(collidableEntities, 1);
            checkCollisionX(collidableEntities, 1);
        }
        return;
    }

    resetColliderFlags();

    //mVelocity.x = mMovement.x * mSpeed;

    
    // ––––– JUMPING ––––– //
    if (mIsJumping && fuel > 0.025)
    {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;
        mWasJumping = true;
        
        // fix angle although this should probably be done elsewhere oh well
        // keeps angle from -180 to 180
        if (mAngle > 180)
            mAngle = mAngle - 360.f;
        else if (mAngle < -180.f)
            mAngle = mAngle + 360.f;
        
        //  change acceleration based on angle // if i thought longer about this i couldve done abs but oh well
        // i jsut remembered the cos and sin take radians not degrees so that was a lot of wasted time
        if (mAngle == 0.f) {
            mAcceleration.y -= mJumpingPower;
        } else if (mAngle > 0.f && mAngle < 90.f) {
            mAcceleration.x += mJumpingPower * sin(mAngle * PI / 180);
            mAcceleration.y -= mJumpingPower * cos(mAngle * PI / 180);
            if (mAcceleration.y >= 1500)
                mAcceleration.y -= 20.f;
        } else if (mAngle == 90.f) {
            mAcceleration.x += mJumpingPower;
            if (mAcceleration.y >= 1500)
                mAcceleration.y -= 20.f;
        } else if (mAngle < 0.f && mAngle >= -90.f) {
            mAcceleration.x += mJumpingPower * sin(mAngle * PI / 180);
            mAcceleration.y -= mJumpingPower * cos(mAngle * PI / 180);
            if (mAcceleration.y >= 1500)
                mAcceleration.y -= 20.f;
            
        } else if (mAngle >= -180.f && mAngle < -90.f) {
            mAcceleration.x += mJumpingPower * sin(mAngle * PI / 180);
            mAcceleration.y -= mJumpingPower * cos(mAngle * PI / 180);
        } else if (mAngle <= 180.f && mAngle > 90.f) {
            mAcceleration.x += mJumpingPower * sin(mAngle * PI / 180);
            mAcceleration.y -= mJumpingPower * cos(mAngle * PI / 180);
        }
        
        //printf("%.6f\n", mAngle);
        //if (mAcceleration.y > 1500 && (mAngle < 90.f || mAngle > -90.f)) { // jumping while accelerating down still
        //    //printf("%.6f\n", mAcceleration.y);
        //    mAcceleration.y -= 20;
        //}
        //printf("%.6f\n", mAngle);
        // use up fuel
    }
    else { // adjust acceleration if there is no jump
        //printf("no jump\n");
        mWasJumping = false;
        if (mAcceleration.x > 1.5f) {
            mAcceleration.x -= 1.5f;
        } else if ( mAcceleration.x < -1.5f) { // air resistance
            mAcceleration.x += 1.5f;
        }
        else if (mAcceleration.x < 1.5f || mAcceleration.x > -1.5f){ // acceleration is approx zero
            mAcceleration.x = 0;
        }
        if (mAcceleration.y < 1500) { // 900 being the planet gravity
            mAcceleration.y += 50.f; // every update you are not boosting you gain 30 negative acceleration
            if (mAcceleration.y > 1500) { // if it accidentally overcorects
                mAcceleration.y = 1500;
            }
        }
        else if (mAcceleration.y > 1500) {
            mAcceleration.y -= 20.f; // past terminal velocity
        }
    }
    //printf("L/R A: %.6f\n", mAcceleration.x);
    //printf("up/downA: %.6f\n", mAcceleration.y);
    
    mVelocity.x = mAcceleration.x * deltaTime;
    mVelocity.y = mAcceleration.y * deltaTime;
    //printf("L/R V: %.6f\n", mVelocity.x);
    //printf("up/downV: %.6f\n", mVelocity.y);

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(blocks, blockCount);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(blocks, blockCount);

    if (mPosition.y < -50.f || mPosition.x < -50.f || mPosition.x > 1970.f) {
        isDone = true;
        isGone = true;
        mSpriteState = BAD;
    }

    //if (mTextureType == ATLAS && GetLength(mMovement) != 0 && mIsCollidingBottom) 
    //    animate(deltaTime);
}

void Entity::render()
{
    //if(mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mEntityType)
    {
        case PLAYER:
            // Top Left
            if (mSpriteState == GOOD) {
                textureArea = {
                    // top-left corner
                    0.0f, 0.0f,

                    // bottom-right corner (of texture)
                    static_cast<float>(mTexture.width / 2),
                    static_cast<float>(mTexture.height / 2)
                };
            } else {
                textureArea = {
                    // top-left corner
                    190.f,
                    190.f,

                    // bottom-right corner (of texture)
                    168.f,
                    168.f
                };
            }
            break;
        case ASTEROID:
            // Top Left
            
                textureArea = {
                // top-left corner
                190.f,
                190.f,

                // bottom-right corner (of texture)
                168.f,
                168.f
            };
            break;
        case FIRE:
            textureArea = {
                static_cast<float>(mTexture.width / 2),
                0.f,
                static_cast<float>(mTexture.width ) / 2,
                static_cast<float>(mTexture.height / 2),
            };
            break;
        default: 
            //printf("test");
            textureArea = {
                60.f,
                200.f,
                61.f,
                201.f
            };
            break;
    }
    //printf("%f\n", static_cast<float>(mTexture.width));
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
    
    Color renderColor = WHITE;
    if (mEntityType == GOAL)
        renderColor = GREEN;
    DrawTexturePro(
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, renderColor
    );

    //displayCollider();
}