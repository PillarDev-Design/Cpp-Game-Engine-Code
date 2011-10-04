// Headers
// ** Import the SDL function libraries.
// ** Import the C++ string, stream (save/load) libraries
// ** Import the vector math library
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

// Screen constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;
// Zone DIM constants
const int ZONE_WIDTH = 1280;
const int ZONE_HEIGHT = 960;
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 1200;
const int TOTAL_SPRITES = 40;
// Static tiles
// ** I've set the tiles to be represented with ID's
// ** The splice function that disects the loaded img file assigns
// **   each of the tiles with the associated number.
// ** It's possible to load this into a looped array to decrease
// **   code line size.
// Passable [0-27]
// ** These tiles (0-27) represent tiles that the character can pass.
const int TILE_GrassOne = 0;
const int TILE_GrassTwo = 1;
const int TILE_GrassThree = 2;
const int TILE_GrassFour = 3;
const int TILE_GrassFive = 4;
const int TILE_DirtIntersection = 5;
const int TILE_DirtTopLeft = 6;
const int TILE_DirtLeft = 7;
const int TILE_DirtBottomLeft = 8;
const int TILE_DirtTop = 9;
const int TILE_DirtMiddle = 10;
const int TILE_DirtBottom = 11;
const int TILE_DirtTopRight = 12;
const int TILE_DirtRight = 13;
const int TILE_DirtBottomRight = 14;
const int TILE_OrangeFlowerOne = 15;
const int TILE_OrangeFlowerTwo = 16;
const int TILE_VioletFlowerOne = 17;
const int TILE_VioletFlowerTwo = 18;
const int TILE_BlueFlowerOne = 19;
const int TILE_BlueFlowerTwo = 20;
const int TILE_BushOne = 21;
const int TILE_BushTwo = 22;
const int TILE_BushThree = 23;
const int TILE_BushFour = 24;
const int TILE_BushFive = 25;
const int TILE_DirtBush = 26;
const int TILE_GrassBush = 27;
// Impassable [27-39]
// ** Tiles (27-39) represent tiles that cannot be passed.
const int TILE_SmallTreeOne = 28;
const int TILE_SmallTreeTwo = 29;
const int TILE_SmallTreeThree = 30;
const int TILE_SmallTreeFour = 31;
const int TILE_LogStack = 32;
const int TILE_LogStump = 33;
const int TILE_LogOneLeft = 34;
const int TILE_LogOneRight = 35;
const int TILE_LogTwoLeft = 36;
const int TILE_LogTwoRight = 37;
const int TILE_RockOneGrass = 38;
const int TILE_RockTwoGrass = 39;
// Animated sprites
// ** These declarations represent the sprite attributes.
// Main Char Dims
// ** Represents the main character's attributes.
const int CHAR_SPRITE_WIDTH = 32;
const int CHAR_SPRITE_HEIGHT = 32;
// ** Directions are represented on an array of 0-3.
const int DIR_UP = 0;
const int DIR_RIGHT = 1;
const int DIR_DOWN = 2;
const int DIR_LEFT = 3;
// ** Each one of the directions have 4 frames in a movement direction.
SDL_Rect mainClipsUp[3];
SDL_Rect mainClipsRight[3];
SDL_Rect mainClipsDown[3];
SDL_Rect mainClipsLeft[3];
//*******************************\\
//Surfaces
SDL_Surface *generalScene = NULL;
SDL_Surface *mainCharSpriteSheet = NULL;
SDL_Surface *screen = NULL;
//Fonts
TTF_Font *font = NULL;
SDL_Color textColor = {0,0,0};
//Music
//clip the generalScene
SDL_Rect tileClips[TOTAL_SPRITES];
//Events
SDL_Event event;
//Camera
SDL_Rect camera = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
//*******************************\\
//***CLASS DECLARATIONS ***
class Tile {
  private:
    SDL_Rect box;
    int type;
  public:
    Tile(int x, int y, int tileType);
    void show();
    int get_type();
    SDL_Rect &get_box();
};
// ** Timer represents how the application regulates frame rates
// **  and occurance of accepting user input.
class Timer {
  private:
    int startTicks;
    int pausedTicks;
    bool paused;
    bool started;
  public:
    Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    int get_ticks();
    bool is_started();
    bool is_paused();
};
class Character {
  private:
    SDL_Rect box;
    int xVel, yVel;
    int frame;
    int status;
  public:
    Character();
    void handle_events();
    void move(Tile *tiles[]);
    void show();
    void set_camera();
    //Saves/Loads
    void set_x(int X);
    void set_y(int Y);
    void set_frame(int F);
    void set_status(int S);
    int get_x();
    int get_y();
    int get_frame();
    int get_status();
};
//*******************************\\
//*** GENERAL FUNCTIONS ***
//load_image
SDL_Surface *load_image(std::string filename) {
  SDL_Surface *loadedImage = NULL;
  SDL_Surface *optimizedImage = NULL;
  loadedImage = IMG_Load(filename.c_str());
  if (loadedImage != NULL) {
    optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
    SDL_FreeSurface(loadedImage);
  }
  return optimizedImage;
}
//apply_surface
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL) {
  SDL_Rect offset;
  offset.x = x;
  offset.y = y;
  SDL_BlitSurface(source,clip,destination,&offset);
}
//check_collision
bool check_collision(SDL_Rect A, SDL_Rect B) {
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;
  
  leftA = A.x;
  rightA = A.x + A.w;
  topA = A.y;
  bottomA = A.y + A.h;
  
  leftB = B.x;
  rightB = B.x + B.w;
  topB = B.y;
  bottomB = B.y + B.h;
  
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}
//set_tiles
void set_clips() {
  // ** Once the image file is loaded, we need to splice the images apart
  // **  and assign them to the declared sprite array.
  //***MAIN CHARACTER***
  //Up
  mainClipsUp[0].x = CHAR_SPRITE_WIDTH * 6;
  mainClipsUp[0].y = CHAR_SPRITE_HEIGHT * 7;
  mainClipsUp[0].w = CHAR_SPRITE_WIDTH;
  mainClipsUp[0].h = CHAR_SPRITE_HEIGHT;
  mainClipsUp[1].x = CHAR_SPRITE_WIDTH * 7;
  mainClipsUp[1].y = CHAR_SPRITE_HEIGHT * 7;
  mainClipsUp[1].w = CHAR_SPRITE_WIDTH;
  mainClipsUp[1].h = CHAR_SPRITE_HEIGHT;
  mainClipsUp[2].x = CHAR_SPRITE_WIDTH * 8;
  mainClipsUp[2].y = CHAR_SPRITE_HEIGHT * 7;
  mainClipsUp[2].w = CHAR_SPRITE_WIDTH;
  mainClipsUp[2].h = CHAR_SPRITE_HEIGHT;
  //Right
  mainClipsRight[0].x = CHAR_SPRITE_WIDTH * 6;
  mainClipsRight[0].y = CHAR_SPRITE_HEIGHT * 6;
  mainClipsRight[0].w = CHAR_SPRITE_WIDTH;
  mainClipsRight[0].h = CHAR_SPRITE_HEIGHT;
  mainClipsRight[1].x = CHAR_SPRITE_WIDTH * 7;
  mainClipsRight[1].y = CHAR_SPRITE_HEIGHT * 6;
  mainClipsRight[1].w = CHAR_SPRITE_WIDTH;
  mainClipsRight[1].h = CHAR_SPRITE_HEIGHT;
  mainClipsRight[2].x = CHAR_SPRITE_WIDTH * 8;
  mainClipsRight[2].y = CHAR_SPRITE_HEIGHT * 6;
  mainClipsRight[2].w = CHAR_SPRITE_WIDTH;
  mainClipsRight[2].h = CHAR_SPRITE_HEIGHT;
  //Left
  mainClipsLeft[0].x = CHAR_SPRITE_WIDTH * 6;
  mainClipsLeft[0].y = CHAR_SPRITE_HEIGHT * 5;
  mainClipsLeft[0].w = CHAR_SPRITE_WIDTH;
  mainClipsLeft[0].h = CHAR_SPRITE_HEIGHT;
  mainClipsLeft[1].x = CHAR_SPRITE_WIDTH * 7;
  mainClipsLeft[1].y = CHAR_SPRITE_HEIGHT * 5;
  mainClipsLeft[1].w = CHAR_SPRITE_WIDTH;
  mainClipsLeft[1].h = CHAR_SPRITE_HEIGHT;
  mainClipsLeft[2].x = CHAR_SPRITE_WIDTH * 8;
  mainClipsLeft[2].y = CHAR_SPRITE_HEIGHT * 5;
  mainClipsLeft[2].w = CHAR_SPRITE_WIDTH;
  mainClipsLeft[2].h = CHAR_SPRITE_HEIGHT;
  //Down
  mainClipsDown[0].x = CHAR_SPRITE_WIDTH * 6;
  mainClipsDown[0].y = CHAR_SPRITE_HEIGHT * 4;
  mainClipsDown[0].w = CHAR_SPRITE_WIDTH;
  mainClipsDown[0].h = CHAR_SPRITE_HEIGHT;
  mainClipsDown[1].x = CHAR_SPRITE_WIDTH * 7;
  mainClipsDown[1].y = CHAR_SPRITE_HEIGHT * 4;
  mainClipsDown[1].w = CHAR_SPRITE_WIDTH;
  mainClipsDown[1].h = CHAR_SPRITE_HEIGHT;
  mainClipsDown[2].x = CHAR_SPRITE_WIDTH * 8;
  mainClipsDown[2].y = CHAR_SPRITE_HEIGHT * 4;
  mainClipsDown[2].w = CHAR_SPRITE_WIDTH;
  mainClipsDown[2].h = CHAR_SPRITE_HEIGHT;
  //***TILES
  //***PASSABLES
  tileClips[TILE_GrassOne].x = 0;
  tileClips[TILE_GrassOne].y = 0;
  tileClips[TILE_GrassOne].w = TILE_WIDTH;
  tileClips[TILE_GrassOne].h = TILE_HEIGHT;
  
  tileClips[TILE_GrassTwo].x = 0;
  tileClips[TILE_GrassTwo].y = TILE_HEIGHT;
  tileClips[TILE_GrassTwo].w = TILE_WIDTH;
  tileClips[TILE_GrassTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_GrassThree].x = 0;
  tileClips[TILE_GrassThree].y = TILE_HEIGHT * 2;
  tileClips[TILE_GrassThree].w = TILE_WIDTH;
  tileClips[TILE_GrassThree].h = TILE_HEIGHT;
  
  tileClips[TILE_GrassFour].x = 0;
  tileClips[TILE_GrassFour].y = TILE_HEIGHT * 3;
  tileClips[TILE_GrassFour].w = TILE_WIDTH;
  tileClips[TILE_GrassFour].h = TILE_HEIGHT;
  
  tileClips[TILE_GrassFive].x = 0;
  tileClips[TILE_GrassFive].y = TILE_HEIGHT * 4;
  tileClips[TILE_GrassFive].w = TILE_WIDTH;
  tileClips[TILE_GrassFive].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtIntersection].x = 0;
  tileClips[TILE_DirtIntersection].y = TILE_HEIGHT * 5;
  tileClips[TILE_DirtIntersection].w = TILE_WIDTH;
  tileClips[TILE_DirtIntersection].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtTopLeft].x = 0;
  tileClips[TILE_DirtTopLeft].y = TILE_HEIGHT * 6;
  tileClips[TILE_DirtTopLeft].w = TILE_WIDTH;
  tileClips[TILE_DirtTopLeft].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtLeft].x = 0;
  tileClips[TILE_DirtLeft].y = TILE_HEIGHT * 7;
  tileClips[TILE_DirtLeft].w = TILE_WIDTH;
  tileClips[TILE_DirtLeft].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtBottomLeft].x = 0;
  tileClips[TILE_DirtBottomLeft].y = TILE_HEIGHT *8;
  tileClips[TILE_DirtBottomLeft].w = TILE_WIDTH;
  tileClips[TILE_DirtBottomLeft].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtTop].x = 0;
  tileClips[TILE_DirtTop].y = TILE_HEIGHT *9;
  tileClips[TILE_DirtTop].w = TILE_WIDTH;
  tileClips[TILE_DirtTop].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtMiddle].x = 0;
  tileClips[TILE_DirtMiddle].y = TILE_HEIGHT *10;
  tileClips[TILE_DirtMiddle].w = TILE_WIDTH;
  tileClips[TILE_DirtMiddle].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtBottom].x = 0;
  tileClips[TILE_DirtBottom].y = TILE_HEIGHT *11;
  tileClips[TILE_DirtBottom].w = TILE_WIDTH;
  tileClips[TILE_DirtBottom].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtTopRight].x = 0;
  tileClips[TILE_DirtTopRight].y = TILE_HEIGHT *12;
  tileClips[TILE_DirtTopRight].w = TILE_WIDTH;
  tileClips[TILE_DirtTopRight].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtRight].x = 0;
  tileClips[TILE_DirtRight].y = TILE_HEIGHT *13;
  tileClips[TILE_DirtRight].w = TILE_WIDTH;
  tileClips[TILE_DirtRight].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtBottomRight].x = 0;
  tileClips[TILE_DirtBottomRight].y = TILE_HEIGHT *14;
  tileClips[TILE_DirtBottomRight].w = TILE_WIDTH;
  tileClips[TILE_DirtBottomRight].h = TILE_HEIGHT;
  
  tileClips[TILE_OrangeFlowerOne].x = 0;
  tileClips[TILE_OrangeFlowerOne].y = TILE_HEIGHT *15;
  tileClips[TILE_OrangeFlowerOne].w = TILE_WIDTH;
  tileClips[TILE_OrangeFlowerOne].h = TILE_HEIGHT;
  
  tileClips[TILE_OrangeFlowerTwo].x = 0;
  tileClips[TILE_OrangeFlowerTwo].y = TILE_HEIGHT *16;
  tileClips[TILE_OrangeFlowerTwo].w = TILE_WIDTH;
  tileClips[TILE_OrangeFlowerTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_VioletFlowerOne].x = 0;
  tileClips[TILE_VioletFlowerOne].y = TILE_HEIGHT *17;
  tileClips[TILE_VioletFlowerOne].w = TILE_WIDTH;
  tileClips[TILE_VioletFlowerOne].h = TILE_HEIGHT;
  
  tileClips[TILE_VioletFlowerTwo].x = 0;
  tileClips[TILE_VioletFlowerTwo].y = TILE_HEIGHT *18;
  tileClips[TILE_VioletFlowerTwo].w = TILE_WIDTH;
  tileClips[TILE_VioletFlowerTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_BlueFlowerOne].x = 0;
  tileClips[TILE_BlueFlowerOne].y = TILE_HEIGHT *19;
  tileClips[TILE_BlueFlowerOne].w = TILE_WIDTH;
  tileClips[TILE_BlueFlowerOne].h = TILE_HEIGHT;
  
  tileClips[TILE_BlueFlowerTwo].x = 0;
  tileClips[TILE_BlueFlowerTwo].y = TILE_HEIGHT *20;
  tileClips[TILE_BlueFlowerTwo].w = TILE_WIDTH;
  tileClips[TILE_BlueFlowerTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_BushOne].x = 0;
  tileClips[TILE_BushOne].y = TILE_HEIGHT *21;
  tileClips[TILE_BushOne].w = TILE_WIDTH;
  tileClips[TILE_BushOne].h = TILE_HEIGHT;
  
  tileClips[TILE_BushTwo].x = 0;
  tileClips[TILE_BushTwo].y = TILE_HEIGHT *22;
  tileClips[TILE_BushTwo].w = TILE_WIDTH;
  tileClips[TILE_BushTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_BushThree].x = 0;
  tileClips[TILE_BushThree].y = TILE_HEIGHT *23;
  tileClips[TILE_BushThree].w = TILE_WIDTH;
  tileClips[TILE_BushThree].h = TILE_HEIGHT;
  
  tileClips[TILE_BushFour].x = 0;
  tileClips[TILE_BushFour].y = TILE_HEIGHT *24;
  tileClips[TILE_BushFour].w = TILE_WIDTH;
  tileClips[TILE_BushFour].h = TILE_HEIGHT;
  
  tileClips[TILE_BushFive].x = 0;
  tileClips[TILE_BushFive].y = TILE_HEIGHT *25;
  tileClips[TILE_BushFive].w = TILE_WIDTH;
  tileClips[TILE_BushFive].h = TILE_HEIGHT;
  
  tileClips[TILE_DirtBush].x = 0;
  tileClips[TILE_DirtBush].y = TILE_HEIGHT *26;
  tileClips[TILE_DirtBush].w = TILE_WIDTH;
  tileClips[TILE_DirtBush].h = TILE_HEIGHT;
  
  tileClips[TILE_GrassBush].x = 0;
  tileClips[TILE_GrassBush].y = TILE_HEIGHT *27;
  tileClips[TILE_GrassBush].w = TILE_WIDTH;
  tileClips[TILE_GrassBush].h = TILE_HEIGHT;
  
  //**IMPASSABLES
  tileClips[TILE_SmallTreeOne].x = TILE_WIDTH * 4;
  tileClips[TILE_SmallTreeOne].y = 0;
  tileClips[TILE_SmallTreeOne].w = TILE_WIDTH;
  tileClips[TILE_SmallTreeOne].h = TILE_HEIGHT;
  
  tileClips[TILE_SmallTreeTwo].x = TILE_WIDTH * 4;
  tileClips[TILE_SmallTreeTwo].y = TILE_HEIGHT;
  tileClips[TILE_SmallTreeTwo].w = TILE_WIDTH;
  tileClips[TILE_SmallTreeTwo].h = TILE_HEIGHT;
  
  tileClips[TILE_SmallTreeThree].x = TILE_WIDTH * 4;
  tileClips[TILE_SmallTreeThree].y = TILE_HEIGHT *2;
  tileClips[TILE_SmallTreeThree].w = TILE_WIDTH;
  tileClips[TILE_SmallTreeThree].h = TILE_HEIGHT;
  
  tileClips[TILE_SmallTreeFour].x = TILE_WIDTH * 4;
  tileClips[TILE_SmallTreeFour].y = TILE_HEIGHT *3;
  tileClips[TILE_SmallTreeFour].w = TILE_WIDTH;
  tileClips[TILE_SmallTreeFour].h = TILE_HEIGHT;
  
  tileClips[TILE_LogStack].x = TILE_WIDTH * 4;
  tileClips[TILE_LogStack].y = TILE_HEIGHT *4;
  tileClips[TILE_LogStack].w = TILE_WIDTH;
  tileClips[TILE_LogStack].h = TILE_HEIGHT;
  
  tileClips[TILE_LogStump].x = TILE_WIDTH * 4;
  tileClips[TILE_LogStump].y = TILE_HEIGHT *5;
  tileClips[TILE_LogStump].w = TILE_WIDTH;
  tileClips[TILE_LogStump].h = TILE_HEIGHT;
  
  tileClips[TILE_LogOneLeft].x = TILE_WIDTH * 4;
  tileClips[TILE_LogOneLeft].y = TILE_HEIGHT *6;
  tileClips[TILE_LogOneLeft].w = TILE_WIDTH;
  tileClips[TILE_LogOneLeft].h = TILE_HEIGHT;
  
  tileClips[TILE_LogOneRight].x = TILE_WIDTH * 4;
  tileClips[TILE_LogOneRight].y = TILE_HEIGHT *7;
  tileClips[TILE_LogOneRight].w = TILE_WIDTH;
  tileClips[TILE_LogOneRight].h = TILE_HEIGHT;
  
  tileClips[TILE_LogTwoLeft].x = TILE_WIDTH * 4;
  tileClips[TILE_LogTwoLeft].y = TILE_HEIGHT *8;
  tileClips[TILE_LogTwoLeft].w = TILE_WIDTH;
  tileClips[TILE_LogTwoLeft].h = TILE_HEIGHT;
  
  tileClips[TILE_LogTwoRight].x = TILE_WIDTH * 4;
  tileClips[TILE_LogTwoRight].y = TILE_HEIGHT *9;
  tileClips[TILE_LogTwoRight].w = TILE_WIDTH;
  tileClips[TILE_LogTwoRight].h = TILE_HEIGHT;
  
  tileClips[TILE_RockOneGrass].x = TILE_WIDTH * 4;
  tileClips[TILE_RockOneGrass].y = TILE_HEIGHT *10;
  tileClips[TILE_RockOneGrass].w = TILE_WIDTH;
  tileClips[TILE_RockOneGrass].h = TILE_HEIGHT;
  
  tileClips[TILE_RockTwoGrass].x = TILE_WIDTH * 4;
  tileClips[TILE_RockTwoGrass].y = TILE_HEIGHT *11;
  tileClips[TILE_RockTwoGrass].w = TILE_WIDTH;
  tileClips[TILE_RockTwoGrass].h = TILE_HEIGHT;
}
//set_tiles
bool set_tiles(Tile *tiles[]) {
  int x = 0, y = 0;
  std::ifstream map("Zones/zoneOne.map");
  if (map == NULL) { return false; }
  
  for (int t = 0; t < TOTAL_TILES; t++) {
    int tileType = -1;
    map >> tileType;
    if (map.fail() == true) { map.close(); return false; }
    if ((tileType >= 0) && (tileType < TOTAL_SPRITES)) { tiles[t] = new Tile(x,y,tileType); }
    else { map.close(); return false; }
    
    x += TILE_WIDTH;
    if (x >= ZONE_WIDTH) { x = 0; y += TILE_HEIGHT; }
  }//end for
  map.close();
  return true;
}
//touches_wall
bool touches_wall(SDL_Rect box, Tile *tiles[]) {
  for (int t = 0; t < TOTAL_TILES; t++) {
    if ((tiles[t]->get_type() >= TILE_SmallTreeOne)&&(tiles[t]->get_type() <= TILE_RockTwoGrass)) {
      if (check_collision(box,tiles[t]->get_box()) == true) { return true; }
    }
  }//end for
  return false;
}
//init
bool init() {
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) { return false; }
  screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);
  if (screen == NULL) { return false; }
  //ttf/mixer
  if (TTF_Init() == -1) { return false; }
  if (Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096) == -1) { return false; }
  
  SDL_WM_SetCaption("Land of Chaos v1.0", NULL);
  return true;
}
//load_files
bool load_files() {
  generalScene = load_image("Graphics/Scene/LoCEnvironment2.png");
  mainCharSpriteSheet = load_image("Graphics/Main/Classes_Low.png");
  font = TTF_OpenFont("Graphics/Fonts/AG_Futura.ttf", 12);
  
  if (generalScene == NULL) { return false; }
  if (mainCharSpriteSheet == NULL) { return false; }
  if (font == NULL) { return false; }
  
  return true;
}
//clean_up
void clean_up(Tile *tiles[]) {
  SDL_FreeSurface(generalScene);
  SDL_FreeSurface(mainCharSpriteSheet);
  
  for (int t = 0; t < TOTAL_TILES; t++) { delete tiles[t]; }
  
  TTF_CloseFont(font);
  
  TTF_Quit();
  SDL_Quit();
}
//*******************************\\
//*** CLASS FUNCTIONS ***
//***TILE
Tile::Tile(int x, int y, int tileType) {
  box.x = x;
  box.y = y;
  box.w = TILE_WIDTH;
  box.h = TILE_HEIGHT;
  type = tileType;
}
void Tile::show() {
  if (check_collision(camera,box) == true) {
    apply_surface(box.x - camera.x, box.y - camera.y, generalScene, screen, &tileClips[type]);
  }
}
int Tile::get_type() { return type; }
SDL_Rect &Tile::get_box() { return box; }
//***TIMER
Timer::Timer() {
  startTicks = 0;
  pausedTicks = 0;
  paused = false;
  started = false;
}
void Timer::start() {
  started = true;
  paused = false;
  startTicks = SDL_GetTicks();
}
void Timer::stop() {
     started = false;
     paused = false;
}
void Timer::pause() {
  if ((started == true) && (paused == false)) {
    paused = true;
    pausedTicks = SDL_GetTicks() - startTicks;
  }
}
void Timer::unpause() {
  if (paused == true) {
    paused = false;
    startTicks = SDL_GetTicks() - pausedTicks;
    pausedTicks = 0;
  }
}
int Timer::get_ticks() {
  if (started == true) {
    if (paused == true) { return pausedTicks; }
    else { return SDL_GetTicks() - startTicks; }
  }
  return 0;
}
bool Timer::is_started() { return started; }
bool Timer::is_paused() { return paused; }
//*** CHARACTER
Character::Character() {
  box.x = 0;
  box.y = 0;
  box.w = CHAR_SPRITE_WIDTH;
  box.h = CHAR_SPRITE_HEIGHT;
  xVel = 0;
  yVel = 0;
  frame = 0;
  status = DIR_DOWN;
}
void Character::handle_events() {
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
      case SDLK_LEFT: xVel -= 6; break;
      case SDLK_RIGHT: xVel += 6; break;
      case SDLK_UP: yVel -= 6; break;
      case SDLK_DOWN: yVel += 6; break;
    }//end switch
  }//end keydown
  else if (event.type == SDL_KEYUP) {
    switch (event.key.keysym.sym) {
      case SDLK_LEFT: xVel += 6; break;
      case SDLK_RIGHT: xVel -= 6; break;
      case SDLK_UP: yVel += 6; break;
      case SDLK_DOWN: yVel -= 6; break;
    }//end switch
  }//end keyup
}
void Character::move(Tile *tiles[]) {
  box.x += xVel;
  if ((box.x < 0)||(box.x + CHAR_SPRITE_WIDTH > ZONE_WIDTH)||touches_wall(box,tiles)) { box.x -= xVel; }
  box.y += yVel;
  if ((box.y < 0)||(box.y + CHAR_SPRITE_HEIGHT > ZONE_HEIGHT)||touches_wall(box,tiles)) { box.y -= yVel; }
}
void Character::show() {
  if (xVel < 0) { status = DIR_LEFT; frame++; }
  else if (xVel > 0) { status = DIR_RIGHT; frame++; }
  else if (yVel < 0) { status = DIR_UP; frame++; }
  else if (yVel > 0) { status = DIR_DOWN; frame++; }
  
  if (frame >= 3) { frame = 0; }
  
  if (status == DIR_LEFT) { apply_surface(box.x - camera.x, box.y - camera.y, mainCharSpriteSheet, screen, &mainClipsLeft[frame]); }
  else if (status == DIR_RIGHT) { apply_surface(box.x - camera.x, box.y - camera.y, mainCharSpriteSheet, screen, &mainClipsRight[frame]); }
  else if (status == DIR_UP) { apply_surface(box.x - camera.x, box.y - camera.y, mainCharSpriteSheet, screen, &mainClipsUp[frame]); }
  else if (status == DIR_DOWN) { apply_surface(box.x - camera.x, box.y - camera.y, mainCharSpriteSheet, screen, &mainClipsDown[frame]); }
}
void Character::set_camera() {
  camera.x = (box.x + CHAR_SPRITE_WIDTH / 2) - SCREEN_WIDTH / 2;
  camera.y = (box.y + CHAR_SPRITE_HEIGHT / 2) - SCREEN_HEIGHT / 2;
  
  if (camera.x < 0) { camera.x = 0; }
  if (camera.y < 0) { camera.y = 0; }
  if (camera.x > ZONE_WIDTH - camera.w) { camera.x = ZONE_WIDTH - camera.w; }
  if (camera.y > ZONE_HEIGHT - camera.h) { camera.y = ZONE_HEIGHT - camera.h; }
}
void Character::set_x(int X) { box.x = X; }
void Character::set_y(int Y) { box.y = Y; }
void Character::set_frame(int F) { frame = F; }
void Character::set_status(int S) { status = S; }

int Character::get_x() { return box.x; }
int Character::get_y() { return box.y; }
int Character::get_frame() { return frame; }
int Character::get_status() { return status; }
//***END CLASS FUNCTIONS***
//*******************************\\
//***MAIN
int main(int argc, char* args[]) {

bool quit = false;
Character mainChar;
Tile *tiles[TOTAL_TILES];
Timer fps;

if (init() == false) { return 1; }
if (load_files() == false) { return 1; }
set_clips();
if (set_tiles(tiles) == false) { return 1; }

while (quit == false) {
  //*** EVENTS ***
  fps.start();
  while (SDL_PollEvent(&event)) {
    mainChar.handle_events();
    if (event.type == SDL_QUIT) { quit = true; }
  }
  
  //*** LOGIC ***
  mainChar.set_camera();
  mainChar.move(tiles);
  
  
  //*** RENDER ***
  for (int t = 0; t < TOTAL_TILES; t++) { tiles[t]->show(); }
  mainChar.show();
  
  if (SDL_Flip(screen) == -1) { return 1; }
  //FPS tracker
  if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND) {
    SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
  }
}//end game loop
clean_up(tiles);
return 0;
}
