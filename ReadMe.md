# Sine

Sine is an unfinished remake of Asteroids that I've made using HolyC.

## Screenshots

| In-game                                                | High score entry                                                                                |
|--------------------------------------------------------|-------------------------------------------------------------------------------------------------|
| ![Firing at some asteroids in Sine](Media/In-game.png) | ![Entering initials on the high score screen after beating a score](Media/High-score-entry.png) |

## Download

An ISO image for TempleOS can be found [here](https://github.com/slx7R4GDZM/Sine/releases).

## Default controls

| Action         | Key              |
|----------------|------------------|
| 1 Player Start | <kbd>1</kbd>     |
| 2 Player Start | <kbd>2</kbd>     |
| Rotate Left    | <kbd>A</kbd>     |
| Rotate Right   | <kbd>D</kbd>     |
| Thrust         | <kbd>W</kbd>     |
| Fire Photon    | <kbd>E</kbd>     |
| Hyperspace     | <kbd>Space</kbd> |
| Left Coin      | <kbd>🡨</kbd>     |
| Center Coin    | <kbd>🡫</kbd>     |
| Right Coin     | <kbd>🡪</kbd>     |
| Resize Window  | <kbd>F5</kbd>    |
| Exit           | <kbd>Esc</kbd>   |

## Running

After inserting the ISO image, you can run it from the disc:

    Cd("T:/Sine");
    #include "Main";

Or you can copy the directory to disk to run it:

    CopyTree("T:", ".");
    Cd("Sine");
    #include "Main";

## What's with the strange constants?

The vector and text constants were made with [Sine Toolkit](https://github.com/slx7R4GDZM/Sine-Toolkit).

## License information

This project is licensed under the terms of the [MIT License](License.TXT).
