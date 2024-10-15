#include "SoundManager.hpp"

void SPlayer::playSound(sf::Sound &sound1, const sf::SoundBuffer &buffer){

    if(isPlaying){
        sound1.stop();
        isPlaying = false;
    }

    {
    std::unique_lock<std::mutex> lock(m);
    std::cout<<"loading song!"<<std::endl;

    sound1.setBuffer(buffer);
    
    sound1.play();
    if(sound1.getStatus() == sf::Sound::Playing){
        isPlaying = true;
    }else{
        printf("err playing sound\n");
        return;
    }
    }

    while(sound1.getStatus() == sf::Sound::Playing){
        sf::sleep(sf::milliseconds(500));
        // if(sound2.getStatus() == sf::Sound::Playing){
        //     printf("sound2 playing");
        //     sound2.stop();
        // }
    }
}

void SPlayer::playS1(const sf::SoundBuffer &buffer){
        playSound(sound, buffer);
}


void SPlayer::playS2(const sf::SoundBuffer &buffer){

    playSound(sound, buffer);

}