#include <jni.h>
#include <iostream>
#include <MsHook.h>

#define hook(a,b,c) MSHookFunction((void*)&a,(void*)&b,(void**)&c)

struct Level;
struct Entity;
struct Mob;
struct Player;
struct MinecraftClient;
struct EntityDamageSource;
struct ResourcePackManager;
struct MobEffectInstance;

struct Level
{
	int getSeed();
};
struct Entity
{
	Level *getLevel();
};
struct Mob : Entity
{
	void removeAllEffects();
};
struct Player : Mob
{
	void die(EntityDamageSource const&);
	int getExperienceReward() const;
	int getScore();
	void addExperience(int);
	void respawn();
	void displayClientMessage(std::string const&);
};
struct MinecractClient
{
	void onPlayerLoaded(Player &);
};

bool isDied = false;
int exp;
int levelId;

static void (*_Player_die)(Player *,EntityDamageSource const&);
static void Player_die(Player *p,EntityDamageSource const&eds)
{
	isDied = true;
	exp = p->getScore();
	levelId = p->getLevel()->getSeed();
}
static void (*_Player_getExperienceReward)(Player *);
static void Player_getExperienceReward(Player *p)
{
	
}
static void (*_Player_respawn)(Player *);
static void Player_respawn(Player *p)
{
	_Player_respawn(p);
	
	if(isDied == true && p->getLevel()->getSeed() == levelId){
		p->addExperience(exp);
		isDied = false;
		exp = 0;
		levelId = 0;
	}
		
}
static void (*_Mob_removeAllEffects)(Mob *);
static void Mob_removeAllEffects(Mob *mob)
{
	if(isDied == false)
	{
		_Mob_removeAllEffects(mob);
	}
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	
	hook(Player::die,Player_die,_Player_die);
	hook(Player::getExperienceReward,Player_getExperienceReward,_Player_getExperienceReward);
	hook(Player::respawn,Player_respawn,_Player_respawn);
	hook(Mob::removeAllEffects,Mob_removeAllEffects,_Mob_removeAllEffects);
	
return JNI_VERSION_1_6;
}
