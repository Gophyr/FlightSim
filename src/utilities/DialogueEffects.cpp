#include "DialogueEffects.h"
#include "Campaign.h"

void smallSupply()
{
	f32 amt = (f32)((std::rand() % 10) + 1);
	campaign->addSupplies(amt);
}
void mediumSupply()
{
	f32 amt = (f32)((std::rand() % 30) + 10);
	campaign->addSupplies(amt);
}
void largeSupply()
{
	f32 amt = (f32)((std::rand() % 60) + 20);
	campaign->addSupplies(amt);
}

void smallAmmo()
{
	campaign->addAmmo((std::rand() % 5) + 1);
}
void mediumAmmo()
{
	campaign->addAmmo((std::rand() % 10) + 4);
}
void largeAmmo()
{
	campaign->addAmmo((std::rand() % 20) + 10);
}
void noDialogueEffect()
{
	//NOTHING!
}
void exitDialogue()
{
	//kill the current dialogue
}