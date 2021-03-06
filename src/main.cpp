#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

using namespace sf;

// `constexpr` defines an immutable compile-time value.
constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.f }, ballVelocity{ 8.f };
constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 6.f };

constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };
unsigned int counter{ countBlocksX * countBlocksY };
sf::Text counterText;
constexpr unsigned int BUTTON_A{ 0 };
constexpr unsigned int BUTTON_B{ 1 };
constexpr unsigned int BUTTON_X{ 2 };
constexpr unsigned int BUTTON_Y{ 3 };
const std::string PADDLEHIT_SOUND { "chord.wav" };
sf::Sound paddleHitsound;
const std::string WALLHIT_SOUND{ "block.wav" };
sf::Sound wallHitsound;
const std::string BRICKHIT_SOUND{ "stop.wav" };
sf::Sound brickHitsound;

struct Ball
{
	CircleShape shape;
	Vector2f velocity{ -ballVelocity, -ballVelocity };

	Ball(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setRadius(ballRadius);
		shape.setFillColor(Color::Red);
		shape.setOrigin(ballRadius, ballRadius);
	}

	void update()
	{
		shape.move(velocity);

		// We need to keep the ball "inside the screen".

		// If it's leaving toward the left, we need to set
		// horizontal velocity to a positive value (towards the right).
		if (left() < 0)
		{
			velocity.x = ballVelocity;
			wallHitsound.play();
		}

		// Otherwise, if it's leaving towards the right, we need to
		// set horizontal velocity to a negative value (towards the left).
		else if (right() > windowWidth)
		{
			velocity.x = -ballVelocity;
			wallHitsound.play();
		}

		// The same idea can be applied for top/bottom collisions.
		if (top() < 0)
		{
			velocity.y = ballVelocity;
			wallHitsound.play();
		}
		else if (bottom() > windowHeight)
		{
			velocity.y = -ballVelocity;
			wallHitsound.play();
		}
	}

	// We can also create "property" methods to easily
	// get commonly used values.
	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getRadius(); }
	float right() { return x() + shape.getRadius(); }
	float top() { return y() - shape.getRadius(); }
	float bottom() { return y() + shape.getRadius(); }
};
struct Paddle
{
	// RectangleShape is an SFML class that defines
	// a renderable rectangular shape.
	RectangleShape shape;
	Vector2f velocity;

	// As with the ball, we construct the paddle with
	// arguments for initial position and pass the values
	// to the SFML `shape`.
	Paddle(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Red);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}

	void update()
	{
		shape.move(velocity);

		// To move the paddle, we check if the user is pressing
		// the left or right arrow key: if so, we change the velocity.
		if (sf::Joystick::isConnected(0))
		{
			//float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
			// XBOX controller
			// Button A = 0    
			// Button B = 1    
			// Button X = 2    
			// Button Y = 3    
			//
			//unsigned int buttonCount = sf::Joystick::getButtonCount(0);
			//for (size_t i = 0; i < buttonCount; i++)
			//{
			//	if (sf::Joystick::isButtonPressed(0, i))
			//	{
			//		std::cout << "Button:" << i << " pressed" << std::endl;
			//	}
			//}
			
			if (sf::Joystick::isButtonPressed(0, BUTTON_X) && left() > 0)
			{
				velocity.x = -paddleVelocity;
				return;
			}
			else if (sf::Joystick::isButtonPressed(0, BUTTON_B) && right() < windowWidth)
			{
				velocity.x = paddleVelocity;
				return;
			}
		}


		// To keep the paddle "inside the window", we change the velocity
		// only if its position is inside the window.
		if (Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
			velocity.x = -paddleVelocity;
		else if (Keyboard::isKeyPressed(Keyboard::Key::Right) &&
			right() < windowWidth)
			velocity.x = paddleVelocity;

		// If the user isn't pressing anything, stop moving.
		else
			velocity.x = 0;
	}

	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getSize().x / 2.f; }
	float right() { return x() + shape.getSize().x / 2.f; }
	float top() { return y() - shape.getSize().y / 2.f; }
	float bottom() { return y() + shape.getSize().y / 2.f; }
};
struct Brick
{
	RectangleShape shape;

	// This boolean value will be used to check
	// whether a brick has been hit or not.
	bool destroyed{ false };

	// The constructor is almost identical to the `Paddle` one.
	Brick(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setSize({ blockWidth, blockHeight });
		shape.setFillColor(Color::Yellow);
		shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
	}

	float x() { return shape.getPosition().x; }
	float y() { return shape.getPosition().y; }
	float left() { return x() - shape.getSize().x / 2.f; }
	float right() { return x() + shape.getSize().x / 2.f; }
	float top() { return y() - shape.getSize().y / 2.f; }
	float bottom() { return y() + shape.getSize().y / 2.f; }
};

// Dealing with collisions: let's define a generic function
// to check if two shapes are intersecting (colliding).
template <class T1, class T2>
bool isIntersecting(T1& mA, T2& mB)
{
	return mA.right() >= mB.left() && mA.left() <= mB.right() &&
		mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

// Let's define a function that deals with paddle/ball collision.
void testCollision(Paddle& mPaddle, Ball& mBall)
{
	// If there's no intersection, get out of the function.
	if (!isIntersecting(mPaddle, mBall)) return;

	// paddle hit make sound 
	paddleHitsound.play();
	// Otherwise let's "push" the ball upwards.
	mBall.velocity.y = -ballVelocity;

	// And let's direct it dependently on the position where the
	// paddle was hit.
	if (mBall.x() < mPaddle.x())
		mBall.velocity.x = -ballVelocity;
	else
		mBall.velocity.x = ballVelocity;
}
void testCollision(Brick& mBrick, Ball& mBall)
{
	// If there's no intersection, get out of the function.
	if (!isIntersecting(mBrick, mBall)) return;
	// Otherwise, the brick has been hit!

	brickHitsound.play();
	counter--;
	counterText.setString(std::to_string(counter));
	mBrick.destroyed = true;

	// Let's calculate how much the ball intersects the brick
	// in every direction.
	float overlapLeft{ mBall.right() - mBrick.left() };
	float overlapRight{ mBrick.right() - mBall.left() };
	float overlapTop{ mBall.bottom() - mBrick.top() };
	float overlapBottom{ mBrick.bottom() - mBall.top() };

	// If the magnitude of the left overlap is smaller than the
	// right one we can safely assume the ball hit the brick
	// from the left.
	bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));

	// We can apply the same idea for top/bottom collisions.
	bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

	// Let's store the minimum overlaps for the X and Y axes.
	float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
	float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

	// If the magnitude of the X overlap is less than the magnitude
	// of the Y overlap, we can safely assume the ball hit the brick
	// horizontally - otherwise, the ball hit the brick vertically.

	// Then, upon our assumptions, we change either the X or Y velocity
	// of the ball, creating a "realistic" response for the collision.
	if (abs(minOverlapX) < abs(minOverlapY))
		mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
	else
		mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}

int main()
{
	SoundBuffer paddleHitBuffer;
	if (!paddleHitBuffer.loadFromFile(PADDLEHIT_SOUND))
	{
		std::cerr << "Error loading " << PADDLEHIT_SOUND;
	}
	paddleHitsound.setBuffer(paddleHitBuffer);
	SoundBuffer wallHitBuffer;
	if (!wallHitBuffer.loadFromFile(WALLHIT_SOUND))
	{
		std::cerr << "Error loading " << WALLHIT_SOUND;
	}
	wallHitsound.setBuffer(wallHitBuffer);
	SoundBuffer brickHitBuffer;
	if (!brickHitBuffer.loadFromFile(BRICKHIT_SOUND))
	{
		std::cerr << "Error loading " << BRICKHIT_SOUND;
	}
	brickHitsound.setBuffer(brickHitBuffer);
	sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "Error loading font" << std::endl;
	}
	sf::Text title("Arkanoid",font, 20);
	title.setFillColor(sf::Color::Green);
	title.setStyle(sf::Text::Bold);
	title.setPosition(windowWidth / 2, windowHeight - windowHeight + 10);

	counterText.setFont(font);
	counterText.setFillColor(sf::Color::Green);
	counterText.setString(std::to_string(counter));


	Ball ball{ windowWidth / 2, windowHeight / 2 };
	Paddle paddle{ windowWidth / 2, windowHeight - 50 };
	std::vector<Brick> bricks;
	for (int iX{ 0 }; iX < countBlocksX; ++iX)
		for (int iY{ 0 }; iY < countBlocksY; ++iY)
			bricks.emplace_back(
			(iX + 1) * (blockWidth + 3) + 22, (iY + 2) * (blockHeight + 3));

	// Creation of the game window.
	RenderWindow window{ { windowWidth, windowHeight }, "Arkanoid - 1" };
	window.setFramerateLimit(60);

	sf::Event event;
	bool running = true;
	// Game loop.
	while (running)
	{
		// "Clear" the window from previously drawn graphics.
		window.clear(Color::Black);

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					running = false;
				}
			}
		}
		ball.update();
		paddle.update();
		testCollision(paddle, ball);
		for (auto& brick : bricks) testCollision(brick, ball);
		bricks.erase(remove_if(begin(bricks), end(bricks),
			[](const Brick& mBrick)
		{
			return mBrick.destroyed;
		}),
			end(bricks));
		// Show the window contents.
		window.draw(title);
		window.draw(counterText);
		window.draw(ball.shape);
		window.draw(paddle.shape);
		for (auto& brick : bricks) window.draw(brick.shape);
		window.display();
	}

	return 0;
}