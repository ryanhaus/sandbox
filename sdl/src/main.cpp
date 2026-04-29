#include <SDL3/SDL_audio.h>
#include <stdexcept>
#include <string>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

int main()
{
    // init
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        throw std::runtime_error("Could not initialize SDL");

    // create window
    SDL_Window* window = SDL_CreateWindow("SDL test", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
        throw std::runtime_error("Could not create SDL window: " + std::string(SDL_GetError()));

    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
        throw std::runtime_error("Could not create SDL renderer: " + std::string(SDL_GetError()));
    
    // setup audio
    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = 1,
        .freq = 8000,
    };

    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!stream)
        throw std::runtime_error("Could not create SDL audio stream: " + std::string(SDL_GetError()));

    SDL_ResumeAudioStreamDevice(stream);

    // setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // main loop
    bool active = true;
    while(active)
    {
        // go through events
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT || e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
                active = false;
        }

        // handle audio
        const int minimum_audio = (8000 * sizeof(float)) / 2;

        if (SDL_GetAudioStreamQueued(stream) < minimum_audio)
        {
            static unsigned int n = 0;
            static float samples[512];

            for (int i = 0; i < SDL_arraysize(samples); i++)
            {
                const int freq = 440;
                
                samples[i] = 0.25 * SDL_sinf(2.0 * SDL_PI_F * freq * n / 8000.0);
                n++;
            }

            n %= 8000;

            SDL_PutAudioStreamData(stream, samples, sizeof(samples));
        }

        // setup imgui for rendering 
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // create things in window
        ImGui::Begin("Test");
        ImGui::Text("Test");
        ImGui::End();

        // render
        ImGui::Render();

        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // exit
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
