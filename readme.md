## TILEMAP STREAMER for the Gamebuino META
[Link to creation and topic for discussion](https://gamebuino.com/creations/tilemap-streamer)

## WHAT IS IT?
This tech demo lets you move around a large map (size: 3840x3072 pixels, 480x384 tiles of 8x8 pixels each or 48x48 blocks of 80x64 pixels each) that gets streamed from the SD card in real time. It also lets you add/remove blocks and these changes will be saved to the SD card right away.

## HOW DOES IT WORK?
There are 4 buffers each the size of the screen (that's 10x8 tiles). As you move around the map these buffers get swapped around and filled with new parts of the map to make it seem like it's one big map.

Making changes to the map is as simple as translating the cursor position to the position in the SD card file and writing the new value to it.

## CONTROLS
D-pad: Move around

A button: Add/remove block

## OPTIMISATIONS
Currently all 4 buffers get looped through to draw the tiles in them even when they are only partially visible or not visible at all. I'm sure it's possible to improve this to lower CPU load.
