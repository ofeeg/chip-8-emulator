#ifndef PLATFORM_H
#define PLATFORM_H

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Platform{
public:
  Platform(char const* title, int window_width, int window_height, int texture_width,int texture_height);
  ~Platform();
  void update(void const * buffer, int pitch);
  bool process_input(unsigned char* keys);
private:
  SDL_Window* window{};
  SDL_Renderer* renderer{};
  SDL_Texture* texture{};
};
#endif
