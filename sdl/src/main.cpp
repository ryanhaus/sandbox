#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_stdinc.h>
#include <stdexcept>
#include <string>
#include <cmath>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <implot.h>

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

    // setup ImGui & ImPlot
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    ImPlot::CreateContext();

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

        static float samples[512] = { 0 };
        static float freq = 440.0f;
        static float phase = 0.0f;

        if (SDL_GetAudioStreamQueued(stream) < sizeof(samples))
        {
            static unsigned int n = 0;

            for (int i = 0; i < SDL_arraysize(samples); i++)
            {
                samples[i] = 0.25 * SDL_sinf(phase);
                phase += 2.0 * SDL_PI_F * freq / 8000.0f;
            }

            phase = fmod(phase, 2.0 * SDL_PI_F);

            SDL_PutAudioStreamData(stream, samples, sizeof(samples));
        }

        // setup imgui for rendering 
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // create things in window
        ImGui::Begin("Test");

        ImGui::Text("Select frequency:");
        ImGui::SliderFloat("float", &freq, 0.0f, 1000.0f);

        if (ImPlot::BeginPlot("Audio plot"))
        {
            ImPlot::PlotLine("Audio samples", samples, SDL_arraysize(samples));
            ImPlot::EndPlot();
        }

        ImGui::End();


        // render
        ImGui::Render();

        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // exit
    ImPlot::DestroyContext();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
