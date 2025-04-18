/// <summary>
/// author Pete Lowe April 2025
/// you need to watch this first
/// https://www.youtube.com/watch?v=Ap5eBYKlGDo
/// </summary>

#include "Game.h"
#include <iostream>




/// <summary>
/// default constructor
/// setup the window properties
/// load and setup the text 
/// load and setup thne image
/// </summary>
Game::Game() :
	m_window{ sf::VideoMode{ 800U, 600U, 32U }, "SFML Game" },
	m_exitGame{false} //when true game will exit
{
	setupFontAndText(); // load font 
	setupSprite(); // load texture
}

/// <summary>
/// default destructor we didn't dynamically allocate anything
/// so we don't need to free it, but mthod needs to be here
/// </summary>
Game::~Game()
{
}


/// <summary>
/// main game loop
/// update 60 times per second,
/// process update as often as possible and at least 60 times per second
/// draw as often as possible but only updates are on time
/// if updates run slow then don't render frames
/// </summary>
void Game::run()
{	
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float fps{ 60.0f };
	sf::Time timePerFrame = sf::seconds(1.0f / fps); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame); //60 fps
#ifdef _DEBUG
			render(); // want to debug drawing while stepping through code
#endif // _DEBUG

		}
		render(); // as many as possible
	}
}
/// <summary>
/// handle user and system events/ input
/// get key presses/ mouse moves etc. from OS
/// and user :: Don't do game update here
/// </summary>
void Game::processEvents()
{
	sf::Event newEvent;
	while (m_window.pollEvent(newEvent))
	{
		if ( sf::Event::Closed == newEvent.type) // window message
		{
			m_exitGame = true;
		}
		if (sf::Event::KeyPressed == newEvent.type) //user pressed a key
		{
			processKeys(newEvent);
		}
	}
}


/// <summary>
/// deal with key presses from the user
/// </summary>
/// <param name="t_event">key press event</param>
void Game::processKeys(sf::Event t_event)
{
	if (sf::Keyboard::Escape == t_event.key.code)
	{
		m_exitGame = true;
	}
	if (sf::Keyboard::X == t_event.key.code)
	{
		if (mode == Collision::AABB)
		{
			mode = Collision::SAT;
			m_welcomeMessage.setString("<shift> arrows to move +- to rotate,  X swap to AABB,P Pixel");
		}
		else
		{
			mode = Collision::AABB;
			m_welcomeMessage.setString("<shift> arrows to move +- to rotate,  X swap to SAT,P Pixel");
		}
	}
	if (sf::Keyboard::P == t_event.key.code)
	{
		mode = Collision::PIXEL;
		m_welcomeMessage.setString("<shift> arrows to move +- to rotate,  X swap to AABB");
	}
}

/// <summary>
/// Update the game world
/// </summary>
/// <param name="t_deltaTime">time interval per frame</param>
void Game::update(sf::Time t_deltaTime)
{
	if (t_deltaTime.asMilliseconds() != 16)
	{
		std::cout << "time warp" << std::endl; // expecting 60 fps of action
	}
	if (m_exitGame)
	{
		m_window.close();
	}
	handleInput();
	checkCollision();
}

void Game::handleInput()
{
	sf::Sprite* target;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
	{
		target = &m_logoSprite;
	}
	else
	{
		target = &m_otherSprite;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		target->move(sf::Vector2f{ 0.0f,-3.0f });		
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		target->move(sf::Vector2f{ 0.0f,3.0f });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		target->move(sf::Vector2f{ -3.0f,0.0f });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		target->move(sf::Vector2f{ 3.0f,0.0f });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
	{
		target->rotate(5.0f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
	{
		target->rotate(-5.0f);
	}

}

/// <summary>
/// draw the frame and then switch buffers
/// </summary>
void Game::render()
{
	m_window.clear(sf::Color::Black);
	m_window.draw(m_welcomeMessage);
	m_window.draw(m_logoSprite);
	m_window.draw(m_otherSprite);
	m_window.display();
}

/// <summary>
/// load the font and setup the text message for screen
/// </summary>
void Game::setupFontAndText()
{
	if (!m_ArialBlackfont.loadFromFile("ASSETS\\FONTS\\ariblk.ttf"))
	{
		std::cout << "problem loading arial black font" << std::endl;
	}
	m_welcomeMessage.setFont(m_ArialBlackfont);
	m_welcomeMessage.setString("<shift> arrows to move +- to rotate,  X swap to SAT,P Pixel");
	
	m_welcomeMessage.setPosition(40.0f, 40.0f);
	m_welcomeMessage.setCharacterSize(20U);
	m_welcomeMessage.setOutlineColor(sf::Color::Red);
	m_welcomeMessage.setFillColor(sf::Color::White);
	m_welcomeMessage.setOutlineThickness(1.0f);

}

/// <summary>
/// load the texture and setup the sprite for the logo
/// </summary>
void Game::setupSprite()
{
	if (!m_logoTexture.loadFromFile("ASSETS\\IMAGES\\SFML-LOGO.png"))
	{
		// simple error message if previous call fails
		std::cout << "problem loading logo" << std::endl;
	}
	m_logoSprite.setTexture(m_logoTexture);
	m_logoSprite.setPosition(100.0f, 80.0f);
	m_otherSprite.setTexture(m_logoTexture);
	m_otherSprite.setPosition(500.0f, 500.0f);
}

void Game::checkCollision()
{
	bool overlap{ false };
	if (mode == Collision::AABB)
	{
		overlap = m_logoSprite.getGlobalBounds().intersects(m_otherSprite.getGlobalBounds());		
	}
	if (mode == Collision::SAT)
	{
		overlap = CollisionDetector::collision(m_logoSprite, m_otherSprite);
	}
	if (mode == Collision::PIXEL)
	{
		overlap = CollisionDetector::pixelPerfectTest(m_logoSprite, m_otherSprite, 50u);
	}
	if (overlap)
	{
		m_logoSprite.setColor(sf::Color::Red);
	}
	else
	{
		m_logoSprite.setColor(sf::Color::White);
	}
	
}


