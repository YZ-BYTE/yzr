#include "ckernal.h"
#include "SDL.h"

#undef main

int main(int argc, char *argv[])
{
    SDL_SetMainReady();
    CKernal &kernal = CKernal::getInstance();
    kernal.initialize(argc, argv);
    return kernal.run();
}
