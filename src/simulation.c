#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "simulation.h"
#include "renderer.h"
#include "constants.h"
#include "physics.h"

bool simulation_init(Simulation *sim, const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    sim->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (sim->window == NULL) {
        return false;
    }

    sim->renderer = SDL_CreateRenderer(sim->window, -1, SDL_RENDERER_ACCELERATED);
    if (sim->renderer == NULL) {
        return false;
    }

    return true;
}

void simulation_run(Simulation *sim) {
    sim->running = true;
    
    Pendulum pendulum;
    if (!pendulum_init(&pendulum)) {
        printf("Failed initialization of pendulum\n");
        return;
    }
    if (!append_node(pendulum.root)) {
        printf("Failed to append a node\n");
        return;
    }

    bool first_node_attached = false;

    SDL_Event e;
    while (sim->running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                sim->running = false;
                break;
            }
            if (!first_node_attached) {
                if (e.type == SDL_MOUSEMOTION) {
                    pendulum.root->child->x = e.motion.x;
                    pendulum.root->child->y = e.motion.y;
                }
                if (e.type == SDL_MOUSEBUTTONUP) {
                    first_node_attached = true;
                    new_node_init_values(pendulum.root->child);
                }
            }
        }

        if (first_node_attached) {
            if (pendulum.root->child->rod_length == 0) {
                printf("error: division by 0\n");
                return;
            }
            calculate_node_pos(pendulum.root->child);
        }

        draw_background(sim->renderer);
        draw_pendulum(sim->renderer, &pendulum);
        
        SDL_RenderPresent(sim->renderer);
        SDL_Delay(16);
    }

    free_nodes(&pendulum);

    return;
}

void simulation_clean(Simulation *sim) {
    SDL_DestroyWindow(sim->window);
    SDL_DestroyRenderer(sim->renderer);
    SDL_Quit();

    return;
}
