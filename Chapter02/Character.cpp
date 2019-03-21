#include "Character.h"
#include "AnimSpriteComponent.h"
#include "TileMapComponent.h"
#include "Game.h"

Character::Character(Game* game)
	: Actor(game)
	, mRightSpeed(0.0f)
	, mDownSpeed(0.0f)
	, mAnimIndex(0)
{
	// Create an animated sprite component
	mAsc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Character01.png"),
		game->GetTexture("Assets/Character02.png"),
		game->GetTexture("Assets/Character03.png"),
		game->GetTexture("Assets/Character04.png"),
		game->GetTexture("Assets/Character05.png"),
		game->GetTexture("Assets/Character06.png"),
		game->GetTexture("Assets/Character07.png"),
		game->GetTexture("Assets/Character08.png"),
		game->GetTexture("Assets/Character09.png"),
		game->GetTexture("Assets/Character10.png"),
		game->GetTexture("Assets/Character11.png"),
		game->GetTexture("Assets/Character12.png"),
		game->GetTexture("Assets/Character13.png"),
		game->GetTexture("Assets/Character14.png"),
		game->GetTexture("Assets/Character15.png"),
		game->GetTexture("Assets/Character16.png"),
		game->GetTexture("Assets/Character17.png"),
		game->GetTexture("Assets/Character18.png"),
	};
	mAsc->SetAnimTextures(anims);
	mAsc->AddAnimDef({ 0,1,2,3,4,5 });
	mAsc->AddAnimDef({ 6,7,8,9,10,11,12,13,14 });
	mAsc->AddAnimDef({ 15,16,17 });
}

void Character::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
}

void Character::ProcessKeyboard(const uint8_t* state)
{
	if (state[SDL_SCANCODE_1]) {
		mAsc->SetAnim(0);
	} else 	if (state[SDL_SCANCODE_2]) {
		mAsc->SetAnim(1);
	} else 	if (state[SDL_SCANCODE_3]) {
		mAsc->SetAnim(2);
	}
}