#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include<climits>

using namespace std;
using namespace sf;

int const r = 25;
int const c = 30;
char arr[r][c];

float tilesize = 20;
//-------------------------------------------------------------
//-------------------------------------------------------------
//for fire 
struct Particle {
    CircleShape shape;
    Vector2f velocity;
    float lifetime;
};
struct FireEffect {
    Vector2f position;
    vector<Particle> particles;
    float spawnRate;
    float timeSinceLastSpawn;
    float intervalTimer;
    bool fireActive;

    FireEffect(Vector2f pos, float rate) : position(pos), spawnRate(rate), timeSinceLastSpawn(0), intervalTimer(0), fireActive(true) {}
};

void readdata2D() {
    ifstream sin;
    sin.open("Maze2.txt");
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            sin >> arr[i][j];
        }
    }
}


bool checkCollisionwithwalls(const FloatRect& playerBounds) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            if (arr[i][j] == 'b') {
                FloatRect wallBounds(j * tilesize, i * tilesize, tilesize, tilesize);
                if (playerBounds.intersects(wallBounds)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool resume1 = false, resume2 = false, resume3 = false, gameOver = false, moveToMenu = true, gameStarted = false;

VideoMode desktopMode = VideoMode::getDesktopMode();


bool MainMenu()
{
    //Main menu

    RenderWindow mainMenu(VideoMode(1160, 680), "Main Menu", Style::Fullscreen);

    mainMenu.setFramerateLimit(60);

    //Auto maximise
    mainMenu.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 39));
    mainMenu.setPosition(Vector2i(-7, -33));

    //Font
    Font font;
    font.loadFromFile("KnightWarrior.otf");


    RectangleShape background1(Vector2f(desktopMode.width, desktopMode.height));
    Texture bg;
    bg.loadFromFile("bg3.jpg");
    background1.setTexture(&bg);

    int resumeLevel;
    string line;
    ifstream res("resume.txt");
    res >> line;
    res >> resumeLevel;
    res.close();

    // Audio
    SoundBuffer mm;
    Sound main;
    mm.loadFromFile("audio/lulllaby.ogg");
    main.setBuffer(mm);

    main.play();
    main.setLoop(true);

    Text title, resumeP, sng, controls, exitG;

    //Title
    title.setFont(font);
    title.setCharacterSize(70);
    title.setPosition(180.f, 100.f);
    title.setString("Shadow Runners");
    title.setFillColor(Color::Red);

    //Resume
    resumeP.setFont(font);
    resumeP.setCharacterSize(25);
    resumeP.setPosition(180.f, 300.f);
    resumeP.setString("Press R to resume from previous save");

    //Start new game
    sng.setFont(font);
    sng.setCharacterSize(25);
    sng.setPosition(180.f, 400.f);
    sng.setString("Press Enter to Start New Game");

    //Controls
    controls.setFont(font);
    controls.setCharacterSize(25);
    controls.setPosition(180.f, 500.f);
    controls.setString("Press C to show controls");

    //Exit
    exitG.setFont(font);
    exitG.setCharacterSize(25);
    exitG.setPosition(180.f, 600.f);
    exitG.setString("Press Esc to exit");

    while (mainMenu.isOpen())
    {
        Event evnt;
        while (mainMenu.pollEvent(evnt))
        {
            if (evnt.type == Event::Closed)
            {
                return false;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
            {
                return false;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
            {

                mainMenu.close();
                main.stop();
                return true;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::R))
            {

                if (line == "1")
                {
                    mainMenu.close();
                    main.stop();
                    if (resumeLevel == 1)
                    {
                        resume1 = true;
                    }
                    else if (resumeLevel == 2)
                    {
                        resume2 = true;
                    }
                    else if (resumeLevel == 3)
                    {
                        resume3 = true;
                    }
                    return true;
                }
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::C))
            {

                bool openC = true;
                Texture c;
                RectangleShape control(Vector2f(276.f, 650.f));
                control.setPosition(100.f, 50.f);
                c.loadFromFile("controls.jpg");
                control.setTexture(&c);
                while (openC == true)
                {
                    mainMenu.clear();
                    mainMenu.draw(background1);
                    mainMenu.draw(control);
                    mainMenu.display();
                    if (Keyboard::isKeyPressed(Keyboard::Key::E))
                    {
                        openC = false;
                    }
                }
            }
        }



        mainMenu.clear();
        mainMenu.draw(background1);
        mainMenu.draw(title);
        if (line == "1")
        {
            mainMenu.draw(resumeP);
        }
        mainMenu.draw(sng);
        mainMenu.draw(controls);
        mainMenu.draw(exitG);
        mainMenu.display();
    }

    gameStarted = true;
}


bool Level1()
{
    while (moveToMenu == true)
    {
        //Declarations

        float startX = 55, startY = 540;
        bool shouldBreak = false;


        RenderWindow gameplay(VideoMode(680, 680), "the maze runner : metamorphosis", Style::Default);
        gameplay.setFramerateLimit(60);

        //Auto maximise
        gameplay.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 39));
        gameplay.setPosition(Vector2i(-7, -33));

        char arr[17][17];
        char collectibles[17][17];
        RectangleShape mazecollect[17][17];
        RectangleShape maze[17][17];
        RectangleShape wall(Vector2f(40.f, 40.f)), floor(Vector2f(40.f, 40.f)), end(Vector2f(40.f, 40.f)), puddle(Vector2f(40.f, 40.f)), go(Vector2f(40.f, 40.f)), coin(Vector2f(30.f, 30.f));
        //Texts
        Text text;
        Font font;
        //Blocks
        Texture start;
        Texture collect;
        Texture water;
        Texture brick;
        Texture grass;
        Texture win;
        Texture player;
        //Load
        if (!player.loadFromFile("player.png"))
            cout << "Couldn't load from file" << endl;
        if (!collect.loadFromFile("collect1.png"))
            cout << "Couldn't load from file" << endl;
        if (!start.loadFromFile("start.png"))
            cout << "Couldn't load from file" << endl;
        go.setTexture(&start);
        if (!water.loadFromFile("hurdle.jpg"))
            cout << "Couldn't load from file" << endl;
        puddle.setTexture(&water);
        if (!win.loadFromFile("endflag1.png"))
            cout << "Couldn't load from file" << endl;
        end.setTexture(&win);
        if (!brick.loadFromFile("wall2.jpg"))
            cout << "Couldn't load from file" << endl;
        if (!grass.loadFromFile("grass2.jpg"))
            cout << "Couldn't load from file" << endl;
        if (!font.loadFromFile("font.ttf"));
        cout << "Couldn't load from file" << endl;
        //settexture
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.f, 10.f);
        wall.setTexture(&brick);
        floor.setTexture(&grass);
        coin.setTexture(&collect);
        gameplay.setFramerateLimit(60);

        Sprite pl;
        pl.setTexture(player);
        pl.setTextureRect(IntRect(0, 306.5, 107.5, 153.25));
        pl.setScale(Vector2f(0.3f, 0.15f));
        pl.setPosition(55, 540);
        // Maze
        string line;
        ifstream file;
        file.open("youwon.txt");
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 17; j++)
            {
                file >> arr[i][j];
                //cout << arr[i][j] << " ";
            }
            //cout << endl;
        }

        //cout << "Collectibles" << endl;
        //COLLECTIBLES
        string line1;
        ifstream filecollect;
        filecollect.open("maze1.txt");
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 17; j++)
            {
                filecollect >> collectibles[i][j];
                //cout << collectibles[i][j] << " ";
            }
            //cout << endl;
        }

        // Sounds
        SoundBuffer pickup;
        pickup.loadFromFile("coinsound.wav");
        Sound pick;
        pick.setBuffer(pickup);
        SoundBuffer jumpsound;
        jumpsound.loadFromFile("jumpsound.wav");
        Sound jumpS;
        jumpS.setBuffer(jumpsound);

        SoundBuffer pauseB;
        pauseB.loadFromFile("audio/pause.mp3");
        Sound pauseS;
        pauseS.setBuffer(pauseB);

        //Music
        Music Music;
        if (!Music.openFromFile("music.wav"))
            cout << "Couldn't load from file" << endl;
        Music.setLoop(true);
        Music.setVolume(50.f);
        Music.play();



        // load the maze
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 17; j++)
            {
                if (arr[i][j] == 'w')
                {
                    maze[i][j] = wall;
                    maze[i][j].setPosition(j * 40, i * 40);
                }
                else if (arr[i][j] == 'f')
                {
                    maze[i][j] = floor;
                    maze[i][j].setPosition(j * 40, i * 40);
                }
                else if (arr[i][j] == 'W')
                {
                    maze[i][j] = end;
                    maze[i][j].setPosition(j * 40, i * 40);
                }
                else if (arr[i][j] == 'p')
                {
                    maze[i][j] = puddle;
                    maze[i][j].setPosition(j * 40, i * 40);
                }
                else if (arr[i][j] == 'g')
                {
                    maze[i][j] = go;
                    maze[i][j].setPosition(j * 40, i * 40);
                }
            }
        }
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 17; j++)
            {
                if (collectibles[i][j] == 'c')
                {
                    mazecollect[i][j] = coin;
                    mazecollect[i][j].setPosition(j * 40, i * 40);
                }
            }
        }
        int score = 0;
        text.setString("Current Score : " + to_string(score));

        //Jump variables

        bool isJumping = false;
        Vector2f startPos;
        Vector2f targetPos;
        float jumpProgress = 0.0f;
        bool ignoreSpike = false;
        int playerX;
        int playerY;


        //Clock timer
        Clock clock;
        Clock jumpClock;
        Time time;

        //Pause time
        Time pauseTime = Time::Zero; //Total accumulated pause time

        float prevTime = 0;
        string highscore = "0";



        ifstream levels1("Level 1.txt");
        levels1 >> highscore;

        if (resume1 == true)
        {
            levels1 >> score; // Load score
            levels1 >> startX; // Load player X position
            levels1 >> startY; // Load player Y position
            levels1 >> prevTime; // Load previous time

            // Load the maze array
            for (int i = 0; i < 17; i++) {
                for (int j = 0; j < 17; j++) {
                    levels1 >> collectibles[i][j];
                    // cout << collectibles[i][j];
                }
                //cout << endl;
            }
            pl.setPosition(startX, startY);
        }
        levels1.close();

        //Main loop
        while (gameplay.isOpen())
        {
            bool left = true, right = true, up = true, down = true;
            bool cleft = true, cright = true, cup = true, cdown = true;

            Event evnt;

            while (gameplay.pollEvent(evnt))
            {
                if (evnt.type == Event::Closed)
                {
                    gameplay.close();
                    return 0;
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                {
                    ofstream res("resume.txt");
                    res << "1" << endl;
                    res << "1" << endl;
                    res.close();
                    ofstream level1("Level 1.txt");
                    level1 << highscore << endl;
                    level1 << score << endl;
                    level1 << pl.getPosition().x << endl;
                    level1 << pl.getPosition().y << endl;
                    level1 << to_string(time.asSeconds() + prevTime) << endl;
                    for (int i = 0; i < 17; i++)
                    {
                        for (int j = 0; j < 17; j++)
                        {
                            level1 << collectibles[i][j];
                        }
                        level1 << endl;
                    }
                    level1.close();
                    gameplay.close();
                    return false;
                }
            }

            //Main game Timer

            time = clock.getElapsedTime() - pauseTime;

            Text timer;
            timer.setFont(font);
            timer.setString(to_string(time.asSeconds() + prevTime));
            timer.setPosition(250.f, 0.f);


            //Collision
            float playerleft = pl.getPosition().x;
            float playerright = playerleft + 25.f;
            float playertop = pl.getPosition().y;
            float playerbottom = playertop + 25.f;
            for (int i = 0; i < 17; i++)
            {

                for (int j = 0; j < 17; j++)
                {

                    if (arr[i][j] == 'w')
                    {
                        float wallleft = maze[i][j].getPosition().x;
                        float wallright = wallleft + 40.f;
                        float walltop = maze[i][j].getPosition().y;
                        float wallbottom = walltop + 40.f;

                        if (playerright > wallleft && playerleft < wallright && playertop < wallbottom && playerbottom > walltop)
                        {

                            if ((playerright > wallleft && playerleft < wallleft) || playerright >= 680)
                            {
                                right = false;
                            }
                            if ((playerleft < wallright && playerright > wallright) || playerleft <= 0)
                            {
                                left = false;
                            }


                            if (playerbottom > walltop && playertop < walltop)
                            {
                                down = false;
                            }
                            if (playertop < wallbottom && playerbottom > wallbottom)
                            {
                                up = false;
                            }
                        }
                    }

                    ////Jump
                    //int jump = false;
                    //if (Keyboard::isKeyPressed(Keyboard::Space))
                    //{
                    //    jump = true;

                    //}
                    //if (arr[i][j] == 'p')
                    //{
                    //    float hurdleleft = maze[i][j].getPosition().x;
                    //    float hurdleright = hurdleleft + 40.f;
                    //    float hurdletop = maze[i][j].getPosition().y;
                    //    float hurdlebottom = hurdletop + 40.f;


                    //    if (jump == true)
                    //    {
                    //        jumpS.play();
                    //    }

                    //    if ((playerright > hurdleleft && playerleft < hurdleright && playertop < hurdlebottom && playerbottom > hurdletop) && jump == false)
                    //    {
                    //        int count = 0;

                    //        if (playerright > hurdleleft && playerleft < hurdleleft)
                    //        {
                    //            right = false;
                    //        }
                    //        if (playerleft < hurdleright && playerright > hurdleright)
                    //        {
                    //            left = false;
                    //        }
                    //        if (playerbottom > hurdletop && playertop < hurdletop)
                    //        {

                    //            down = false;
                    //        }
                    //        if (playertop < hurdlebottom && playerbottom > hurdlebottom)
                    //        {

                    //            up = false;
                    //        }
                    //    }

                    //}

                    if (pl.getPosition().x + 20 >= 660 && pl.getPosition().y + 20 >= 140)
                    {
                        gameplay.close();
                        return true;
                    }
                    if (pl.getPosition().x <= 0)
                    {
                        left = false;
                    }
                }
            }



            //Jump functionality



            Time jumpTime = jumpClock.getElapsedTime();


            if (isJumping == false && jumpTime.asSeconds() > 0.5 && Keyboard::isKeyPressed(Keyboard::Key::Space))
            {
                jumpClock.restart();
                jumpS.play();

                playerX = (pl.getPosition().x + 15) / 40;
                playerY = (pl.getPosition().y + 30) / 40;
                startPos = pl.getPosition();

                if (arr[playerY][playerX - 1] == 'p' && playerX > 0)
                {
                    jumpS.play();
                    targetPos = startPos + Vector2f(-80.f, 0.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY][playerX + 1] == 'p' && playerX < 28)
                {
                    jumpS.play();
                    targetPos = startPos + Vector2f(80.f, 0.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY + 1][playerX] == 'p' && playerY < 16)
                {
                    jumpS.play();
                    targetPos = startPos + sf::Vector2f(0.f, 80.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY - 1][playerX] == 'p' && playerY > 0)
                {
                    jumpS.play();
                    targetPos = startPos + sf::Vector2f(0.f, -80.f);
                    isJumping = true;
                    ignoreSpike = true;
                }

                jumpProgress = 0.0f;
            }

            if (isJumping)
            {
                jumpProgress += 0.05f;

                if (jumpProgress >= 1.0f)
                {
                    jumpProgress = 1.0f;
                    isJumping = false;
                    ignoreSpike = false;
                }


                Vector2f newPos = startPos + (targetPos - startPos) * jumpProgress;
                pl.setPosition(newPos);
            }


            //COLLECTIBLES COLLISION AND SCORE
            for (int i = 0; i < 17; i++)
            {

                for (int j = 0; j < 17; j++)
                {
                    if (collectibles[i][j] == 'c')
                    {
                        if (pl.getGlobalBounds().intersects(mazecollect[i][j].getGlobalBounds()))
                        {
                            collectibles[i][j] = 'z';
                            score += 50;

                        }
                    }


                }
                text.setString("Current Score : " + to_string(score));
            }

            //MOVEMENTS

            if (right && (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)))
            {
                int xtexture = 0;
                xtexture = int(pl.getPosition().x / 10) % 4;
                xtexture = xtexture * 107.5;
                pl.setTextureRect(IntRect(xtexture, 306.5, 107.5, 153.25));
                pl.move(3.f, 0.f);
            }
            if (left && (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)))
            {
                int xtexture = 0;
                xtexture = int(pl.getPosition().x / 10) % 4;
                xtexture = xtexture * 107.5;
                pl.setTextureRect(IntRect(xtexture, 153.25, 107.5, 153.25));
                pl.move(-3.f, 0.f);
            }
            if (up && (Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)))
            {
                int ytexture = 0;
                ytexture = int(pl.getPosition().y / 10) % 4;
                ytexture = ytexture * 107.5;
                pl.setTextureRect(IntRect(ytexture, 459.75, 107.5, 153.25));
                pl.move(0.f, -3.f);
            }
            if (down && (Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)))
            {
                int ytexture = 0;
                ytexture = int(pl.getPosition().y / 10) % 4;
                ytexture = ytexture * 107.5;
                pl.setTextureRect(IntRect(ytexture, 0, 107.5, 153.25));
                pl.move(0.f, 3.f);
            }

            gameplay.clear();

            // Draw maze
            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 17; j++)
                {
                    gameplay.draw(maze[i][j]);
                }
            }
            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 17; j++)
                {
                    if (collectibles[i][j] == 'c')
                        gameplay.draw(mazecollect[i][j]);
                }
            }
            gameplay.draw(pl);
            gameplay.draw(text);
            gameplay.draw(timer);
            gameplay.display();

            //Pause game

            if (Keyboard::isKeyPressed(Keyboard::Key::P))
            {
                pauseS.play();

                Clock pauseClock;

                RenderWindow pause(VideoMode(1015, 595), "Paused", Style::Default);
                pause.setFramerateLimit(60);

                pause.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 35));
                pause.setPosition(Vector2i(-7, -40));

                RectangleShape background(Vector2f(1015.f, 595.f));
                Texture bg;
                bg.loadFromFile("background.jpg");
                background.setTexture(&bg);

                Text paused, resumeT, menu, exit, restart;

                //Paused
                paused.setFont(font);
                paused.setCharacterSize(100);
                paused.setPosition(235.f, 50.f);
                paused.setString("Game Paused");

                //Resume
                resumeT.setFont(font);
                resumeT.setCharacterSize(30);
                resumeT.setPosition(300.f, 200.f);
                resumeT.setString("Press Enter to resume level");

                //Restart
                restart.setFont(font);
                restart.setCharacterSize(30);
                restart.setPosition(300.f, 300.f);
                restart.setString("Press R to restart level");

                //Menu
                menu.setFont(font);
                menu.setCharacterSize(30);
                menu.setPosition(300.f, 400.f);
                menu.setString("Press M to enter Main Menu");

                //Exit
                exit.setFont(font);
                exit.setCharacterSize(30);
                exit.setPosition(300.f, 500.f);
                exit.setString("Press Esc to exit");

                while (pause.isOpen())
                {
                    Event evnt;
                    while (pause.pollEvent(evnt))
                    {
                        if (evnt.type == Event::Closed)
                        {
                            return 0;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "1" << endl;
                            res.close();
                            ofstream level1("Level 1.txt");
                            level1 << highscore << endl;
                            level1 << score << endl;
                            level1 << pl.getPosition().x << endl;
                            level1 << pl.getPosition().y << endl;
                            level1 << to_string(time.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 17; j++)
                                {
                                    level1 << collectibles[i][j];
                                }
                                level1 << endl;
                            }
                            level1.close();
                            gameplay.close();
                            pause.close();
                            return false;
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
                        {
                            // Accumulate pause time and close pause menu
                            pauseTime += pauseClock.getElapsedTime();
                            //pauseClock.restart();
                            pause.close();
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::R))
                        {
                            pl.setPosition(55.f, 540.f);
                            clock.restart();
                            pause.close();
                            pauseClock.restart();
                            resume1 = false;
                            score = 0;

                            //Re-initialize collectbles

                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 17; j++)
                                {
                                    if (collectibles[i][j] == 'z')
                                    {
                                        collectibles[i][j] = 'c';
                                    }
                                }
                            }

                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::M))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "1" << endl;
                            res.close();
                            ofstream level1("Level 1.txt");
                            level1 << highscore << endl;
                            level1 << score << endl;
                            level1 << pl.getPosition().x << endl;
                            level1 << pl.getPosition().y << endl;
                            level1 << to_string(time.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 17; j++)
                                {
                                    level1 << collectibles[i][j];
                                }
                                level1 << endl;
                            }
                            level1.close();
                            gameplay.close();
                            pause.close();
                            shouldBreak = MainMenu();
                            break;
                        }
                    }

                    if (shouldBreak == true)
                    {
                        break;
                    }

                    pause.clear();
                    pause.draw(background);
                    pause.draw(paused);
                    pause.draw(resumeT);
                    pause.draw(restart);
                    pause.draw(menu);
                    pause.draw(exit);
                    pause.display();
                }
            }

        }
    }

}

bool Level2()
{
    while (moveToMenu == true)

    {


        bool shouldBreak = false;
        float startX = 0, startY = 0;

        //Pause time
        Time pauseTime = Time::Zero; //Total accumulated pause time

        int keys = 0;
        int lives = 2;
        int playerscore = 0;
        const float mov_speed = 0.1f;
        RenderWindow window(VideoMode(c * tilesize, r * tilesize + 30.0f), "Maze Runner Level 2");
        window.setFramerateLimit(60);

        //Auto maximise
        window.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 39));
        window.setPosition(Vector2i(-7, -33));


        //------------------------------------------------
        //------------------------------------------------
        // fire logic initializations
        // Fire effects at different positions
        vector<FireEffect> fireEffects = {
            FireEffect(Vector2f(19 * tilesize + 1.5f, 2 * tilesize + 10.f), 0.04f),
            FireEffect(Vector2f(11 * tilesize + 1.5f, 11 * tilesize + 10.f), 0.04f),
            FireEffect(Vector2f(23 * tilesize + 1.5f, 21 * tilesize - 20.f), 0.04f),
            FireEffect(Vector2f(9 * tilesize + 1.5f, 19 * tilesize + 10.f), 0.04f)
        };

        // Predefined attributes
        float velocities[5][2] = {
            {50.0f, -10.0f}, {60.0f, -5.0f}, {70.0f, 0.0f}, {60.0f, 5.0f}, {50.0f, 10.0f} };
        Color colors[] = { Color::Red, Color::Red, Color::Red };
        const float fireOnDuration = 2.0f;  // Fire active for 2 seconds
        const float fireOffDuration = 2.0f; // No fire for 2 seconds


        // Clock for delta time
        Clock clock;

        //player
        /*CircleShape player;
        player.setRadius(tilesize / 2);
        player.setFillColor(Color::Black);*/
        //CircleShape player(tilesize / 2.4f); // Slightly smaller than a tile
        //player.setFillColor(sf::Color::Red);
        //player.setPosition(0, 0);

        //----------------------------------------------------------
        Texture player;
        player.loadFromFile("Textures/player.png");
        Sprite playerS;
        playerS.setTexture(player);
        const int frameWidth = 102; // image reso was 1841 * 2400 divided each by 4 as 4 by 4 spritesheet
        const int frameHeight = 153;
        IntRect currentFrame(0, 0, frameWidth, frameHeight); // taking bounds of sprite 
        playerS.setTextureRect(currentFrame);
        playerS.setPosition(0, 0); //player initial position 
        playerS.setScale((float)(3.5 * tilesize) / player.getSize().x, (float)(3.5 * tilesize) / player.getSize().y);// Initial position
        float animationTime = 0.1f;// time for each sprite 
        float elapsedAnimationTime = 0.0f; // max time they can take 
        int Column = 0;
        int Row = 0;


        //---------------------------------------------------------
        RectangleShape box(Vector2f(tilesize, tilesize));
        box.setOutlineColor(Color::Black);
        box.setOutlineThickness(2);
        readdata2D();


        RectangleShape maze2[r][c];
        Texture grass;
        grass.loadFromFile("Textures/blood.jpg");
        RectangleShape grassb(Vector2f(tilesize, tilesize));
        grassb.setTexture(&grass);

        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < c; j++)
            {
                maze2[i][j].setSize(Vector2f(tilesize, tilesize));
                if (arr[i][j] == 'a') {
                    maze2[i][j] = box;
                    maze2[i][j].setFillColor(Color(105, 105, 105));
                    maze2[i][j].setTexture(nullptr);
                }
                else {
                    maze2[i][j] = grassb;
                    maze2[i][j].setOutlineColor(Color::Black);
                    maze2[i][j].setOutlineThickness(2);
                }
                maze2[i][j].setPosition(j * tilesize, i * tilesize);
            }

        }


        //---------------------------------------------------------
        //background image 
        /*Texture grass;
        grass.loadFromFile("Textures/blood.jpg");
        RectangleShape grassb;
        grassb.setTexture(&grass);
        grassb.setScale((float)(c * tilesize) / grass.getSize().x, (float)(r * tilesize) / grass.getSize().y);*/

        //-----------------------------------------------
        Texture net;
        net.loadFromFile("Textures/spider.png");
        vector<Sprite>Spidernet;
        vector<Vector2f>netpos = {
            {22 * tilesize + 10, 1 * tilesize - 9},
            {15 * tilesize, 1 * tilesize},
            {1 * tilesize, 4 * tilesize},
            {16 * tilesize, 15 * tilesize},
            {1 * tilesize, 12 * tilesize},
        };
        for (const auto& position : netpos) {
            Sprite Spidern;
            Spidern.setTexture(net);
            Spidern.setScale((float)(14 * tilesize) / net.getSize().x, (float)(14 * tilesize) / net.getSize().y);
            Spidern.setPosition(position);
            Spidernet.push_back(Spidern);
        }
        //---------------------------------------------------
        //----------------------------------------------------
        // sound of ghosts

        SoundBuffer buffer;
        buffer.loadFromFile("Sound/boo.mp3");
        Sound sound;
        sound.setBuffer(buffer);
        bool soundplay = false;

        //sound of fire
        SoundBuffer bufferfire;
        bufferfire.loadFromFile("Sound/fire.mp3");
        Sound soundfire;
        soundfire.setBuffer(bufferfire);
        bool soundplayfire = false;
        //Clock deltafiretime;
        //bool soundplay = false;

        //sound of walking
        SoundBuffer bufferwalk;
        bufferwalk.loadFromFile("Sound/walk.mp3");
        Sound soundwalk;
        soundwalk.setBuffer(bufferwalk);

        //sound of keys
        SoundBuffer buffercollect;
        buffercollect.loadFromFile("Sound/collect.mp3");
        Sound soundcollect;
        soundcollect.setBuffer(buffercollect);

        //sound of skullcoin
        SoundBuffer buffercoin;
        buffercoin.loadFromFile("Sound/coin.mp3");
        Sound soundcoin;
        soundcoin.setBuffer(buffercoin);

        //sound of dying
        SoundBuffer bufferdie;
        bufferdie.loadFromFile("Sound/die.mp3");
        Sound sounddie;
        sounddie.setBuffer(bufferdie);

        SoundBuffer pauseB;
        pauseB.loadFromFile("audio/pause.mp3");
        Sound pauseS;
        pauseS.setBuffer(pauseB);



        //-----------------------------------------------------------
        //------------------------------------------------------





        /*vector<Vector2f>keypos = {
            {10 * tilesize - 3, 19 * tilesize - 3},
            {18 * tilesize - 3, 3 * tilesize - 3},
            {28 * tilesize - 3, 1 * tilesize - 3}
        };
        for (const auto& kpos : keypos) {
            Sprite keyb;
            keyb.setTexture(key);
            keyb.setScale((float)(1.1 * tilesize) / key.getSize().x, (float)(1.1 * tilesize) / key.getSize().y);
            keyb.setPosition(kpos);
            keyss.push_back(keyb);
        }*/


        //-------------------------------------------------------
        //-------------------------------------------------------
        Texture spookyhands;
        spookyhands.loadFromFile("Textures/hands.png");
        vector<Sprite>hands;
        vector<Vector2f>handpos = {
        {17 * tilesize, 17 * tilesize},
        {3 * tilesize, 9 * tilesize},
        {24 * tilesize, 1 * tilesize}
        };

        for (const auto& pos : handpos) {
            Sprite handsn;
            handsn.setTexture(spookyhands);
            handsn.setScale((float)(3 * tilesize) / spookyhands.getSize().x, (float)(3 * tilesize) / spookyhands.getSize().y);
            handsn.setPosition(pos);
            hands.push_back(handsn);
        }

        //-------------------------------------------
        Texture skull2;
        skull2.loadFromFile("Textures/skull3.png");
        Sprite skull2b;
        skull2b.setTexture(skull2);
        skull2b.setPosition(15 * tilesize, 1 * tilesize);
        skull2b.setScale((float)(4 * tilesize) / skull2.getSize().x, (float)(4 * tilesize) / skull2.getSize().y);

        //-------------------------------------------
        Texture ghost;
        ghost.loadFromFile("Textures/spooky.png");
        Sprite ghostb;
        ghostb.setTexture(ghost);
        ghostb.setPosition(-200, -200);
        ghostb.setScale((float)(6 * tilesize) / ghost.getSize().x, (float)(6 * tilesize) / ghost.getSize().y);

        //text 
        //------------------------------------------
        //time
        Font font;
        font.loadFromFile("Fonts/ArialTh.ttf");
        Text text;
        text.setFont(font);
        text.setFillColor(Color::White);
        text.setCharacterSize(14);
        text.setStyle(Text::Bold);
        text.setPosition(25 * tilesize, 25 * tilesize - 1);
        //---------------------------------------
        //live
        Text text2;
        text2.setFont(font);
        text2.setFillColor(Color::White);
        text2.setCharacterSize(14);
        text2.setStyle(Text::Bold);
        text2.setPosition(1 * tilesize, 25 * tilesize - 1);

        //-------------------------------------------------
        //score
        Text text3;
        text3.setFont(font);
        text3.setFillColor(Color::White);
        text3.setCharacterSize(14);
        text3.setStyle(Text::Bold);
        text3.setPosition(8 * tilesize, 25 * tilesize - 1);
        //---------------------------------------------------
        //message
        Text text4;
        text4.setFont(font);
        text4.setFillColor(Color::White);
        text4.setCharacterSize(30);
        text4.setStyle(Text::Bold);
        text4.setPosition(3 * tilesize, 12 * tilesize + 2);

        //---------------------------------------------------
        //key display
        Text text5;
        text5.setFont(font);
        text5.setFillColor(Color::White);
        text5.setCharacterSize(14);
        text5.setStyle(Text::Bold);
        text5.setPosition(18 * tilesize, 25 * tilesize - 1);

        //---------------------------------------------------
        Clock timer;




        //Resume functionality
        float prevTime = 0;
        string highscore = "0";

        string line;

        ifstream levels2("Level 2.txt");
        levels2 >> highscore;

        //COLLECTIBLES



        char collectibles[r][c];

        char keyss[r][c];

        if (resume2 == true)
        {
            levels2 >> playerscore; // Load score
            levels2 >> lives;  // Load lives
            levels2 >> startX; // Load player X position
            levels2 >> startY; // Load player Y position
            levels2 >> prevTime; // Load previous time

            // Load the maze array
            for (int i = 0; i < r; i++)
            {
                for (int j = 0; j < c; j++)
                {
                    levels2 >> collectibles[i][j];
                }
            }
            for (int i = 0; i < r; i++)
            {
                for (int j = 0; j < c; j++)
                {
                    levels2 >> keyss[i][j];
                }
            }
            playerS.setPosition(startX, startY);
        }
        else
        {
            ifstream cFile("collectibles.txt");
            for (int i = 0;i < r;i++)
            {
                for (int j = 0;j < c;j++)
                {
                    cFile >> collectibles[i][j];
                }
            }
            cFile.close();

            ifstream kFile("keys.txt");
            for (int i = 0;i < r;i++)
            {
                for (int j = 0;j < c;j++)
                {
                    kFile >> keyss[i][j];
                }
            }
            kFile.close();

        }
        levels2.close();


        //-----------------------------------------------
        //-----------------------------------------------

        //COINS

        Texture skullcoins;
        skullcoins.loadFromFile("Textures/skullcoin2.png");

        RectangleShape skullC(Vector2f(tilesize, tilesize));
        skullC.setTexture(&skullcoins);

        RectangleShape Scoin[r][c];

        for (int i = 0;i < r;i++)
        {
            for (int j = 0;j < c;j++)
            {
                if (collectibles[i][j] == 'c')
                {
                    Scoin[i][j] = skullC;
                    Scoin[i][j].setPosition(j * tilesize - 3, i * tilesize - 3);
                }
            }
        }

        //KEYS


        Texture keyT;
        keyT.loadFromFile("Textures/key.png");

        RectangleShape key(Vector2f(tilesize, tilesize));
        key.setTexture(&keyT);

        RectangleShape keyArr[r][c];

        for (int i = 0;i < r;i++)
        {
            for (int j = 0;j < c;j++)
            {
                if (keyss[i][j] == 'k')
                {
                    keyArr[i][j] = key;
                    keyArr[i][j].setPosition(j * tilesize - 3, i * tilesize - 3);
                }
            }
        }


        /*Vector2f coinpos[11] = {
            {16 * tilesize - 3, 12 * tilesize - 3},
            {6 * tilesize - 3, 6 * tilesize - 3},
            {1 * tilesize - 3, 12 * tilesize - 3},
            {1 * tilesize - 3, 21 * tilesize - 3},
            {9 * tilesize - 3, 14 * tilesize - 3},
            {23 * tilesize - 3, 22 * tilesize - 3},
            {18 * tilesize - 3, 6 * tilesize - 3},
            {9 * tilesize - 3, 23 * tilesize - 3},
            {10 * tilesize - 3, 4 * tilesize - 3},
            {28 * tilesize - 3, 12 * tilesize - 3},
            {25 * tilesize - 3, 6 * tilesize - 3}
        };*/






        //for (const auto& pos : coinpos) {
        //    Sprite coin;
        //    coin.setTexture(skullcoins);
        //    coin.setScale((float)(1.3 * tilesize) / skullcoins.getSize().x, (float)(1.3 * tilesize) / skullcoins.getSize().y);
        //    collectibles[(int)((pos.y + 3) / tilesize)][(int)((pos.x + 3) / tilesize)] = 'c';
        //    for (int i = 0; i < r; i++)
        //    {
        //        for (int j = 0; j < c; j++)
        //        {
        //            if (collectibles[i][j] == 'c')
        //            {
        //                coin.setPosition(pos);
        //            }
        //            else if (collectibles[i][j] == 'a')
        //                coin.setPosition(-100, -100);
        //        }
        //    }
        //    //coin.setPosition(pos);
        //    //collectibles[(int)((pos.y + 3) / tilesize)][(int)((pos.x + 3) / tilesize)] = 'c';
        //    coins.push_back(coin);
        //}







        //------------------------------------------------
        //------------------------------------------------
        while (window.isOpen()) {

            //-----------------------------------------------------------
            //------------------------------------------------------------
            //adding time + text and font
            //float deltafiretime = clock.restart().asSeconds();
            float dt = clock.restart().asSeconds();
            Time elapsed = timer.getElapsedTime();
            float timevalue = round(elapsed.asSeconds() * 100.0f) / 100.0f;
            string stringn = to_string(timevalue + prevTime);
            text.setString("Time: " + stringn);
            string lives1 = to_string(lives);
            text2.setString("Lives: " + lives1);
            string score = to_string(playerscore);
            text3.setString("Score:" + score);
            string keystr = to_string(keys);
            text5.setString("Keys:" + keystr);

            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                {
                    ofstream res("resume.txt");
                    res << "1" << endl;
                    res << "2" << endl;
                    res.close();
                    ofstream level2("Level 2.txt");
                    level2 << highscore << endl;
                    level2 << playerscore << endl;
                    level2 << lives << endl;
                    level2 << playerS.getPosition().x << endl;
                    level2 << playerS.getPosition().y << endl;
                    level2 << to_string(elapsed.asSeconds() + prevTime) << endl;
                    for (int i = 0; i < r; i++)
                    {
                        for (int j = 0; j < c; j++)
                        {
                            level2 << collectibles[i][j];
                        }
                        level2 << endl;
                    }
                    for (int i = 0; i < r; i++)
                    {
                        for (int j = 0; j < c; j++)
                        {
                            level2 << keyss[i][j];
                        }
                        level2 << endl;
                    }
                    level2.close();
                    window.close();
                    return false;
                }
            }

            //------------------------------------------------------------------------------
            //-----------------------------------------------------------------------------
            // Update fire effects logic 
            for (auto& fire : fireEffects) {
                fire.timeSinceLastSpawn += dt;
                fire.intervalTimer += dt;

                // Toggle fire on/off based on the interval
                if (fire.fireActive && fire.intervalTimer >= fireOnDuration) {
                    fire.fireActive = false; // Turn fire off
                    fire.intervalTimer = 0; // Reset the timer
                }
                else if (!fire.fireActive && fire.intervalTimer >= fireOffDuration) {
                    fire.fireActive = true; // Turn fire on
                    fire.intervalTimer = 0; // Reset the timer
                }

                // Spawn new particles only if fire is active
                if (fire.fireActive && fire.timeSinceLastSpawn >= fire.spawnRate) {
                    fire.timeSinceLastSpawn = 0;

                    if (fire.particles.size() < 500) { // Limit max particles
                        static int velocityIndex = 0, colorIndex = 0;

                        // Create a new particle
                        Particle p;
                        p.shape.setRadius(7.0f); // Fixed size
                        p.shape.setFillColor(colors[colorIndex]); // Cycling through predefined colors
                        p.shape.setPosition(fire.position); // Spawn at the fire's position
                        p.velocity = Vector2f(velocities[velocityIndex][0], velocities[velocityIndex][1]); // Predefined velocities
                        p.lifetime = 2.0f; // Fixed lifetime

                        // Add the particle to the vector
                        fire.particles.push_back(p);

                        // Cycle through predefined attributes
                        velocityIndex = (velocityIndex + 1) % 5; // Cycle through 5 velocities
                        colorIndex = (colorIndex + 1) % 3;       // Cycle through 3 colors
                    }
                }
                //----------------------------------------------------------------------------------
                //----------------------------------------------------------------------------------

                ////collsion with coins 
                //for (auto& skullcoins : coins) {
                //    if (playerS.getGlobalBounds().intersects(skullcoins.getGlobalBounds()) &&
                //        collectibles[(int)((skullcoins.getPosition().y + 3) / tilesize)][(int)((skullcoins.getPosition().x + 3) / tilesize)] == 'c')
                //    {

                //        collectibles[(int)((skullcoins.getPosition().y + 3) / tilesize)][(int)((skullcoins.getPosition().x + 3) / tilesize)] = 'a';
                //        playerscore += 100;
                //        soundcoin.play();
                //        skullcoins.setPosition(-100, -100);
                //    }
                //}

                int playerX = (playerS.getPosition().x + 3) / tilesize;
                int playerY = (playerS.getPosition().y + 3) / tilesize;

                if (collectibles[playerY][playerX] == 'c')
                {
                    playerscore += 100;
                    collectibles[playerY][playerX] = 'a';
                    soundcoin.play();
                }
                else if (keyss[playerY][playerX] == 'k')
                {
                    keyss[playerY][playerX] = 'n';
                    soundcollect.play();
                    keys++;
                }


                /*for (auto& key : keyss) {
                    if (playerS.getGlobalBounds().intersects(key.getGlobalBounds())) {
                        key.setPosition(-100, -100);
                        keys++;
                        soundcollect.play();
                    }
                }*/


                //----------------------------------------------------------------------------------
                //----------------------------------------------------------------------------------

                // Update particles
                for (size_t i = 0; i < fire.particles.size();) {
                    Particle& p = fire.particles[i];
                    p.lifetime -= dt;

                    // Gradually fade color
                    Color currentColor = p.shape.getFillColor();

                    // This part focuses on fading the color
                    if (currentColor.a > 0) {
                        float newAlpha = currentColor.a - (255 * dt);
                        newAlpha = max(0.0f, newAlpha);
                        currentColor.a = newAlpha;
                    }
                    p.shape.setFillColor(currentColor);

                    // Move the particle
                    p.shape.move(p.velocity * dt);

                    // Remove if lifetime is over or fully faded
                    if (p.lifetime <= 0 || (currentColor.a == 0)) {
                        fire.particles.erase(fire.particles.begin() + i);
                    }
                    else {
                        ++i; // Only increment if particle is not removed
                    }

                    if (playerS.getGlobalBounds().intersects(p.shape.getGlobalBounds())) {
                        sounddie.play();
                        playerscore = playerscore - 100;
                        if (lives == 0)
                        {
                            SoundBuffer gameoverB;
                            Sound gameoverS;
                            gameoverB.loadFromFile("audio/gameover.mp3");
                            gameoverS.setBuffer(gameoverB);
                            cout << "YOU DIED! :(" << endl;
                            gameoverS.play();

                            gameOver = true;

                            RenderWindow endgame(VideoMode(1015, 595), "GameOver", Style::Default);
                            endgame.setFramerateLimit(60);

                            endgame.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 35));
                            endgame.setPosition(Vector2i(-7, -40));

                            RectangleShape background(Vector2f(1015.f, 595.f));
                            Texture bg;
                            bg.loadFromFile("background2.jpg");
                            background.setTexture(&bg);

                            Text gameover, menu, exit, restart;

                            //Gameover
                            gameover.setFont(font);
                            gameover.setCharacterSize(150);
                            gameover.setPosition(235.f, 50.f);
                            gameover.setString("GAME OVER");

                            //Restart
                            restart.setFont(font);
                            restart.setCharacterSize(30);
                            restart.setPosition(365.f, 250.f);
                            restart.setString("Press R to restart level");

                            //Menu
                            menu.setFont(font);
                            menu.setCharacterSize(30);
                            menu.setPosition(365.f, 350.f);
                            menu.setString("Press M to enter Main Menu");

                            //Exit
                            exit.setFont(font);
                            exit.setCharacterSize(30);
                            exit.setPosition(365.f, 450.f);
                            exit.setString("Press Esc to exit");

                            while (endgame.isOpen())
                            {
                                Event evnt;;
                                while (endgame.pollEvent(evnt))
                                {
                                    if (evnt.type == Event::Closed)
                                    {
                                        ofstream res("resume.txt");
                                        res << "0" << endl;
                                        res << "0" << endl;
                                        res.close();
                                        ofstream level2("Level 2.txt");
                                        level2 << highscore << endl;
                                        level2.close();
                                        endgame.close();
                                        return false;
                                    }

                                }
                                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                                {
                                    ofstream res("resume.txt");
                                    res << "0" << endl;
                                    res << "0" << endl;
                                    res.close();
                                    ofstream level2("Level 2.txt");
                                    level2 << highscore << endl;
                                    level2.close();
                                    gameOver = true;
                                    endgame.close();
                                    return false;
                                }
                                else if (Keyboard::isKeyPressed(Keyboard::Key::R))
                                {
                                    gameOver = false;
                                    lives = 3;
                                    playerS.setPosition(0.f, 0.f);
                                    timer.restart();
                                    endgame.close();
                                    resume3 = false;
                                    playerscore = 0;

                                    //Re-initialize collectbles

                                    for (int i = 0; i < 17; i++)
                                    {
                                        for (int j = 0; j < 29; j++)
                                        {
                                            if (arr[i][j] == 'z')
                                            {
                                                arr[i][j] = 'f';
                                            }
                                        }
                                    }

                                }
                                else if (Keyboard::isKeyPressed(Keyboard::Key::M))
                                {
                                    ofstream res("resume.txt");
                                    res << "0" << endl;
                                    res << "0" << endl;
                                    res.close();
                                    ofstream level2("Level 2.txt");
                                    level2 << highscore << endl;
                                    level2.close();
                                    endgame.close();
                                    window.close();
                                    resume2 = false;
                                    shouldBreak = MainMenu();
                                    moveToMenu = true;
                                    break;
                                }
                                endgame.clear();
                                endgame.draw(background);
                                endgame.draw(gameover);
                                endgame.draw(restart);
                                endgame.draw(menu);
                                endgame.draw(exit);
                                endgame.display();
                            }
                        }

                        if (shouldBreak == true)
                        {
                            break;
                        }
                        else {
                            lives--;
                            playerS.setPosition(0, 0);
                        }
                    }
                }
            }
            //
            //-------------------------------------------------------------------------------------
            //-------------------------------------------------------------------------------------
            //movement + collsion + player animation logic 
            bool ani_movement = false;
            Vector2f PlayerCurrentPosition = playerS.getPosition(); // Storing in next location because we need current posiion value later
            Vector2f PLayernextPosition = PlayerCurrentPosition;

            if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
                PLayernextPosition.y -= tilesize * mov_speed;
                Row = 3;
                ani_movement = true;
                //soundwalk.setLoop(true);
                soundwalk.play();

            }
            if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
                PLayernextPosition.y += tilesize * mov_speed;
                Row = 0;
                ani_movement = true;
                //soundwalk.setLoop(true);
                soundwalk.play();
            }
            if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) {
                PLayernextPosition.x -= tilesize * mov_speed;
                Row = 1;
                ani_movement = true;
                //soundwalk.setLoop(true);
                soundwalk.play();
            }
            if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
                PLayernextPosition.x += tilesize * mov_speed;
                Row = 2;
                ani_movement = true;
                //soundwalk.setLoop(true);
                soundwalk.play();
            }
            //updating the position now
           //------------------------------------------------------------------------------
           //Checking for screen collision here  

            if (PLayernextPosition.x < 0)
                PLayernextPosition.x = 0;
            else if (PLayernextPosition.x > (c - 1) * tilesize)
                PLayernextPosition.x = (c - 1) * tilesize;
            if (PLayernextPosition.y < 0)
                PLayernextPosition.y = 0;
            else if (PLayernextPosition.y > (r - 1) * tilesize)
                PLayernextPosition.y = (r - 1) * tilesize;

            playerS.setPosition(PLayernextPosition);


            if (checkCollisionwithwalls(playerS.getGlobalBounds())) {
                playerS.setPosition(PlayerCurrentPosition); // if flag came back true then this gives the current position value to set position of player
            }

            //--------------------------------------------------------------------------
            //-------------------------------------------------------------------------
            //---------------------------------------------------------------------------
            // player winning logic
            Vector2f WINLOC;
            WINLOC.x = 29 * tilesize;
            WINLOC.y = 24 * tilesize;
            if (playerS.getPosition() == WINLOC) {
                if (keys == 3) {
                    cout << "You won";
                    cout << "Your score is " << playerscore;
                    return true;
                }
                else {
                    text4.setString("YOU NEED 3 KEYS TO GO TO NEXT LEVEL");
                }
            }

            /*if (player.getGlobalBounds().intersects(Coinb.getGlobalBounds())) {
                Coinb.setPosition(-100, -100);
                playerscore += 100;
            }*/

            //---------------------------------------------------------------------------
            //--------------------------------------------------------------------------

            //-----------------------------------------------------------------------------
            // -------------------------------------------------------------------------------
            // Animation logic
            if (ani_movement) {
                elapsedAnimationTime += dt;
                if (elapsedAnimationTime >= animationTime) {
                    // Advance to the next frame
                    elapsedAnimationTime = 0.0f;
                    Column = (Column + 1) % 4; // Loop through columns (4 frames per row)
                    currentFrame = IntRect(Column * frameWidth, Row * frameHeight, frameWidth, frameHeight);
                    playerS.setTextureRect(currentFrame);
                }
            }
            else {

                Column = 0;
                currentFrame = IntRect(Column * frameWidth, Row * frameHeight, frameWidth, frameHeight);
                playerS.setTextureRect(currentFrame);
            }

            //-------------------------------------------------------------------------
            //--------------------------------------------------------------------------
            //drawing things 
            window.clear(Color::Black);
            //
            //-------------------------------------------------------------
            //-----------------------------------------------------------
            //all drawings 
            //MazeMaking(window, grass);

            for (int i = 0; i < r; i++)
            {
                for (int j = 0; j < c; j++)
                {
                    window.draw(maze2[i][j]);
                    if (arr[i][j] == 'a')
                    {
                        window.draw(box);
                    }
                }
            }

            for (const auto& net : Spidernet) {
                window.draw(net);
            }
            for (const auto& spookyhands : hands) {
                window.draw(spookyhands);
            }
            //soundplayfire = false;
            for (const auto& fire : fireEffects) {
                //soundplayfire = false;
                for (const auto& p : fire.particles) {
                    window.draw(p.shape);
                    if (soundplayfire == false) {
                        soundfire.setLoop(true);
                        soundfire.play();
                        soundplayfire = true;
                    }
                }

            }
            /*for (const auto& skullcoins : coins) {

                for (int i = 0; i < r; i++)
                {
                    for (int j = 0; j < c; j++)
                    {
                        if (collectibles[i][j] == 'c')
                            window.draw(skullcoins);
                    }
                }
            }*/

            for (int i = 0;i < r;i++)
            {
                for (int j = 0;j < c;j++)
                {
                    if (collectibles[i][j] == 'c')
                    {
                        window.draw(Scoin[i][j]);
                    }
                }
            }

            for (int i = 0;i < r;i++)
            {
                for (int j = 0;j < c;j++)
                {
                    if (keyss[i][j] == 'k')
                    {
                        window.draw(keyArr[i][j]);
                    }
                }
            }

            /*for (const auto& key : keyss) {
                window.draw(key);
            }*/
            //-------------------------------------------------------------
            //-------------------------------------------------------------
            //Ghost reappearence logic
            bool soundplay = false;
            if (playerS.getPosition().x >= 21.0 * tilesize && playerS.getPosition().x <= 22.9 * tilesize && (playerS.getPosition().y >= 23.0 * tilesize && playerS.getPosition().y <= 23.9 * tilesize)) {
                ghostb.setPosition(22 * tilesize, 19 * tilesize);
                window.draw(ghostb);
                if (soundplay == false) {
                    sound.play();
                    soundplay = true;
                }
                else
                    soundplay = false;
            }
            if (playerS.getPosition().x >= 5.0 * tilesize && playerS.getPosition().x <= 6.9 * tilesize && (playerS.getPosition().y >= 7.0 * tilesize && playerS.getPosition().y <= 7.9 * tilesize)) {
                ghostb.setPosition(4 * tilesize, 4 * tilesize);
                window.draw(ghostb);
                if (soundplay == false) {
                    sound.play();
                    soundplay = true;
                }
                else
                    soundplay = false;
            }
            if (playerS.getPosition().x >= 14.0 * tilesize && playerS.getPosition().x <= 15.9 * tilesize && (playerS.getPosition().y >= 17.0 * tilesize && playerS.getPosition().y <= 17.9 * tilesize)) {
                ghostb.setPosition(14 * tilesize, 13 * tilesize);
                window.draw(ghostb);
                if (soundplay == false) {
                    sound.play();
                    soundplay = true;
                }
                else
                    soundplay = false;
            }
            if (playerS.getPosition().x >= 26.0 * tilesize && playerS.getPosition().x <= 26.9 * tilesize && (playerS.getPosition().y >= 4 * tilesize && playerS.getPosition().y <= 4.9 * tilesize)) {
                ghostb.setPosition(25 * tilesize, 2 * tilesize);
                window.draw(ghostb);
                if (soundplay == false) {
                    sound.play();
                    soundplay = true;
                }
                else
                    soundplay = false;
            }

            if (playerS.getPosition().x >= 9.0 * tilesize && playerS.getPosition().x <= 9.9 * tilesize && (playerS.getPosition().y >= 21.0 * tilesize && playerS.getPosition().y <= 21.9 * tilesize)) {
                ghostb.setPosition(9 * tilesize, 18 * tilesize);
                window.draw(ghostb);
                if (soundplay == false) {
                    sound.play();
                    soundplay = true;
                }
                else
                    soundplay = false;
            }
            //-------------------------------------------------------------------------
            //--------------------------------------------------------------------------
            window.draw(text);
            window.draw(skull2b);
            window.draw(text2);
            window.draw(text3);
            if (playerS.getPosition().x == 29 * tilesize && playerS.getPosition().y == 24 * tilesize) {
                window.draw(text4);
            }
            window.draw(text5);
            window.draw(playerS);

            window.display();

            //Pause game

            if (Keyboard::isKeyPressed(Keyboard::Key::P))
            {
                pauseS.play();

                Clock pauseClock;

                RenderWindow pause(VideoMode(1015, 595), "Paused", Style::Default);
                pause.setFramerateLimit(60);

                pause.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 35));
                pause.setPosition(Vector2i(-7, -40));

                RectangleShape background(Vector2f(1015.f, 595.f));
                Texture bg;
                bg.loadFromFile("background.jpg");
                background.setTexture(&bg);

                Text paused, resumeT, menu, exit, restart;

                //Paused
                paused.setFont(font);
                paused.setCharacterSize(100);
                paused.setPosition(235.f, 50.f);
                paused.setString("Game Paused");

                //Resume
                resumeT.setFont(font);
                resumeT.setCharacterSize(30);
                resumeT.setPosition(300.f, 200.f);
                resumeT.setString("Press Enter to resume level");

                //Restart
                restart.setFont(font);
                restart.setCharacterSize(30);
                restart.setPosition(300.f, 300.f);
                restart.setString("Press R to restart level");

                //Menu
                menu.setFont(font);
                menu.setCharacterSize(30);
                menu.setPosition(300.f, 400.f);
                menu.setString("Press M to enter Main Menu");

                //Exit
                exit.setFont(font);
                exit.setCharacterSize(30);
                exit.setPosition(300.f, 500.f);
                exit.setString("Press Esc to exit");

                while (pause.isOpen())
                {
                    Event evnt;
                    while (pause.pollEvent(evnt))
                    {
                        if (evnt.type == Event::Closed)
                        {
                            return 0;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "2" << endl;
                            res.close();
                            ofstream level2("Level 2.txt");
                            level2 << highscore << endl;
                            level2 << playerscore << endl;
                            level2 << lives << endl;
                            level2 << playerS.getPosition().x << endl;
                            level2 << playerS.getPosition().y << endl;
                            level2 << to_string(elapsed.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    level2 << collectibles[i][j];
                                }
                                level2 << endl;
                            }
                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    level2 << keyss[i][j];
                                }
                                level2 << endl;
                            }
                            level2.close();
                            window.close();
                            pause.close();
                            return false;
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
                        {
                            // Accumulate pause time and close pause menu
                            pauseTime += pauseClock.getElapsedTime();
                            //pauseClock.restart();
                            pause.close();
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::R))
                        {
                            gameOver = false;
                            lives = 2;
                            playerS.setPosition(0.f, 0.f);
                            timer.restart();
                            pause.close();
                            pauseClock.restart();
                            resume3 = false;
                            playerscore = 0;

                            //Re-initialize collectbles

                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    if (collectibles[i][j] == 'a')
                                    {
                                        collectibles[i][j] = 'c';
                                    }
                                }
                            }

                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    if (keyss[i][j] == 'n')
                                    {
                                        keyss[i][j] = 'k';
                                    }
                                }
                            }

                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::M))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "2" << endl;
                            res.close();
                            ofstream level2("Level 2.txt");
                            level2 << highscore << endl;
                            level2 << playerscore << endl;
                            level2 << lives << endl;
                            level2 << playerS.getPosition().x << endl;
                            level2 << playerS.getPosition().y << endl;
                            level2 << to_string(elapsed.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    level2 << collectibles[i][j];
                                }
                                level2 << endl;
                            }
                            for (int i = 0; i < r; i++)
                            {
                                for (int j = 0; j < c; j++)
                                {
                                    level2 << keyss[i][j];
                                }
                                level2 << endl;
                            }
                            level2.close();
                            window.close();
                            pause.close();
                            shouldBreak = MainMenu();
                            break;
                        }
                    }

                    if (shouldBreak == true)

                    {
                        break;
                    }

                    pause.clear();
                    pause.draw(background);
                    pause.draw(paused);
                    pause.draw(resumeT);
                    pause.draw(restart);
                    pause.draw(menu);
                    pause.draw(exit);
                    pause.display();
                }
            }
        }

    }
}

bool Level3()
{


    while (moveToMenu == true)
    {
        gameOver = false;
        bool shouldBreak = false;

        //Font
        Font font;
        font.loadFromFile("KnightWarrior.otf");

        RenderWindow gameplay(VideoMode(1160, 680), "The Maze Runner : Level 3", Style::Default);
        gameplay.setFramerateLimit(60);

        //Auto maximise
        gameplay.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 39));
        gameplay.setPosition(Vector2i(-7, -33));

        //Audio
        SoundBuffer bgB, fireB, sizzleB, spikeB, jumpB, gameoverB, pauseB, c;

        bgB.loadFromFile("audio/melancholia.mp3");
        fireB.loadFromFile("audio/fire.mp3");
        sizzleB.loadFromFile("audio/sizzle.mp3");
        spikeB.loadFromFile("audio/spike.mp3");
        jumpB.loadFromFile("audio/jump2.mp3");
        gameoverB.loadFromFile("audio/gameover.mp3");
        pauseB.loadFromFile("audio/pause.mp3");
        c.loadFromFile("audio/coin.mp3");


        Sound bgS, fireS, sizzleS, spikeS, jumpS, gameoverS, pauseS, coin;

        bgS.setBuffer(bgB);
        fireS.setBuffer(fireB);
        sizzleS.setBuffer(sizzleB);
        spikeS.setBuffer(spikeB);
        jumpS.setBuffer(jumpB);
        gameoverS.setBuffer(gameoverB);
        pauseS.setBuffer(pauseB);
        //coin.setBuffer(c);

        int score = 0;
        float startX = 37.5f, startY = 600.f;


        // Declarations

        int lives = 3;
        cout << "You have " << lives << " lives remaining." << endl;


        char arr[17][29];
        RectangleShape maze[17][29];
        RectangleShape collectible[17][29];



        //Sprites
        RectangleShape wall(Vector2f(40.f, 40.f)), floor(Vector2f(40.f, 40.f)), start(Vector2f(40.f, 40.f)), end(Vector2f(40.f, 40.f)),
            trap(Vector2f(40.f, 40.f)), background(Vector2f(1015, 595)), spikeTrap(Vector2f(40.f, 40.f)), torchF(Vector2f(40.f, 40.f)), flame(Vector2f(40.f, 40.f)), skull(Vector2f(40.f, 40.f));

        Sprite player;
        Texture pl;
        pl.loadFromFile("sprite.png");
        player.setTexture(pl);
        float height = 320, width = 212.75;
        player.setTextureRect(IntRect(0.f, height * 3, width, height));
        player.setScale(30 / width, 30 / height);


        // Set initial player position
        player.setPosition(startX, startY);

        //Set Textures
        Texture brick, grass, stFlag, endflag, fireTrap, bg1, spTrap, flameTexture, sk;
        Texture bg;
        bg.loadFromFile("bg3.jpg");
        brick.loadFromFile("black2.jpg");
        grass.loadFromFile("red.jpg");
        stFlag.loadFromFile("go.png");
        endflag.loadFromFile("endflag.png");
        fireTrap.loadFromFile("lava.jpeg");
        bg1.loadFromFile("cemetary.jpeg");
        spTrap.loadFromFile("spikey.png");
        flameTexture.loadFromFile("flame.jpg");
        sk.loadFromFile("skull.png");

        wall.setTexture(&brick);
        floor.setTexture(&grass);
        start.setTexture(&stFlag);
        end.setTexture(&endflag);
        trap.setTexture(&fireTrap);
        background.setTexture(&bg);
        spikeTrap.setTexture(&spTrap);
        flame.setTexture(&flameTexture);
        skull.setTexture(&sk);


        // Animation variables
        const int frameWidth = 94.8;   // Width of each frame
        const int frameHeight = 145;  // Height of each frame
        const int numFrames = 15;     // Total number of frames
        const int framesPerRow = 5;   // Number of frames in each row of the sprite sheet
        Time frameDuration = sf::milliseconds(100); // Time per frame

        int currentFrames[17][29] = { 0 };   // Raw array to track the current animation frame
        Clock animationClock;


        // Load maze from file
        ifstream file("maze3.txt");
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 29; j++)
            {
                file >> arr[i][j];
            }
        }
        file.close();


        // Initialize maze tiles
        for (int i = 0; i < 17; i++)
        {
            for (int j = 0; j < 29; j++)
            {
                if (arr[i][j] == 'w')
                {
                    maze[i][j] = wall;
                }
                else if (arr[i][j] == 'f')
                {
                    maze[i][j] = floor;
                    if (i % 2 == 0 || j % 2 == 0)
                    {
                        collectible[i][j] = skull;
                        collectible[i][j].setPosition(j * 40 + 10, i * 40 + 10);
                        collectible[i][j].setScale(0.5, 0.5);
                    }
                }
                else if (arr[i][j] == 's')
                {
                    maze[i][j] = start;
                }
                else if (arr[i][j] == 'e')
                {
                    maze[i][j] = end;
                }
                else if (arr[i][j] == 't')
                    maze[i][j] = trap;
                else if (arr[i][j] == 'g')
                    maze[i][j] = floor;
                else if (arr[i][j] == 'o')
                    maze[i][j] = flame;

                maze[i][j].setPosition(j * 40, i * 40);
            }
        }



        //Trap trackers
        int thisTrap = 0;
        bool spikes = false;

        //Jump variables

        bool isJumping = false;
        Vector2f startPos;
        Vector2f targetPos;
        float jumpProgress = 0.0f;
        bool ignoreSpike = false;
        int playerX;
        int playerY;

        //Clock timer
        Clock clock;
        Clock trapClock;
        Clock jumpClock;
        Time time;

        //Pause time
        Time pauseTime = Time::Zero; //Total accumulated pause time



        bgS.play();
        fireS.play();

        //Resume functionality
        float prevTime = 0;
        string highscore = "0";

        string line;

        ifstream levels3("Level 3.txt");
        levels3 >> highscore;

        if (resume3 == true)
        {
            levels3 >> score; // Load score
            levels3 >> lives;
            levels3 >> startX; // Load player X position
            levels3 >> startY; // Load player Y position
            levels3 >> prevTime; // Load previous time

            // Load the maze array
            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 29; j++)
                {
                    levels3 >> arr[i][j];
                }
            }
            player.setPosition(startX, startY);
        }
        levels3.close();


        //Main loop
        while (gameplay.isOpen() && gameOver == false)
        {
            Event evnt;
            while (gameplay.pollEvent(evnt))
            {

                if (evnt.type == Event::Closed)
                {
                    ofstream res("resume.txt");
                    res << "1" << endl;
                    res << "3" << endl;
                    res.close();
                    ofstream level3("Level 3.txt");
                    level3 << highscore << endl;
                    level3 << score << endl;
                    level3 << player.getPosition().x << endl;
                    level3 << player.getPosition().y << endl;
                    level3 << to_string(time.asSeconds() + prevTime) << endl;
                    for (int i = 0; i < 17; i++)
                    {
                        for (int j = 0; j < 29; j++)
                        {
                            level3 << arr[i][j];
                        }
                        level3 << endl;
                    }
                    level3.close();
                    gameplay.close();
                    return false;
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                {
                    ofstream res("resume.txt");
                    res << "1" << endl;
                    res << "3" << endl;
                    res.close();
                    ofstream level3("Level 3.txt");
                    level3 << highscore << endl;
                    level3 << score << endl;
                    level3 << lives << endl;
                    level3 << player.getPosition().x << endl;
                    level3 << player.getPosition().y << endl;
                    level3 << to_string(time.asSeconds() + prevTime) << endl;
                    for (int i = 0; i < 17; i++)
                    {
                        for (int j = 0; j < 29; j++)
                        {
                            level3 << arr[i][j];
                        }
                        level3 << endl;
                    }
                    level3.close();
                    gameplay.close();
                    return false;
                }
            }


            //Score and Lives
            Text scoreT, livesT;
            scoreT.setFont(font);
            scoreT.setString("Your score is : " + to_string(score));

            livesT.setFont(font);
            livesT.setString("Lives : " + to_string(lives));
            livesT.setPosition(500, 0);

            // Update animation for each flame
            if (animationClock.getElapsedTime() > frameDuration) {
                for (int row = 0; row < 17; ++row) {
                    for (int col = 0; col < 29; ++col) {
                        if (arr[row][col] == 'o' && (row == 0 || col == 0 || row == 16 || col == 28))
                        {
                            // Update the current frame
                            currentFrames[row][col] = (currentFrames[row][col] + 1) % numFrames;

                            // Calculate the texture rectangle
                            int x = (currentFrames[row][col] % framesPerRow) * frameWidth; // Column in the sprite sheet
                            int y = (currentFrames[row][col] / framesPerRow) * frameHeight; // Row in the sprite sheet
                            maze[row][col].setTextureRect(sf::IntRect(x, y, frameWidth, frameHeight));
                        }
                    }
                }
                animationClock.restart();
            }

            //Main game Timer

            time = clock.getElapsedTime() - pauseTime;

            Text timer;
            timer.setFont(font);
            timer.setString(to_string(time.asSeconds() + prevTime));
            timer.setPosition(1000.f, 0.f);



            // Player movement flags
            bool left = true, right = true, up = true, down = true;


            // Collision detection + Stop movement altogether
            for (int i = 0; i < 17; i++) {
                for (int j = 0; j < 29; j++) {
                    if (arr[i][j] == 'w' || arr[i][j] == 'o') {  // Check for walls or obstacles
                        // Wall positions
                        float wallLeft = maze[i][j].getPosition().x + 5;
                        float wallRight = wallLeft + 30;
                        float wallTop = maze[i][j].getPosition().y + 5;
                        float wallBottom = wallTop + 30;

                        // Player positions
                        float playerLeft = player.getPosition().x + 10;
                        float playerRight = playerLeft + 20;  // Assuming player's width is 30
                        float playerTop = player.getPosition().y + 10;
                        float playerBottom = playerTop + 30;  // Assuming player's height is 30

                        // Check for horizontal collision (left and right)
                        if (playerBottom > wallTop && playerTop < wallBottom) { // Only check horizontal if vertical overlap
                            if (playerRight > wallLeft && playerLeft < wallLeft) { // Colliding from the left
                                right = false;  // Prevent rightward movement
                            }
                            else if (playerLeft < wallRight && playerRight > wallRight) { // Colliding from the right
                                left = false;  // Prevent leftward movement
                            }
                        }

                        // Check for vertical collision (up and down)
                        if (playerRight > wallLeft && playerLeft < wallRight) { // Only check vertical if horizontal overlap
                            if (playerBottom > wallTop && playerTop < wallTop) { // Colliding from above
                                down = false;  // Prevent downward movement
                            }
                            else if (playerTop < wallBottom && playerBottom > wallBottom) { // Colliding from below
                                up = false;  // Prevent upward movement
                            }
                        }
                    }
                }
            }


            // Player movement

            if (right && (Keyboard::isKeyPressed(Keyboard::Key::Right) || Keyboard::isKeyPressed(Keyboard::Key::D)))
            {
                int xTexture = 0;
                xTexture = (int)player.getPosition().x / 30 % 4;
                player.setTextureRect(IntRect(xTexture * width, height * 2, width, height));
                player.move(3, 0);
            }
            if (left && (Keyboard::isKeyPressed(Keyboard::Key::Left) || Keyboard::isKeyPressed(Keyboard::Key::A)))
            {
                int xTexture = 0;
                xTexture = (int)player.getPosition().x / 30 % 4;
                player.setTextureRect(IntRect(xTexture * width, height, width, height));
                player.move(-3, 0);
            }
            if (up && (Keyboard::isKeyPressed(Keyboard::Key::Up) || Keyboard::isKeyPressed(Keyboard::Key::W)))
            {
                int xTexture = 0;
                xTexture = (int)player.getPosition().y / 30 % 4;
                player.setTextureRect(IntRect(xTexture * width, height * 3, width, height));
                player.move(0, -3);
            }
            if (down && (Keyboard::isKeyPressed(Keyboard::Key::Down) || Keyboard::isKeyPressed(Keyboard::Key::S)))
            {
                int xTexture = 0;
                xTexture = (int)player.getPosition().y / 30 % 4;
                player.setTextureRect(IntRect(xTexture * width, 0.f, width, height));
                player.move(0, 3);
            }

            //Fire Traps


            Time trapTime = trapClock.getElapsedTime();
            int count = 0;
            if (trapTime.asSeconds() > 1.0f)
            {

                for (int i = 0; i < 17; i++)
                {
                    for (int j = 0; j < 29; j++)
                    {
                        if (arr[i][j] == 't')
                        {
                            count++;
                            if (thisTrap == 0)
                            {

                                maze[i][j] = floor;
                                maze[i][j].setPosition(j * 40, i * 40);
                            }
                            else
                            {

                                maze[i][j] = trap;
                                maze[i][j].setPosition(j * 40, i * 40);
                            }

                        }
                    }
                }
                if (count == 12)
                {
                    trapClock.restart();
                    if (thisTrap == 0)
                    {
                        thisTrap = 1;
                    }
                    else
                        thisTrap = 0;
                }
            }


            //Player and fire trap collision
            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 29; j++)
                {
                    if (thisTrap == 0 && arr[i][j] == 't' && ((player.getPosition().x + 20 > maze[i][j].getPosition().x) &&
                        (player.getPosition().x + 15 < maze[i][j].getPosition().x + 40) &&
                        (player.getPosition().y + 20 > maze[i][j].getPosition().y) &&
                        (player.getPosition().y + 10 < maze[i][j].getPosition().y + 40)))

                    {
                        lives--;
                        std::cout << "You have " << lives << " lives remaining." << std::endl;
                        sizzleS.play();

                        if (player.getPosition().x + 30 > maze[i][j].getPosition().x && player.getPosition().x < maze[i][j].getPosition().x)
                        {
                            player.setPosition(maze[i][j].getPosition().x - 50, player.getPosition().y);
                        }
                        else if (player.getPosition().x + 10 < maze[i][j].getPosition().x + 40 && player.getPosition().x + 30 > maze[i][j].getPosition().x + 40)
                        {
                            player.setPosition(maze[i][j].getPosition().x + 60, player.getPosition().y);
                        }

                        else if (player.getPosition().y + 30 > maze[i][j].getPosition().y && player.getPosition().y < maze[i][j].getPosition().y)
                        {
                            player.setPosition(player.getPosition().x, maze[i][j].getPosition().y - 50);
                        }
                        else if (player.getPosition().y < maze[i][j].getPosition().y + 40 && player.getPosition().y + 30 > maze[i][j].getPosition().y + 40)
                        {
                            player.setPosition(player.getPosition().x, maze[i][j].getPosition().y + 60);
                        }
                    }
                }
            }

            //Spike Traps

            if (spikes == false)
            {
                for (int i = 0; i < 17; i++)
                {
                    for (int j = 0; j < 29; j++)
                    {
                        if (arr[i][j] == 'g')
                            maze[i][j] = floor;
                        maze[i][j].setPosition(j * 40, i * 40);
                    }
                }
            }

            //Trigger and collision

            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 29; j++)
                {
                    if (arr[i][j] == 'g')
                    {

                        //Triggering the trap
                        if ((player.getPosition().x + 10 >= maze[i][j - 1].getPosition().x) && (player.getPosition().x + 5 <= maze[i][j + 1].getPosition().x + 30)
                            && (player.getPosition().y + 10 >= maze[i - 1][j].getPosition().y) && (player.getPosition().y + 5 <= maze[i + 1][j].getPosition().y + 30))
                        {
                            spikeS.play();
                            spikes = true;
                            maze[i][j] = spikeTrap;
                            maze[i][j].setPosition(j * 40, i * 40);
                        }

                        //Player collides with trap

                        if (ignoreSpike == false && ((player.getPosition().x + 20 > maze[i][j].getPosition().x) &&
                            (player.getPosition().x + 10 < maze[i][j].getPosition().x + 30) &&
                            (player.getPosition().y + 30 > maze[i][j].getPosition().y) &&
                            (player.getPosition().y + 10 < maze[i][j].getPosition().y + 30)))
                        {


                            lives--;
                            std::cout << "You have " << lives << " lives remaining." << std::endl;
                            spikeS.play();

                            if (player.getPosition().x + 20 > maze[i][j].getPosition().x && player.getPosition().x + 10 < maze[i][j].getPosition().x)
                            {

                                player.setPosition(maze[i][j].getPosition().x - 45, player.getPosition().y);
                            }
                            else if (player.getPosition().x + 10 < maze[i][j].getPosition().x + 30 && player.getPosition().x + 20 > maze[i][j].getPosition().x + 30)
                            {

                                player.setPosition(maze[i][j].getPosition().x + 60, player.getPosition().y);
                            }

                            else if (player.getPosition().y + 30 > maze[i][j].getPosition().y && player.getPosition().y < maze[i][j].getPosition().y)
                            {

                                player.setPosition(player.getPosition().x, maze[i][j].getPosition().y - 70);
                            }
                            else if (player.getPosition().y + 10 < maze[i][j].getPosition().y + 30 && player.getPosition().y + 20 > maze[i][j].getPosition().y + 30)
                            {

                                player.setPosition(player.getPosition().x, maze[i][j].getPosition().y + 60);
                            }
                        }
                    }
                }
            }




            //Losing condition

            if (lives == 0)
            {
                cout << "YOU DIED! :(" << endl;
                gameoverS.play();

                gameOver = true;

                RenderWindow endgame(VideoMode(1015, 595), "GameOver", Style::Default);
                endgame.setFramerateLimit(60);

                endgame.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 35));
                endgame.setPosition(Vector2i(-7, -40));

                RectangleShape background(Vector2f(1015.f, 595.f));
                Texture bg;
                bg.loadFromFile("background2.jpg");
                background.setTexture(&bg);

                Text gameover, menu, exit, restart;

                //Gameover
                gameover.setFont(font);
                gameover.setCharacterSize(150);
                gameover.setPosition(235.f, 50.f);
                gameover.setString("GAME OVER");

                //Restart
                restart.setFont(font);
                restart.setCharacterSize(30);
                restart.setPosition(365.f, 250.f);
                restart.setString("Press R to restart level");

                //Menu
                menu.setFont(font);
                menu.setCharacterSize(30);
                menu.setPosition(365.f, 350.f);
                menu.setString("Press M to enter Main Menu");

                //Exit
                exit.setFont(font);
                exit.setCharacterSize(30);
                exit.setPosition(365.f, 450.f);
                exit.setString("Press Esc to exit");

                while (endgame.isOpen())
                {
                    Event evnt;;
                    while (endgame.pollEvent(evnt))
                    {
                        if (evnt.type == Event::Closed)
                        {
                            ofstream res("resume.txt");
                            res << "0" << endl;
                            res << "0" << endl;
                            res.close();
                            ofstream level3("Level 3.txt");
                            level3 << highscore << endl;
                            level3.close();
                            endgame.close();
                            return false;
                        }

                    }
                    if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                    {
                        ofstream res("resume.txt");
                        res << "0" << endl;
                        res << "0" << endl;
                        res.close();
                        ofstream level3("Level 3.txt");
                        level3 << highscore << endl;
                        level3.close();
                        gameOver = true;
                        endgame.close();
                        return false;
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Key::R))
                    {
                        gameOver = false;
                        lives = 3;
                        spikes = false;
                        player.setPosition(37.5f, 600.f);
                        clock.restart();
                        endgame.close();
                        resume3 = false;
                        score = 0;

                        //Re-initialize collectbles

                        for (int i = 0; i < 17; i++)
                        {
                            for (int j = 0; j < 29; j++)
                            {
                                if (arr[i][j] == 'z')
                                {
                                    arr[i][j] = 'f';
                                }
                            }
                        }

                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Key::M))
                    {
                        ofstream res("resume.txt");
                        res << "0" << endl;
                        res << "0" << endl;
                        res.close();
                        ofstream level3("Level 3.txt");
                        level3 << highscore << endl;
                        level3.close();
                        endgame.close();
                        gameplay.close();
                        resume3 = false;
                        shouldBreak = MainMenu();
                        moveToMenu = true;
                        break;
                    }
                    endgame.clear();
                    endgame.draw(background);
                    endgame.draw(gameover);
                    endgame.draw(restart);
                    endgame.draw(menu);
                    endgame.draw(exit);
                    endgame.display();
                }
            }

            if (shouldBreak == true)
            {
                break;
            }

            //Collectibles

            playerX = (player.getPosition().x + 15) / 40;
            playerY = (player.getPosition().y + 30) / 40;

            if (arr[playerY][playerX] == 'f' && (playerY % 2 == 0 || playerX % 2 == 0))
            {
                arr[playerY][playerX] = 'z';
                score += 50;
                coin.play();
            }


            //Jump functionality

            Time jumpTime = jumpClock.getElapsedTime();


            if (isJumping == false && jumpTime.asSeconds() > 0.5 && Keyboard::isKeyPressed(Keyboard::Key::Space))
            {
                jumpClock.restart();
                jumpS.play();

                playerX = (player.getPosition().x + 15) / 40;
                playerY = (player.getPosition().y + 30) / 40;
                startPos = player.getPosition();

                if (arr[playerY][playerX - 1] == 'g' && playerX > 0)
                {
                    jumpS.play();
                    targetPos = startPos + Vector2f(-80.f, 0.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY][playerX + 1] == 'g' && playerX < 28)
                {
                    jumpS.play();
                    targetPos = startPos + Vector2f(80.f, 0.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY + 1][playerX] == 'g' && playerY < 16)
                {
                    jumpS.play();
                    targetPos = startPos + sf::Vector2f(0.f, 80.f);
                    isJumping = true;
                    ignoreSpike = true;
                }
                else if (arr[playerY - 1][playerX] == 'g' && playerY > 0)
                {
                    jumpS.play();
                    targetPos = startPos + sf::Vector2f(0.f, -80.f);
                    isJumping = true;
                    ignoreSpike = true;
                }

                jumpProgress = 0.0f;
            }

            if (isJumping)
            {
                jumpProgress += 0.05f;

                if (jumpProgress >= 1.0f)
                {
                    jumpProgress = 1.0f;
                    isJumping = false;
                    ignoreSpike = false;
                }


                Vector2f newPos = startPos + (targetPos - startPos) * jumpProgress;
                player.setPosition(newPos);
            }



            // Check win condition

            if (arr[playerY][playerX] == 'e')
            {
                ofstream res("resume.txt");
                res << "0" << endl;
                res << "0" << endl;
                res.close();
                ofstream level3("Level 3.txt");
                if (score > stoi(highscore))
                {
                    level3 << score << endl;
                }
                else
                    level3 << highscore << endl;

                level3.close();
                cout << "NISHAN E AQAL WALON K LIYE KAAFI HAI!!!!!" << endl;

                RenderWindow win(VideoMode(1160, 680), "You can't win", Style::Default);
                win.setFramerateLimit(60);

                // Auto maximize
                VideoMode desktopMode = VideoMode::getDesktopMode();
                win.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 39));
                win.setPosition(Vector2i(-7, -33));

                Sound winS, evilS;
                SoundBuffer winB, evilB;
                winB.loadFromFile("audio/violin.mp3");
                winS.setBuffer(winB);
                evilB.loadFromFile("audio/evil.mp3");
                evilS.setBuffer(evilB);
                bgS.stop();
                winS.play();


                // Dialogue lines
                vector<string> dialogues =
                {
                    "YOU WON",
                    "You really thought you could win? How pathetic.",
                    "I?ve watched you stumble through my maze.",
                    "Clinging to the foolish belief that you could outsmart me.",
                    "But every step you took, every choice you made.",
                    "It all led here.",
                    "Right where I wanted you.",
                    "You see, that?s the cruel truth you fail to grasp.",
                    "There was never any victory to claim.",
                    "You?re not the hero of some grand tale.",
                    " You?re a pawn, moved and discarded at my whim.",
                    "You fought so hard, endured so much.",
                    "All for nothing.",
                    "And now, here you stand, at the edge of oblivion.",
                    "Staring into the void I created for you.",
                    "This isn?t your story.",
                    "It never was.",
                    "It?s mine.",
                    " and it ends when I decide.",
                    "The truth is, the world won?t remember you.",
                    "No songs will be sung.",
                    "No statues will be raised.",
                    "Youll vanish",
                    "Nothing but a fleeting flicker of light.",
                    "You.",
                    "Are.",
                    "Nothing.",
                    "but you could be something.",
                    "",
                    "Credits : ",
                    "Aliha Fatima Chaudhary",
                    "Ali Shahid Hassan Nawaz",
                    "Syed Abdullah",
                    "Special thanks to Sir Usama Hassan Alvi"
                };

                // Text object for displaying dialogue
                Text dialogueText;
                dialogueText.setFont(font);
                dialogueText.setCharacterSize(50);
                dialogueText.setFillColor(Color::Red);
                dialogueText.setPosition(35.f, 340.f);

                size_t dialogueIndex = 0;

                Clock dialogue;
                int count = 0;
                while (win.isOpen())
                {
                    Time dTime = dialogue.getElapsedTime();
                    Event event;
                    while (win.pollEvent(event))
                    {
                        if (event.type == Event::Closed)
                            win.close();
                    }

                    if (dTime.asSeconds() > 3.5)
                    {
                        count++;
                        if (count == 1)
                            evilS.play();
                        if (dialogueIndex < dialogues.size() - 1)
                        {
                            dialogueIndex++;
                            dialogue.restart();
                        }
                        else
                            win.close(); // Close the window after the last dialogue
                    }

                    // Update the text to the current dialogue
                    dialogueText.setString(dialogues[dialogueIndex]);

                    // Render
                    win.clear();
                    win.draw(dialogueText);
                    win.display();
                }

                return false;
            }



            //Draw

            gameplay.clear();


            for (int i = 0; i < 17; i++) {
                for (int j = 0; j < 29; j++)
                {
                    gameplay.draw(maze[i][j]);
                }
            }
            for (int i = 0; i < 17; i++)
            {
                for (int j = 0; j < 29; j++)
                {
                    if (arr[i][j] == 'f')
                    {
                        gameplay.draw(collectible[i][j]);
                    }
                }
            }
            gameplay.draw(timer);
            gameplay.draw(scoreT);
            gameplay.draw(livesT);
            gameplay.draw(player);
            gameplay.display();

            //Pause game

            if (Keyboard::isKeyPressed(Keyboard::Key::P))
            {
                pauseS.play();

                Clock pauseClock;

                RenderWindow pause(VideoMode(1015, 595), "Paused", Style::Default);
                pause.setFramerateLimit(60);

                pause.setSize(Vector2u(desktopMode.width - 2, desktopMode.height - 35));
                pause.setPosition(Vector2i(-7, -40));

                RectangleShape background(Vector2f(1015.f, 595.f));
                Texture bg;
                bg.loadFromFile("background.jpg");
                background.setTexture(&bg);

                Text paused, resumeT, menu, exit, restart;

                //Paused
                paused.setFont(font);
                paused.setCharacterSize(100);
                paused.setPosition(235.f, 50.f);
                paused.setString("Game Paused");

                //Resume
                resumeT.setFont(font);
                resumeT.setCharacterSize(30);
                resumeT.setPosition(300.f, 200.f);
                resumeT.setString("Press Enter to resume level");

                //Restart
                restart.setFont(font);
                restart.setCharacterSize(30);
                restart.setPosition(300.f, 300.f);
                restart.setString("Press R to restart level");

                //Menu
                menu.setFont(font);
                menu.setCharacterSize(30);
                menu.setPosition(300.f, 400.f);
                menu.setString("Press M to enter Main Menu");

                //Exit
                exit.setFont(font);
                exit.setCharacterSize(30);
                exit.setPosition(300.f, 500.f);
                exit.setString("Press Esc to exit");

                while (pause.isOpen())
                {
                    Event evnt;
                    while (pause.pollEvent(evnt))
                    {
                        if (evnt.type == Event::Closed)
                        {
                            return 0;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "3" << endl;
                            res.close();
                            ofstream level3("Level 3.txt");
                            level3 << highscore << endl;
                            level3 << score << endl;
                            level3 << lives << endl;
                            level3 << player.getPosition().x << endl;
                            level3 << player.getPosition().y << endl;
                            level3 << to_string(time.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 29; j++)
                                {
                                    level3 << arr[i][j];
                                }
                                level3 << endl;
                            }
                            level3.close();
                            gameplay.close();
                            pause.close();
                            return false;
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
                        {
                            // Accumulate pause time and close pause menu
                            pauseTime += pauseClock.getElapsedTime();
                            //pauseClock.restart();
                            pause.close();
                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::R))
                        {
                            gameOver = false;
                            lives = 3;
                            spikes = false;
                            player.setPosition(37.5f, 600.f);
                            clock.restart();
                            pause.close();
                            pauseClock.restart();
                            resume3 = false;
                            score = 0;

                            //Re-initialize collectbles

                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 29; j++)
                                {
                                    if (arr[i][j] == 'z')
                                    {
                                        arr[i][j] = 'f';
                                    }
                                }
                            }

                        }
                        else if (Keyboard::isKeyPressed(Keyboard::Key::M))
                        {
                            ofstream res("resume.txt");
                            res << "1" << endl;
                            res << "3" << endl;
                            res.close();
                            ofstream level3("Level 3.txt");
                            level3 << highscore << endl;
                            level3 << score << endl;
                            level3 << lives << endl;
                            level3 << player.getPosition().x << endl;
                            level3 << player.getPosition().y << endl;
                            level3 << to_string(time.asSeconds() + prevTime) << endl;
                            for (int i = 0; i < 17; i++)
                            {
                                for (int j = 0; j < 29; j++)
                                {
                                    level3 << arr[i][j];
                                }
                                level3 << endl;
                            }
                            level3.close();
                            gameplay.close();
                            pause.close();
                            shouldBreak = MainMenu();
                            break;
                        }
                    }

                    if (shouldBreak == true)
                    {
                        break;
                    }

                    pause.clear();
                    pause.draw(background);
                    pause.draw(paused);
                    pause.draw(resumeT);
                    pause.draw(restart);
                    pause.draw(menu);
                    pause.draw(exit);
                    pause.display();
                }
            }
        }
    }
}

int main()
{
    bool isStart;
    isStart = MainMenu();
    while (isStart == true)
    {
        // Resuming from previous level
        if (resume1 == true)
        {
            isStart = Level1();
            if (isStart == true)
            {
                isStart = Level2();
                if (isStart == true)
                    isStart = Level3();
            }
        }
        else if (resume2 == true)
        {
            isStart = Level2();
            if (isStart == true)
                isStart = Level3();
        }
        else if (resume3 == true)
        {
            isStart = Level3();
        }
        else // Starting new game
        {
            isStart = Level1();
            if (isStart == true)
            {
                isStart = Level2();
                if (isStart == true)
                    isStart = Level3();
            }
        }
    }

    return 0;
}
