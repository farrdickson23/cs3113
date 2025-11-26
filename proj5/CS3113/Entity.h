#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include <Math.h>

enum Direction    {PIDLE, LEFT, UP, RIGHT, DOWN, UPRIGHT, UPLEFT, DOWNLEFT, DOWNRIGHT, 
                   SRR, SRL, SRU, SRD, SGR, SGL, SGU, SGD, ARR, ARL, ARU, ARD}; // For walking and blaster bc fuck it we ball
enum EntityStatus { ACTIVE, INACTIVE                   };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, NONET, BLASTER, BLAST, BIRD};
enum AIType       { WANDERER, FOLLOWER, SNIPER                 };
enum AIState      { WALKING, IDLE, FOLLOWING, AIMING           };
//enum BlasterType  { SR, SG, AR };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    //BlasterType blasta = SR;
    Vector2 mScale;
    Vector2 mColliderDimensions;
    //Vector2 trajectory;
    
    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    Vector2 gMousePos;
    Vector2 startPos;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection;
    int mFrameSpeed = 4;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;
    
    

    bool aiming = false;
    bool mIsOpen = false;

    bool mIsJumping = false;
    float mJumpingPower = 0.0f;
    float aimTolerance = 1.2f;

    bool itIsDark = true;

    int mSpeed;
    
    float mAngle; // i should never change this unless like i die or something as the player
    float mAimAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    bool mWantToShoot = false;
    bool mCanShoot = true;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;

    AIType  mAIType;
    AIState mAIState = IDLE;

    

    void checkCollisionY(Entity **collidableEntities, int collisionCheckCount);
    void checkCollisionY(Map *map);

    void checkCollisionX(Entity **collidableEntities, int collisionCheckCount);
    void checkCollisionX(Map *map);
    
    void resetColliderFlags() 
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target);
    void AIWander();
    void AIFollow(Entity *target);
    void AISRFollow(Entity* player);

public:
    //bool interact;
    bool win = false;
    int health = 3;

    bool youSuck = false;
    float aimingTimer = 0.0f;
    bool mSecondBarrier = false;
    bool canDash = true;
    float counterTime = 0.0f;
    float dashTime = 0.5f; // at this point FUCK getters they take too much time. ples dont tell sterling

    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 4;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, 
        std::map<Direction, std::vector<int>> animationAtlas, 
        EntityType entityType);
    ~Entity();

    bool isColliding(Entity* other) const; // i moved it fight me

    void update(float deltaTime, Entity *player, Map *map, 
        Entity **collidableEntities, int collisionCheckCount);
    void render();
    void normaliseMovement() { Normalise(&mMovement); }
    void fixDirection(int wpn, Direction dir);

    void jump()       { mIsJumping = true;  }
    void open() { mIsOpen = true; mDirection = UP; }
    void activate()   { 
        mEntityStatus  = ACTIVE;   
        if (mEntityType == NPC)
            health = 5;
        if (mEntityType == BLAST) {
            if (mDirection == SGR || mDirection == SGD ||
                mDirection == SGU || mDirection == SGL) {
                health = 3;
            }
            else if (mDirection == SRR || mDirection == SRD ||
                mDirection == SRU || mDirection == SRL) {
                health = 6;
            }
            else if (mDirection == ARR || mDirection == ARD ||
                mDirection == ARU || mDirection == ARL) {
                health = 2;
            }
        }
    }
    void deactivate() { mEntityStatus  = INACTIVE; aimingTimer = 0.f; }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE ? true : false; }
    bool isOpen() { return mIsOpen; }

    void moveUp()    { 
        mMovement.y = -1;
        if (mAIType == BIRD)
            return;
        mDirection = UP; 
        mAimAngle = (90.f); 
    }
    void moveUpLeft();
    void moveUpRight();
    void moveDownLeft();
    void shot() { mWantToShoot = false; }
    void moveDownRight();
    void tryToShoot(Entity* blast[]);
    void moveDown()  { 
        mMovement.y =  1;
        if (mAIType == BIRD)
            return;
        mDirection = DOWN; 
        mAimAngle = (-90.f); //printf("aim angle: %f\n", mAimAngle);
    }
    void imReallyBadAtFunctionNames() { mWantToShoot = true; }
    void moveLeft()  { 
        mMovement.x = -1; 
        mDirection = LEFT; 
        mAimAngle = ( 180.f ); 

    }
    void moveRight() { 
        mMovement.x =  1; 
        mDirection = RIGHT; 
        mAimAngle = ( 0.f ); 
    }

    //void mouseInformation( Entity* player );
    //void blasterType(BlasterType type) { blasta = type; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mScale;                 }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    float getAimAngle() const { return mAimAngle; }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    EntityType  getEntityType()            const { return mEntityType;            }
    AIType      getAIType()                const { return mAIType;                }
    AIState     getAIState()               const { return mAIState;               }
    bool isAiming() const { return aiming; }
    //void pressedF() { interact = true; }

    bool isItDark() { return itIsDark; }

    bool canBeShot() { return mCanShoot; }
    bool wantToShoot() { return mWantToShoot; }

    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void switchLight() { itIsDark = !itIsDark; }
    void mouseInformation( Entity* player);
    //void setTrajectory(Vector2 traj) { trajectory = traj; }

    void shotCooldownOver() { mCanShoot = true; }
    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions) 
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions) 
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed  = newSpeed;                      }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }
    void setAngle(float newAngle) 
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void setDirection(Direction newDirection)
    { 
        mDirection = newDirection;

        if (mTextureType == ATLAS) mAnimationIndices = mAnimationAtlas.at(mDirection);

        if (mDirection == SGR || mDirection == SGD ||
            mDirection == SGU || mDirection == SGL) {
            mSpeed = 800;
        }
        else if (mDirection == SRR || mDirection == SRD ||
            mDirection == SRU || mDirection == SRL) {
            mSpeed = 1200;
        }
        else if (mDirection == ARR || mDirection == ARD ||
            mDirection == ARU || mDirection == ARL) {
            mSpeed = 800;
        }
    }
    void setMousePos(Vector2 info) { gMousePos = info; }
    void setAIState(AIState newState)
        { mAIState = newState;                     }
    void setAIType(AIType newType)
        { mAIType = newType;                       }
};

#endif // ENTITY_H