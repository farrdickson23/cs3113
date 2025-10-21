#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"
#include <math.h>

//enum SpriteState    { sFIRE, GOOD, BAD , RIDGE }; // all different animations
//enum EntityStatus { ACTIVE, INACTIVE              }; //i dont think i need this
enum EntityType { PLAYER, GOAL, RIDGE, FIRE };
enum PlayerState {GOOD, BAD};

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;
    
    Texture2D mTexture;
    //TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    
    //std::map<SpriteState, std::vector<int>> mAnimationAtlas;
    //std::vector<int> mAnimationIndices;
    PlayerState mSpriteState;
    EntityType mEntityType;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    bool mIsJumping = false;
    bool isDone = false; // stop game condition
    float mJumpingPower = 0.0f;

    int mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    //EntityStatus mEntityStatus = ACTIVE;
    //EntityType   mEntityType;

    bool isColliding(Entity *other) const;
    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void resetColliderFlags() 
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    //void animate(float deltaTime);

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity(); // for now no 
    Entity(Vector2 position, Vector2 scale, const char* textureFilepath,
        EntityType entityType, Vector2 spriteSheetDimensions);
    /*
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, 
        std::map<SpriteState, std::vector<int>> animationAtlas, 
        EntityType entityType);
    */
    ~Entity();

    void update(float deltaTime, Entity *collidableEntities, int collisionCheckCount,
        Entity* blocks, int blockCount);
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void jump()       { mIsJumping = true;  }
    //void activate()   { mEntityStatus  = ACTIVE;   }
    //void deactivate() { mEntityStatus  = INACTIVE; }
    void displayCollider();

    //bool isActive() { return mEntityStatus == ACTIVE ? true : false; }

    //void moveUp()    { mMovement.y = -1; mSpriteState = UP;    }
    //void moveDown()  { mMovement.y =  1; mSpriteState = DOWN;  }
    void turnLeft()  { mAngle -= 1; }
    void turnRight() { mAngle += 1; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }
    bool     getDoneStat()              const { return isDone; }
    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mScale;                 }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    //TextureType getTextureType()           const { return mTextureType;           }
    PlayerState   getSpriteState()             const { return mSpriteState;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    
    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    //std::map<SpriteState, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

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
};

#endif // ENTITY_CPP