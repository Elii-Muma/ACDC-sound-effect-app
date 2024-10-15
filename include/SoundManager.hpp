#pragma once

#include <iostream>
#include <SFML/Audio.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>

class SPlayer{
    public:

        std::atomic<bool> isPlaying{false};
        void playSound(sf::Sound &sound1, const sf::SoundBuffer &buffer);
        void playS1(const sf::SoundBuffer &buffer);
        void playS2(const sf::SoundBuffer &buffer);
    private:
        std::mutex m;
        sf::Sound sound;
        sf::SoundBuffer buffer1, buffer2;
};