#include "Entity.h"
#include <stdlib.h>

Entity::Entity() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0},
                   mEntityType {NONE} { }

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

                if (collidableEntity->mEntityType == NPC) {
                    if (collidableEntity->mAIType == BOSS || collidableEntity->mAIType == BOUNCER) { // i was going to do something with this but oh well not anymore
                        //reset player position in level,
                        //lose one health
                        return;
                    }
                    else {
                        collidableEntity->AIDie();
                    }
                }
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
            // When standing on a platform, we're always slightly overlapping
            // it vertically due to gravity, which causes false horizontal
            // collision detections. So the solution I dound is only resolve X
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
    if (!other->isActive() || other == this) return false;

    float xDistance = fabs(mPosition.x - other->getPosition().x) - 
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) - 
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

/*
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        if (mCurrentFrameIndex == mAnimationIndices.size() && mAIState == PUNCH) {
            mAIState = NORM; // anything but punch SHOULD work
            printf("through\n");
        }
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}*/
void Entity::animate(float deltaTime)
{
    //mAnimationIndices = mAnimationAtlas.at(mDirection);
    auto it = mAnimationAtlas.find(mDirection); // I replaced the above with it because this was causing crashes when I had sprites without idle animations which I could fix
                                                // but this seems like a good guard to have anyway
    if (it == mAnimationAtlas.end() || it->second.empty()) {
        // no animation defined for this direction or no frames
        return;
    }

    mAnimationIndices = it->second; // since im using auto it will get the pair not the indexs so now im making sure were looking at the indicies

    // make sure index is in range
    if (mCurrentFrameIndex >= static_cast<int>(mAnimationIndices.size())) {
        mCurrentFrameIndex = 0;
    }

    mAnimationTime += deltaTime;

    if (mFrameSpeed <= 0.0f) return; // avoid divide-by-zero

    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;
        if (mDirection == DDEAD)
        {
            if (mCurrentFrameIndex + 1 < static_cast<int>(mAnimationIndices.size()))
            {
                mCurrentFrameIndex = mCurrentFrameIndex +1;  // advance
            }
            // else: already at last frame, stay there
            return;
        }
        mCurrentFrameIndex++;

        if (mCurrentFrameIndex == static_cast<int>(mAnimationIndices.size()) &&
            (mAIState == PUNCHL || mAIState == PUNCHR))
        {
            mAIState = NORM;
            //printf("through\n");
        }

        mCurrentFrameIndex %= static_cast<int>(mAnimationIndices.size());
    }
}


void Entity::AIWander() { moveLeft(); }

void Entity::AIBounce() {
    if (isCollidingBottom()) {
        jump();
        // float angle = rand() % 180 - 90; // i need to stop using trig for all of my projects its too confusing
        // mVelocity.x = cos(angle);
    }
    if (mIsCollidingLeft) // slime hits something on the left
        moveRight();
    else if (mPosition.x > 800.f) // slime checks right hit bruv
        moveLeft();
    else if (mMovement.x == 0.f) // when the level starts
        moveLeft();
}

void Entity::AIFly() { // i need to fix this

    
    float top = std::min(mFlyingMax, mFlyingMin);// min is actually the "higher" one i forgot y cordinate lower is higher on screen so top is min
    float bottom = std::max(mFlyingMax, mFlyingMin); // max is lower one so bottom is max

    if (top == bottom) return; // if there is no range 

  
    if (mPosition.y <= top) { // we are "less than" the top w cord flip this is higher than the top bound
        mFlyingDown = true; // start flying down
    }
    else if (mPosition.y >= bottom) { // greater than the bottom
        mFlyingDown = false; // start flying up
    }

    float flySpeed = static_cast<float>(mSpeed);
    mVelocity.y = mFlyingDown ? flySpeed : -flySpeed; // using these makes me feel so cool
}

void Entity::AIFollow(Entity *target)
{
    if (!target) return;// make sure there is an actual target to follow

    switch (mAIState)
    {
    case SIDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f) 
            mAIState = WALKING;
        break;

    case WALKING:
        // Depending on where the player is in respect to their x-position
        // Change direction of the enemy
        if (mPosition.x > target->getPosition().x) moveLeft();
        else                                       moveRight();
        break;
    
    //case FLYING:
     //   AIFly();
     //   break;

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
    
    case FLYING:
        AIFly();
        break;

    case BOUNCER:
        AIBounce();
        break;

    default:
        break;
    }
}

void Entity::update(float deltaTime, Entity *player, Map *map, 
    Entity *collidableEntities, int collisionCheckCount)
{
    if (mEntityStatus == INACTIVE) return;
    
    if (mEntityType == NPC) AIActivate(player);

    resetColliderFlags();

    mVelocity.x = mMovement.x * mSpeed;
    //mVelocity.y = mMovement.y * mSpeed;

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;
        
        // STEP 2: The player now acquires an upward velocity
        mVelocity.y -= mJumpingPower;
    }

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(map);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(map);

    /* // update so fliers are animated
    if (mTextureType == ATLAS && GetLength(mMovement) != 0 && mIsCollidingBottom) {
        if (mAIState == PUNCHL)
            mDirection = PLEFT;
        if (mAIState == PUNCHR)
            mDirection = PRIGHT;
        animate(deltaTime);
    }
    if (mTextureType == ATLAS && GetLength(mMovement) == 0 && mIsCollidingBottom) { // maybe and added if is PLAYER
        mDirection = IDLE;
        if (mAIState == PUNCHL)
            mDirection = PLEFT;
        if (mAIState == PUNCHR)
            mDirection = PRIGHT;
        animate(deltaTime);
        */

    if (mTextureType == ATLAS)
    {
        bool isPunching = (mAIState == PUNCHL || mAIState == PUNCHR); // this all just so i can only use punch for the boss oh well

        if (isPunching)
            animate(deltaTime);
        else if (mDirection == DDEAD)
            animate(deltaTime);
        else if (mEntityType == NPC && (mAIType == BOUNCER || mAIType == FLYING)) // to show slime and demon animations
            animate(deltaTime);
        // Everyone else (player, ground NPCs): keep old grounded behaviour
        else if (mIsCollidingBottom) {
            if (GetLength(mMovement) != 0) {
                animate(deltaTime);
            }
            else {
                mDirection = IDLE;
                animate(deltaTime);
            }
        }
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
	    // SAFETY CHECKS RECOMMENDED BY CHAT GPT
	    if (mAnimationIndices.empty()) {
                // Nothing to draw for this entity
                return;
            }
	    if (mCurrentFrameIndex < 0 ||
                mCurrentFrameIndex >= static_cast<int>(mAnimationIndices.size()))
            {
                // Wrap it instead of just forcing to 0, in case it’s large
                mCurrentFrameIndex %= static_cast<int>(mAnimationIndices.size());
                if (mCurrentFrameIndex < 0) {
                    mCurrentFrameIndex += static_cast<int>(mAnimationIndices.size());
                }
            }
	    // PROF SAID I COULD 
            textureArea = getUVRectangle(
                &mTexture, 
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );
	    break; // also suggested by gpt
        
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