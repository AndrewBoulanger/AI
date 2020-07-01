#include "Player.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "DebugManager.h"
#define SPEED 2

Player::Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0)
{
	m_dX = m_dY = m_vel = 0.0;
	m_accel = 1;
	m_velMax = 5.0;
}

void Player::Update()
{
	switch (m_state)
	{
	case idle:
		Stop();
		if (EVMA::KeyHeld(SDL_SCANCODE_W) || EVMA::KeyHeld(SDL_SCANCODE_S) ||
			EVMA::KeyHeld(SDL_SCANCODE_A) || EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			SetState(running);
		}
		break;

	case arrive:
	{

		if (MAMA::Distance((GetDstP()->x + GetDstP()->w / 2.0f), (m_nodes[trgt]->GetToNode()->x + 16.0),
			(GetDstP()->y + GetDstP()->h / 2.0f), (m_nodes[trgt]->GetToNode()->y + 16.0f)) <= 5.0)
		{
			if (trgt == m_nodes.size() - 1)
				SetState(idle);
			else
			{
				Stop();
				trgt++;
			}
		}
		else
		{
			double destAngle = MAMA::AngleBetweenPoints((m_nodes[trgt]->GetToNode()->y + 16.0f) - (GetDstP()->y + GetDstP()->h / 2.0f),
				(m_nodes[trgt]->GetToNode()->x + 16.0f) - (GetDstP()->x + GetDstP()->w / 2.0f));
			Move2Stop(MAMA::Rad2Deg(destAngle));
		}

		break;
	}
	case running:
		if (EVMA::KeyReleased(SDL_SCANCODE_W) || EVMA::KeyReleased(SDL_SCANCODE_S) ||
			EVMA::KeyReleased(SDL_SCANCODE_A) || EVMA::KeyReleased(SDL_SCANCODE_D))
		{
			SetState(idle);
			break; // Skip movement parsing below.
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_W))
		{
			if (m_dst.y > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
			{
				m_dst.y += -SPEED;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_S))
		{
			if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
			{
				m_dst.y += SPEED;
			}
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_A))
		{
			if (m_dst.x > 0  && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
			{
				m_dst.x += -SPEED;
				m_dir = 1;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			if (m_dst.x < 1024 - 32 &&  !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
			{
				m_dst.x += SPEED;
				m_dir = 0;
			}
		}
		break;
	}
	Animate();
	m_dst.x += m_dX;
	m_dst.y += m_dY;
}

void Player::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	/*if(m_nodes.size() != 0)
	DEMA::DrawLine({ (int)m_dst.x, (int)m_dst.y }, { (int)m_nodes.at(trgt)->GetToNode()->x, (int)m_nodes.at(trgt)->GetToNode()->y }, { 0,0,0,0 });*/
}

void Player::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = m_spriteMin = m_spriteMax = 0;
	}
	else // Only other is running for now...
	{
		m_sprite = m_spriteMin = 1;
		m_spriteMax = 4;
	}
}

void Player::Move2Stop(const double angle)
{
	if (angle <45 && angle > -45 )
	{
		m_dX = 1;
		m_dir = 0;
	}
	else if (angle > 135 || angle < -135)
	{
		m_dX = -1;
		m_dir = 1;
	}
	if (angle > 45 && angle < 135)
		m_dY = 1;
	else if (angle > -135 && angle < -45)
		m_dY = -1;

}

void Player::Stop()
{
	m_dX = 0;
	m_dY = 0;
	m_vel = 0;
}

void Player::setTargetPath(std::vector<PathConnection*> nodes)
{
	if (nodes.size() != 0)
	{
		m_nodes = nodes;
		trgt = 0;
	}
}


