// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <vector>

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mLeftPaddleDir(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	mLeftPaddlePos.x = 10.0f;
	mLeftPaddlePos.y = 768.0f/2.0f;
	mRightPaddlePos.x = 1024.0f - thickness - 10.0f;
	mRightPaddlePos.y = 768.0f / 2.0f;

	mBalls.push_back({ 1024.0f / 2.0f, 768.0f / 2.0f, -150.0f, 235.0f });
	mBalls.push_back({ 1024.0f / 2.0f, 768.0f / 2.0f,  150.0f, 235.0f });
	mBalls.push_back({ 1024.0f / 2.0f, 768.0f / 3.0f,  50.0f, 500.0f });
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update left paddle direction based on W/S keys
	mLeftPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddleDir += 1;
	}

	// Update right paddle direction based on W/S keys
	mRightPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mRightPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mRightPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update left paddle position based on direction
	if (mLeftPaddleDir != 0)
	{
		mLeftPaddlePos.y += mLeftPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mLeftPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mLeftPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mLeftPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mLeftPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update right paddle position based on direction
	if (mRightPaddleDir != 0)
	{
		mRightPaddlePos.y += mRightPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mRightPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mRightPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mRightPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mRightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	for (auto& it : mBalls) {
		it.p.x += it.v.x * deltaTime;
		it.p.y += it.v.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the left paddle?
		float diffLeft = mLeftPaddlePos.y - it.p.y;
		float diffRight = mRightPaddlePos.y - it.p.y;
		// Take absolute value of difference
		diffLeft = (diffLeft > 0.0f) ? diffLeft : -diffLeft;
		diffRight = (diffRight > 0.0f) ? diffRight : -diffRight;
		if (
			// Our y-difference is small enough
			diffLeft <= paddleH / 2.0f &&
			// We are in the correct x-position
			it.p.x <= 25.0f && it.p.x >= 20.0f &&
			// The ball is moving to the left
			it.v.x < 0.0f)
		{
			it.v.x *= -1.0f;
		}
		// Did we intersect with the left paddle?
		else if (
			 diffRight <= paddleH / 2.0f && it.p.x >= 1024.0f - 25.0f && it.p.x <= 1024.0f - 20.0f && it.v.x > 0.0f
		)
		{
			it.v.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (it.p.x <= 0.0f || it.p.x >= 1024.0f)
		{
			mIsRunning = false;
		}


		// Did the ball collide with the top wall?
		if (it.p.y <= thickness && it.v.y < 0.0f)
		{
			it.v.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (it.p.y >= (768 - thickness) &&
			it.v.y > 0.0f)
		{
			it.v.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw left paddle
	SDL_Rect lpaddle{
		static_cast<int>(mLeftPaddlePos.x),
		static_cast<int>(mLeftPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &lpaddle);

	// Draw right paddle
	SDL_Rect rpaddle{
		static_cast<int>(mRightPaddlePos.x),
		static_cast<int>(mRightPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &rpaddle);
	
	// Draw ball
	for (auto b : mBalls) {
		SDL_Rect ball{
			static_cast<int>(b.p.x - thickness / 2),
			static_cast<int>(b.p.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);
	}
	
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
