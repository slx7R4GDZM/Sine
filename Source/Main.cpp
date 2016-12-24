// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Game/Game.h"
#include <cstdlib>
#include <ctime>

int main()
{
    srand(time(nullptr));
    Game game;
}

// TODO
// create formula for asteroid spawn velocity rather than using giant constant arrays
// confirm velocity code for asteroids
// confirm velocity and start position of saucer photons
// figure out small saucer photon aiming
// make ship/other stuff not spawn if they will collide during spawn
// add sound
// figure out how the ship specific explosion graphics works
// implement accurate vector generator and vector ROM emulation to be able to use opcodes like JSR correctly
// add a better cropping mode
// replace more magic numbers with constants
// get rid of passing window and vectorgenerator to everything
// denightmarify spaceobject deletion?
// make the photon spawn position code less stupid
// make the score code less stupid
// create a makefile
// fix some super long lines
// replace pragma onces with include guards and get rid of other non-standard stuff like case ranges
// do some other stuff
