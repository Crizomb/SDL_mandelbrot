#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 750;
const int SCREEN_HEIGHT = 750;
const int CENTER_SCREEN_WIDTH = SCREEN_WIDTH/2;
const int CENTER_SCREEN_HEIGHT = SCREEN_HEIGHT/2;
const double ZOOM_FACTOR = 1.1;

SDL_Window* createWindow(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    SDL_Window* window = SDL_CreateWindow("Braindead Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(-1);
    }
    
    return window;
}


void handle_events(SDL_Event e, bool& quit, bool& cliqued, double* center_offset_x, double* center_offset_y, double* zoom, int* max_itter){
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    
                    *center_offset_y += (double) (mouse_y - CENTER_SCREEN_HEIGHT) / *zoom;
                    *center_offset_x += (double) (mouse_x - CENTER_SCREEN_WIDTH) / *zoom;
                    std::cout << "New center offset " << *center_offset_x << ", " << *center_offset_y << ")\n";
                }

                if (e.button.button == SDL_BUTTON_RIGHT) {
                    *max_itter = (int)((double)*max_itter * 1.1);
                    std::cout << "Increase max_itter :" << *max_itter << "\n";
                }

                if (e.button.button == SDL_BUTTON_MIDDLE) {
                    *max_itter = (int)((double)*max_itter / 1.1);
                    std::cout << "Decrease max_itter :" << *max_itter << "\n";
                }
            }
            else if (e.type == SDL_MOUSEWHEEL){
                if (e.wheel.y == 1){
                    *zoom *= ZOOM_FACTOR;
                    std::cout << "Increase zoom :" << *zoom << "\n";
                }
                else{
                    *zoom /= ZOOM_FACTOR;
                    std::cout << "Decrease zoom :" << *zoom << "\n";
                }
            }
            

        }
}

int mandelbrot_pixel(double cx, double cy, int max_itter){
    double ux = 0.0;
    double uy = 0.0;
    double temp_ux;
    for (int i = 0; i < max_itter; i++) {
        temp_ux = ux;
        ux = ux*ux - uy*uy + cx;
        uy = 2*uy*temp_ux + cy;
        if (ux*ux + uy*uy > 4) return i;
    }
    return 0;
}

void draw_mandelbrot(SDL_Renderer* renderer, double center_offset_x, double center_offset_y, double zoom, int max_itter) {
    double cx, cy;
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            cy = 4.0 * (y + center_offset_y * zoom) / SCREEN_HEIGHT - 2.0;
            cx = 4.0 * (x + center_offset_x * zoom) / SCREEN_WIDTH - 2.0;

            int mandelbrot_escape = mandelbrot_pixel(cx / zoom, cy / zoom, max_itter);

            if (mandelbrot_escape) {
                uint8_t r = mandelbrot_escape % 255;
                uint8_t g = (2*mandelbrot_escape) % 255;
                uint8_t b = (3*mandelbrot_escape) % 255;

                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}


int main() {

    SDL_Window* window = createWindow();
    SDL_Event e;
    bool quit = false;
    bool clicked = false;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    double center_offset_y = 0.0;
    double center_offset_x = 0.0;
    double zoom = 1.0;
    int max_itter = 255;

    while (!quit) {
        handle_events(e, quit, clicked, &center_offset_x, &center_offset_y, &zoom, &max_itter);
        draw_mandelbrot(renderer, center_offset_x, center_offset_y, zoom, max_itter);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
