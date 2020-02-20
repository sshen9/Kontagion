#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//ACTOR CLASS
Actor::Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* ptr)
:GraphObject(imageID, startX, startY, startDirection, depth)
{
    m_dead = false;
    m_worldPtr = ptr;
}

Actor::~Actor(){
    ;
}

bool Actor::isDamagable(){
    return true;
}

void Actor::doSomething(){
    return;
}


bool Actor::isDead() const{
    return m_dead;
}

void Actor::setDead(){
    m_dead = true;
}

bool Actor::hasHP(){
    return false;
}

bool Actor::isFood(){
    return false;
}

bool Actor::isDirt(){
    return false;
}

void Actor::decHP(int dmg){
    return;
}

StudentWorld* Actor::getWorld() const{
    return m_worldPtr;
}

//DIRT
//

Dirt::Dirt(int startX, int startY, StudentWorld* ptr)
:Actor(IID_DIRT, startX, startY, 0, 1, ptr)
{
    
}

bool Dirt::isDirt(){
    return true;
}

Dirt::~Dirt(){
    ;
}

Food::Food(int startX, int startY, StudentWorld* ptr)
:Actor(IID_FOOD, startX, startY, 90, 1, ptr)
{
}

bool Food::isDamagable(){
    return false;
}

bool Food::isFood(){
    return true;
}

LivingActor::LivingActor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* ptr, int hp)
:Actor(imageID, startX, startY, startDirection, depth, ptr)
{
    m_HP = hp;
}

bool LivingActor::hasHP(){
    return true;
}

int LivingActor::getHP(){
    return m_HP;
}

void LivingActor::setHP(int num){
    m_HP = num;
}

void LivingActor::checkDead(){
    if(m_HP<=0){
        setDead();
    }
}

void LivingActor::decHP(int dmg){
    m_HP-=dmg;
}

Socrates::Socrates(StudentWorld* ptr)
:LivingActor(IID_PLAYER, 0, 128, 0, 0, ptr, 100)
{
    m_sprayCount = 20;
    m_flameCount = 5;
}

void Socrates::doSomething(){
    checkDead();
    if(isDead()){
        return;
    }
    int ch;
    double PI = 3.14159265;
    if(getWorld()->getKey(ch)){
        switch (ch) {
            case KEY_PRESS_LEFT:
                //move clockwise
                setDirection(getDirection()+5);
                moveTo(128*cos((getDirection()+180)*1.0 / 360 * 2 * PI)+128, 128*sin((getDirection()+180)*1.0 / 360 * 2 * PI)+128);
                break;
            case KEY_PRESS_RIGHT:
                //MOVE COUNTERCLOCKWISE
                setDirection(getDirection()-5);
                moveTo(128*cos((getDirection()+180)*1.0 / 360 * 2 * PI)+128, 128*sin((getDirection()+180)*1.0 / 360 * 2 * PI)+128);
                break;
            case KEY_PRESS_ENTER:
                //flamethrower
                if(m_flameCount>0){
                    //add 16 flame objects
                    getWorld()->addFlames(getX(), getY(), getDirection());
                    m_flameCount-=1;
                    //playsound
                }
                break;
            case KEY_PRESS_SPACE:
                if(m_sprayCount>0){
                    getWorld()->addSpray(getX(), getY(), getDirection());
                    m_sprayCount-=1;
                    //playsound
                }
                break;
            default:
                break;
        }
    }else if(m_sprayCount<20){
        m_sprayCount+=1;
    }
    return;
}

void Socrates::addFlame(){
    m_flameCount+=5;
}

Socrates::~Socrates(){
    ;
}

Projectile::Projectile(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* ptr, int travelDistance, int damage)
:Actor(imageID, startX, startY, startDirection, depth, ptr)
{
    m_damage = damage;
    m_maxTravel = travelDistance;
    m_distTraveled = 0;
}

bool Projectile::isDamagable(){
    return false;
}

int Projectile::getDamage() const{
    return m_damage;
}

int Projectile::getDist() const{
    return m_distTraveled;
}

int Projectile::getMaxTravel() const{
    return m_maxTravel;
}

void Projectile::increaseDist(int dist){
    m_distTraveled+=dist;
}

void Projectile::doSomething(){
    if(isDead()){
        return;
    }
    double x = SPRITE_RADIUS*2*cos(getDirection()*3.14159265/180);
    double y = SPRITE_RADIUS*2*sin(getDirection()*3.14159265/180);
    if(getWorld()->checkProjOverlap(getX(), getY(), getDamage())){
        setDead();
        return;
    }
    moveTo(getX()+x, getY()+y);
    increaseDist(SPRITE_RADIUS*2);
    if(getDist()>=getMaxTravel()){
        setDead();
    }
    
}

//FLAME PROJECTILE
Flame::Flame(int startX, int startY, Direction startDirection, StudentWorld* ptr)
:Projectile(IID_FLAME, startX, startY, startDirection, 1, ptr, 32, 5)
{
    
}


//SPRAY PROJECTILE
Spray::Spray(int startX, int startY, Direction startDirection, StudentWorld* ptr)
:Projectile(IID_SPRAY, startX, startY, startDirection, 1, ptr, 112, 2)
{
    
}

//GOODIES

Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* ptr)
:Actor(imageID, startX, startY, 0, 1, ptr)
{
    m_lifespan = std::max(rand()%(300-10*getWorld()->getLevel()), 50);
}

int Goodie::getLifespan(){
    return m_lifespan;
}

void Goodie::decreaseLife(){
    m_lifespan-=1;
}


//RESTORE HEALTH GOODIE

RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* ptr)
:Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, ptr)
{
    
}

void RestoreHealth::doSomething(){
    if(isDead()){
        return;
    }
    
    if(getWorld()->checkSocratesOverlap(getX(), getY())){
        //increase points by 250
        getWorld()->increaseScore(250);
        setDead();
        //play sound
        //restore health
        getWorld()->restorePlayerHealth();
        return;
    }
    
    if(getLifespan()<=0){
        setDead();
    }
    
    decreaseLife();
}

//FLAME GOODIE

FlameGoodie::FlameGoodie(int startX, int startY, StudentWorld* ptr)
:Goodie(IID_FLAME_THROWER_GOODIE, startX, startY, ptr)
{
    
}

void FlameGoodie::doSomething(){
    if(isDead()){
        return;
    }
    
    if(getWorld()->checkSocratesOverlap(getX(), getY())){
        getWorld()->increaseScore(300);
        setDead();
        //play sound
        getWorld()->addPlayerFlame();
        return;
    }
    
    if(getLifespan()<=0){
        setDead();
    }
    
    decreaseLife();
}

//CHANGE HERE

ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* ptr)
:Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, ptr)
{

}

void ExtraLifeGoodie::doSomething(){
    if(isDead()){
        return;
    }
    if(getWorld()->checkSocratesOverlap(getX(), getY())){
        getWorld()->increaseScore(500);
        setDead();
        //playsound
        getWorld()->incLives();
        return;
    }
    if(getLifespan()<=0){
        setDead();
    }

    decreaseLife();
    return;
}

Fungus::Fungus(int startX, int startY, StudentWorld* ptr)
:Goodie(IID_FUNGUS, startX, startY, ptr)
{
    
}

void Fungus::doSomething(){
    if(isDead()){
        return;
    }
    if(getWorld()->checkSocratesOverlap(getX(), getY())){
        getWorld()->increaseScore(-50);
        setDead();
        getWorld()->hurtPlayerHealth(20);
        return;
    }

    if(getLifespan()<=0){
        setDead();
    }

    decreaseLife();
    return;
}


Bacteria::Bacteria(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld* ptr, int hp, int mpDist)
:LivingActor(imageID, startX, startY, startDirection, depth, ptr, hp)
{
    m_foodCount = 0;
    m_mpDist = mpDist;
}

int Bacteria::getFoodCount(){
    return m_foodCount;
}

void Bacteria::resetFoodCount(){
    m_foodCount = 0;
}

void Bacteria::incFoodCount(){
    m_foodCount+=1;
}

int Bacteria::getMpDist(){
    return m_mpDist;
}

void Bacteria::decMpDist(){
    m_mpDist-=1;
}

void Bacteria::setMpDist(){
    m_mpDist = 10;
}

Salmonella::Salmonella(int startX, int startY, StudentWorld* ptr)
:Bacteria(IID_SALMONELLA, startX, startY, 90, 0, ptr, 4, 0)
{
    
}

void Salmonella::doSomething(){
    checkDead();
    if(isDead()){
        return;
    }
    if(getWorld()->checkSocratesOverlap(getX(), getY())){
        getWorld()->hurtPlayerHealth(1);
    }else if(getFoodCount()==3){
        double newx;
        double newy;
        if(getX()<=VIEW_WIDTH/2){
            newx = getX()+SPRITE_RADIUS;
        }else{
            newx = getX()-SPRITE_RADIUS;
        }
        if(getY()<=VIEW_HEIGHT/2){
            newy = getY()+SPRITE_RADIUS;
        }else{
            newy = getY()-SPRITE_RADIUS;
        }
        //add new salmonella at coordinate
        getWorld()->addBacteria(new Salmonella(newx, newy, getWorld()));
        resetFoodCount();
    }else if(getWorld()->checkFoodOverlap(getX(), getY())){
        incFoodCount();
    }
    
    if(getMpDist()>0){
        decMpDist();
        if(getWorld()->canBacteriaMoveForward(getX(), getY(), getDirection())){
            double x = getX()+ 3*cos(getDirection()*3.14159265/180);
            double y = getY()+ 3*sin(getDirection()*3.14159265/180);
            moveTo(x, y);
        }else{
            int dir = randInt(0, 359);
            setDirection(dir);
            setMpDist();
        }
        return;
    }else{
        //get directional angle of closest food
        int dir;
        if(getWorld()->getClosestFoodAngle(getX(), getY(), dir)){
            setDirection(dir);
            if(getWorld()->canBacteriaMoveForward(getX(), getY(), dir)){
                double x = getX()+ 3*cos(getDirection()*3.14159265/180);
                double y = getY()+ 3*sin(getDirection()*3.14159265/180);
                moveTo(x, y);
            }else{
                setDirection(randInt(0, 359));
                setMpDist();
            }
        }else{
            setDirection(randInt(0, 359));
            setMpDist();
        }
    }
}


