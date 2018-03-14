## TILEMAP STREAMER for the Gamebuino META
[Link to creation and topic for discussion](https://gamebuino.com/creations/tilemap-streamer)

## What is it?
This tech demo lets you move around a map of 512x512 tiles (8x8 pixels each) which gets streamed from the SD card in real time. It also lets you add/remove blocks and these changes will be saved to the SD card right away.

## How does it work?
There's a tilebuffer of 11x9 tiles which is one tile more in both directions than what is needed to fill the screen so the camera has some freedom to move around within this buffer smoothly. When the camera moves outside this buffer, all tiles in the tilebuffer gets shifted in the opposite direction and a new strip of tiles get streamed from the SD card file and put into the tilebuffer.

To make reading these strips from the SD card faster, the tilemap data is stored in the SD card file twice: once for reading horizontal strips from left to right and again for reading vertical strips that same way.

Writing changes to the SD card is as simple as calculating the current block position you're about to edit and writing a new value to both the SD card file and tilebuffer.

## Controls
D-pad: Move around
A button: Add/remove block
