#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Sprite.h"
#include "Tile.h"
#include <vector>

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update();
	void Render();
	void setTargetPath(std::vector<PathConnection*> nodes);
	void Stop();
	void Move2Stop(const double angle);
	void SetVs(const double angle);
	void SetState(int s);
private:
	enum state { idle, running, arrive } m_state;
	bool m_dir;
	std::vector<PathConnection*> m_nodes;
	double m_dX, m_dY, m_accel, m_vel, m_velMax;
	int trgt;
};

#endif

