#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "Button.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "EventManager.h"
#include "PathManager.h"
#include "DebugManager.h"
#include "FontManager.h"
#include <string.h>

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

// Begin TitleState.
TitleState::TitleState() {}

void TitleState::Enter()
{
	m_playBtn = new StartButton({ 0,0,200,100 }, { 350.0f,400.0f,300.0f,100.0f }, Engine::Instance().GetRenderer(), TEMA::GetTexture("start"));
	name = new Label("tile", 350, 650, "Andrew Boulanger 101292574", { 0,100,200,0 });
}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 150, 50, 0);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	for (int i = 0; i < 3; i++)
		name->Render();
	m_playBtn->Render();
	State::Render();
}

void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}
// End TitleState.


// ============================

GameState::GameState() {}
//entering gamestate
void GameState::Enter()
{
	std::cout << "Entering GameState..." << std::endl;

	m_pPlayer = new Player({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("player"), 0, 0, 0, 4);
	m_pBling = new Sprite({ 224,64,32,32 }, { (float)(16) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("tiles"));
	
	SOMA::PlayMusic("game");
	m_level = Engine::Instance().GetLevel();

	m_costText = new Label("tile", 10, 10, "current cost: " );
	m_instructions = new Label("tile", 10, 740, "H = debug mode, F = display viable paths, M = move character");
	m_debugInstruct = new Label("tile", 10, 750, "left click to place player, right click to place the goal");
}

void GameState::Update()
{
	m_pPlayer->Update(); // Just stops MagaMan from moving.
	if (EVMA::KeyPressed(SDL_SCANCODE_H)) // h key. Toggle debug mode.
		m_showCosts = !m_showCosts;
	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
	{
		m_hEuclid = !m_hEuclid;
		std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
	}
	if ((EVMA::MousePressed(1) || EVMA::MousePressed(3)) && m_showCosts) // If user has clicked.
	{
		m_pPlayer->SetState(0); //idle state
		int xIdx = (EVMA::GetMousePos().x / 32);
		int yIdx = (EVMA::GetMousePos().y / 32);
		if (m_level[yIdx][xIdx]->IsObstacle() || m_level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
			return; // We clicked on an invalid tile.
		if (EVMA::MousePressed(1)) // Move the player with left-click.
		{
			m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
			m_pPlayer->GetDstP()->y = (float)(yIdx * 32);
		}
		else if (EVMA::MousePressed(3)) // Else move the bling with right-click.
		{
			m_pBling->GetDstP()->x = (float)(xIdx * 32);
			m_pBling->GetDstP()->y = (float)(yIdx * 32);
		}
		for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
		{ // Update each node with the selected heuristic and set the text for debug mode.
			for (int col = 0; col < COLS; col++)
			{
				if (m_level[row][col]->Node() == nullptr)
					continue;
				if (m_hEuclid)
					m_level[row][col]->Node()->SetH(PAMA::HEuclid(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
				else
					m_level[row][col]->Node()->SetH(PAMA::HManhat(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
				m_level[row][col]->m_lCost->SetText(std::to_string((int)(m_level[row][col]->Node()->H())).c_str());
			}
		}
		PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_F))
	{
		// Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
		PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
		//for (unsigned i = 0; i < m_level[row][col]->Node()->GetConnections().size(); i++)
		m_pPlayer->SetState(0); //idle state
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_M))
	{
		m_pPlayer->setTargetPath(PAMA::getPath());
		m_pPlayer->SetState(2); //arrive state
	}

	m_cost = 0;
	for (int i = 0; i < PAMA::getPath().size(); i++)
	{
		m_cost += PAMA::getPath().at(i)->GetCost();
	
	}
	//	m_cost = m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node()->H();
		m_costText->SetText(("current cost: " + std::to_string(m_cost)).c_str());
}


void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			m_level[row][col]->Render(); // Render each tile.
			// Render the debug data...
			if (m_showCosts && m_level[row][col]->Node() != nullptr)
			{
				m_level[row][col]->m_lCost->Render();
				m_level[row][col]->m_lX->Render();
				m_level[row][col]->m_lY->Render();
				// I am also rendering out each connection in blue. If this is a bit much for you, comment out the for loop below.
				for (unsigned i = 0; i < m_level[row][col]->Node()->GetConnections().size(); i++)
				{
		/*			DEMA::QueueLine({ m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->x, m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->y },
						{ m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->x, m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->y }, { 0,0,255,255 });
					DEMA::QueueLine({ m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->x + 32, m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->y +32},
						{ m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->x +32, m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->y +32 }, { 0,0,255,255 });*/
					DEMA::QueueLine({ m_level[row][col]->Node()->x, m_level[row][col]->Node()->y }, { m_level[row][col]->Node()->x + 32, m_level[row][col]->Node()->y }, { 0,0,255,255 });
					DEMA::QueueLine({ m_level[row][col]->Node()->x, m_level[row][col]->Node()->y }, { m_level[row][col]->Node()->x , m_level[row][col]->Node()->y +32 }, { 0,0,255,255 });
				}
			}
		}
	}
	m_pPlayer->Render();
	m_pBling->Render();
	m_costText->Render();
	m_instructions->Render();
	if (m_showCosts)
		m_debugInstruct->Render();
	PAMA::DrawPath(); // I save the path in a static vector to be drawn here.
	DEMA::FlushLines(); // And... render ALL the queued lines. Phew.



	// Draw the platforms.
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 70, 192, 0, 255);

	// If GameState != current state.
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();

}

void GameState::Exit()
{

}
