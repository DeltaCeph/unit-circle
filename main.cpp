//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//Trigonometric constants
const int OriginX = (SCREEN_WIDTH/2);
const int OriginY = (SCREEN_HEIGHT/2);

const float pi = 3.14159265;
const int rad = 200;

//Texture wrapper class
class Texture
{
    public:
        //Initialize variables
        Texture();

        //Deallocate memory
        ~Texture();

        //Loads image at path
        bool loadFromFile(std::string path);

        //Load text from font and string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

        //Deallocate texture
        void free();

        //Render texture at a given point
        void render(int x, int y);

        //Get dimensions
        int getWidth();
        int getHeight();

    private:
        //The texture
        SDL_Texture* cTexture;

        //Image dimensions;
        int cWidth;
        int cHeight;        
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Global Font
TTF_Font* gFont = NULL;

//Text Textures
Texture gTextBuffer;


Texture::Texture()
{
    //Initialize
    cTexture = NULL;
    cWidth = 0;
    cHeight = 0;
}

Texture::~Texture()
{
    //Deallocate
    free();
}

bool Texture::loadFromFile(std::string path)
{
    //Get rid of old textures
    free();

    //Final Texture
    SDL_Texture* newTexture = NULL;

    //Load image at path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Create texture from surface
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            cWidth = loadedSurface->w;
            cHeight = loadedSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    cTexture = newTexture;
    return cTexture != NULL;
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    //Get rid of old texture
    free();

    //Render text
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if(textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture
        cTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(cTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get dimensions
            cWidth = textSurface->w;
            cHeight = textSurface->h;
        }

        //Remove surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return cTexture != NULL;
}

void Texture::free()
{
    //Free texture if there is one
    if(cTexture != NULL)
    {
        SDL_DestroyTexture(cTexture);
        cTexture = NULL;
        cWidth = 0;
        cHeight = 0;
    }
}

void Texture::render(int x, int y)
{
    //Set space and render
    SDL_Rect renderQuad = {x, y, cWidth, cHeight};
    SDL_RenderCopy(gRenderer, cTexture, NULL, &renderQuad);
}

int Texture::getWidth()
{
    return cWidth;
}

int Texture::getHeight()
{
    return cHeight;
}


bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "Unit Circle Fun!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }

                if(TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Open font
    gFont = TTF_OpenFont("CourierPrimeCode.ttf", 12);
    if(gFont == NULL)
    {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        //Prep Text Texture
        SDL_Color textColor = {0, 0, 0};
        if(!gTextBuffer.loadFromRenderedText("The caffinated mathimatician jumps over a lazy pack of foxes.", textColor))
        {
            printf("Failed to render text!\n");
            success = false;
        }
    }

    //Nothing to load
    return success;
}

void close()
{
    //Free images
    gTextBuffer.free();

    //Free font
    TTF_CloseFont(gFont);
    gFont = NULL;

    //Destroy window    
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}


void traceCircle (int x0, int y0, int radius)
{
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while(x >= y)
    {
        SDL_RenderDrawPoint(gRenderer, x0 + x, y0 - y);
        SDL_RenderDrawPoint(gRenderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(gRenderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(gRenderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(gRenderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(gRenderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(gRenderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(gRenderer, x0 - y, y0 + x);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-radius << 1) + dx;
        }

        SDL_RenderPresent( gRenderer );
        SDL_Delay(25);
    }
}

void drawAngle(int xs, int ys, double input, int radius)
{
    int delay = 50;

    int r_x = 0;
    int r_y = 0;

    r_x = cos(input * pi / 180.0) * radius;
    r_y = sin(input * pi / 180.0) * radius;

    SDL_RenderDrawLine(gRenderer, xs, ys, xs + r_x, ys - r_y);
    SDL_RenderPresent(gRenderer);

    SDL_Delay(delay);

    SDL_Color textColor = {0, 0, 0};

    int switchBuff = input;

    switch(switchBuff)
    {
        case 0:
        gTextBuffer.loadFromRenderedText("0pi (1, 0)", textColor);
        gTextBuffer.render((xs + r_x) + 5, (ys - r_y) - gTextBuffer.getHeight());
        SDL_RenderPresent(gRenderer);
        break;

        case 30:
        gTextBuffer.loadFromRenderedText("1/6pi (sqrt(3)/2, 1/2)", textColor);
        gTextBuffer.render((xs + r_x) + 7, (ys - r_y) - gTextBuffer.getHeight()/3);
        SDL_RenderPresent(gRenderer);
        break;

        case 45:
        gTextBuffer.loadFromRenderedText("1/4pi (sqrt(2)/2, sqrt(2)/2)", textColor);
        gTextBuffer.render((xs + r_x) + 5, (ys - r_y) - gTextBuffer.getHeight()/2);
        SDL_RenderPresent(gRenderer);
        break;

        case 60:
        gTextBuffer.loadFromRenderedText("1/3pi (1/2, sqrt(3)/2)", textColor);
        gTextBuffer.render((xs + r_x) + 3, (ys - r_y) - gTextBuffer.getHeight());
        SDL_RenderPresent(gRenderer);
        break;


        case 90:
        gTextBuffer.loadFromRenderedText("1/2pi (0, 1)", textColor);
        gTextBuffer.render((xs + r_x) - (gTextBuffer.getWidth() / 2), (ys - r_y) - gTextBuffer.getHeight());
        SDL_RenderPresent(gRenderer);
        break;

        case 120:
        gTextBuffer.loadFromRenderedText("2/3pi (-1/2, sqrt(3)/2)", textColor);
        gTextBuffer.render(((xs + r_x) - 3) - gTextBuffer.getWidth(), (ys - r_y) - gTextBuffer.getHeight());
        SDL_RenderPresent(gRenderer);
        break;

        case 135:
        gTextBuffer.loadFromRenderedText("3/4pi (-sqrt(2)/2, sqrt(2)/2)", textColor);
        gTextBuffer.render(((xs + r_x) - 5) - gTextBuffer.getWidth(), (ys - r_y) - gTextBuffer.getHeight()/2);
        SDL_RenderPresent(gRenderer);
        break;

        case 150:
        gTextBuffer.loadFromRenderedText("5/6pi (-sqrt(3)/2, 1/2)", textColor);
        gTextBuffer.render(((xs + r_x) - 7) - gTextBuffer.getWidth(), (ys - r_y)- gTextBuffer.getHeight()/3);
        SDL_RenderPresent(gRenderer);
        break;


        case 180:
        gTextBuffer.loadFromRenderedText("1pi (-1, 0)", textColor);
        gTextBuffer.render((xs + r_x) - gTextBuffer.getWidth() - 5, (ys - r_y) - (gTextBuffer.getHeight()/2));
        SDL_RenderPresent(gRenderer);
        break;

        case 210:
        gTextBuffer.loadFromRenderedText("7/6pi (-sqrt(3)/2, -1/2)", textColor);
        gTextBuffer.render((xs + r_x) - gTextBuffer.getWidth(), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;

        case 225:
        gTextBuffer.loadFromRenderedText("5/4pi (-sqrt(2)/2, -sqrt(2)/2)", textColor);
        gTextBuffer.render((xs + r_x) - gTextBuffer.getWidth(), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;

        case 240:
        gTextBuffer.loadFromRenderedText("4/3pi (-1/2, -sqrt(3)/2)", textColor);
        gTextBuffer.render((xs + r_x) - gTextBuffer.getWidth(), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;


        case 270:
        gTextBuffer.loadFromRenderedText("3/2pi (0, -1)", textColor);
        gTextBuffer.render((xs + r_x) - gTextBuffer.getWidth()/2, (ys - r_y) + (gTextBuffer.getHeight()/2));
        SDL_RenderPresent(gRenderer);
        break;

        case 300:
        gTextBuffer.loadFromRenderedText("5/3pi (1/2, -sqrt(3)/2)", textColor);
        gTextBuffer.render((xs + r_x), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;

        case 315:
        gTextBuffer.loadFromRenderedText("7/4pi (sqrt(2)/2, -sqrt(2)/2)", textColor);
        gTextBuffer.render((xs + r_x), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;

        case 330:
        gTextBuffer.loadFromRenderedText("11/6pi (sqrt(3)/2, -1/2)", textColor);
        gTextBuffer.render((xs + r_x), (ys - r_y));
        SDL_RenderPresent(gRenderer);
        break;


        case 360:
        gTextBuffer.loadFromRenderedText("2pi (1, 0)", textColor);
        gTextBuffer.render((xs + r_x) + 7, (ys - r_y) + gTextBuffer.getHeight()/2);
        SDL_RenderPresent(gRenderer);
        break;
    }

    SDL_Delay(delay);
}

void popularLines(int x0, int y0, int radi)
{   
    drawAngle(x0, y0, 0.0, radi);
    drawAngle(x0, y0, 30.0, radi);
    drawAngle(x0, y0, 45.0, radi);
    drawAngle(x0, y0, 60.0, radi);
    
    drawAngle(x0, y0, 90.0, radi);
    drawAngle(x0, y0, 120.0, radi);
    drawAngle(x0, y0, 135.0, radi);
    drawAngle(x0, y0, 150.0, radi);
    
    drawAngle(x0, y0, 180.0, radi);
    drawAngle(x0, y0, 210.0, radi);
    drawAngle(x0, y0, 225.0, radi);
    drawAngle(x0, y0, 240.0, radi);

    drawAngle(x0, y0, 270.0, radi);
    drawAngle(x0, y0, 300.0, radi);
    drawAngle(x0, y0, 315.0, radi);
    drawAngle(x0, y0, 330.0, radi);
    
    drawAngle(x0, y0, 360.0, radi);
}

int drawn = 0;
int lined = 0;

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {   
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }

                    else if( e.type == SDL_KEYDOWN)
                    {
                        switch(e.key.keysym.sym)
                        {
                            case SDLK_ESCAPE:
                                quit = true;
                                break;
                        }
                    }
                }

                if(drawn == 0)
                {
                    //Clear Screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );
                }

                //Draw points
                SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );

                if(drawn == 0)
                {
                    traceCircle(OriginX, OriginY, rad);

                    drawn = 1;
                }

                if(drawn == 1 || lined == 0)
                {
                    popularLines(OriginX, OriginY, rad);

                    lined = 1;
                }

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}