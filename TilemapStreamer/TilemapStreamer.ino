#include <Gamebuino-Meta.h>

struct Point
{
	int16_t x, y;
	Point(int16_t x = 0, int16_t y = 0) : x(x), y(y){}
};

File file;

Point cameraPosition;
Point cursorPosition = Point(16, 16);
bool placingTiles = false;
int32_t previousTile;
uint8_t currentTileType;

uint8_t mapWidth = 0;
uint8_t mapHeight = 0;

const uint8_t BUFFERSIZE = 80;
uint8_t buffer[4][BUFFERSIZE];

const uint16_t tilesetData[] = {8,8,2,0,0,0,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,0xdf7a,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd3e5,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xd3e5,0xd229,0xd3e5,0xd553,0xd553,0xd553,0xd553,0xd3e5,0xd229,0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,0xd229,0xd3e5,0x8266,0x8266,0x8266,0x8266,0xd3e5,0xd229,0xd3e5,0x8266,0x8266,0x8266,0x8266,0x8266,0x8266,0xd3e5};
Image tilesetImage = Image(tilesetData);

const uint16_t cursorData[] = {7,9,0,0,0xf81f,0,0x84b4,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0xf81f,0x84b4,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0x84b4,0xdf7a,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,0x84b4,0xdf7a,0xdf7a,0xdf7a,0x84b4,0xf81f,0xf81f,0x84b4,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0x84b4,0xf81f,0x84b4,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0x84b4,0x84b4,0xdf7a,0xdf7a,0x84b4,0x84b4,0x84b4,0x84b4,0x84b4,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0x84b4,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0xf81f};
Image cursorImage = Image(cursorData);

void setup()
{
	gb.begin();

	// Open file for reading and writing
	file = SD.open("Test.map", O_RDWR);

	// Get map width and height
	mapWidth = file.read();
	mapHeight = file.read();
	
	// Fill buffers with all tiles currently in view
	file.seek(2);
	file.read(buffer[0], BUFFERSIZE);
	file.read(buffer[1], BUFFERSIZE);

	file.seek((mapWidth * BUFFERSIZE) + 2);
	file.read(buffer[2], BUFFERSIZE);
	file.read(buffer[3], BUFFERSIZE);
}

void loop()
{
	if(gb.update())
	{
		gb.display.fill(WHITE);

		// Draw buffers
		for(uint8_t i = 0; i < BUFFERSIZE; i++)
		{
			tilesetImage.setFrame(buffer[0][i]);
			gb.display.drawImage((((i % 10) * 8) - cameraPosition.x - 80) % 160 + 80, (((i / 10) * 8) - cameraPosition.y - 64) % 128 + 64, tilesetImage);
		}

		for(uint8_t i = 0; i < BUFFERSIZE; i++)
		{
			tilesetImage.setFrame(buffer[1][i]);
			gb.display.drawImage((((i % 10) * 8) - cameraPosition.x) % 160 + 80, (((i / 10) * 8) - cameraPosition.y - 64) % 128 + 64, tilesetImage);
		}

		for(uint8_t i = 0; i < BUFFERSIZE; i++)
		{
			tilesetImage.setFrame(buffer[2][i]);
			gb.display.drawImage((((i % 10) * 8) - cameraPosition.x - 80) % 160 + 80, (((i / 10) * 8) - cameraPosition.y) % 128 + 64, tilesetImage);
		}

		for(uint8_t i = 0; i < BUFFERSIZE; i++)
		{
			tilesetImage.setFrame(buffer[3][i]);
			gb.display.drawImage((((i % 10) * 8) - cameraPosition.x) % 160 + 80, (((i / 10) * 8) - cameraPosition.y) % 128 + 64, tilesetImage);
		}

		// Draw cursor and tile selection rect
		gb.display.setColor((Color) 0x6d45);
		gb.display.drawRect((cursorPosition.x / 8 * 8) - cameraPosition.x, (cursorPosition.y / 8 * 8) - cameraPosition.y, 8, 8);
		gb.display.drawImage(cursorPosition.x - cameraPosition.x, cursorPosition.y - cameraPosition.y, cursorImage);

		// Draw stats
		gb.display.setColor((Color) 0x1063);
		gb.display.setCursor(1, 1);
		gb.display.print("CPU: ");
		gb.display.print(gb.getCpuLoad());
		gb.display.print(" RAM: ");
		gb.display.print(gb.getFreeRam());

		// Handle movement
		if(gb.buttons.repeat(BUTTON_UP, 1)) cursorPosition.y -= 2;
		if(gb.buttons.repeat(BUTTON_DOWN, 1)) cursorPosition.y += 2;
		if(gb.buttons.repeat(BUTTON_LEFT, 1)) cursorPosition.x -= 2;
		if(gb.buttons.repeat(BUTTON_RIGHT, 1)) cursorPosition.x += 2;

		cursorPosition = Point(constrain(cursorPosition.x, 0, (mapWidth * 80) - 1), constrain(cursorPosition.y, 0, (mapHeight * 64) - 1));

		Point previousCameraPosition = cameraPosition;
		cameraPosition = Point(constrain(cursorPosition.x - 40, 0, (mapWidth - 1) * 80), constrain(cursorPosition.y - 32, 0, (mapHeight - 1) * 64));

		// Handle editing
		if(gb.buttons.repeat(BUTTON_A, 1))
		{
			Point cursorPositionNormalized = Point(cursorPosition.x / 8, cursorPosition.y / 8);

			uint16_t currentBlock = (cursorPositionNormalized.x / 10) + ((cursorPositionNormalized.y / 8) * mapWidth);

			Point tileInBlock = Point(cursorPositionNormalized.x - (cursorPositionNormalized.x / 10 * 10), cursorPositionNormalized.y - (cursorPositionNormalized.y / 8 * 8));

			int32_t currentTile = (currentBlock * BUFFERSIZE) + 2 + (tileInBlock.x + (tileInBlock.y * 10));
			
			if(currentTile != previousTile)
			{
				file.seek(currentTile);

				if(!placingTiles)
				{
					currentTileType = (file.peek() == 1)?0:1;
					placingTiles = true;
				}

				file.write(currentTileType);
				file.flush();

				file.seek((((currentTile - 2) / 80) * 80) + 2);

				Point currentTileInBlock = Point(((currentTile - 2) / 80) % mapWidth, ((currentTile - 2) / 80) / mapWidth);

				file.read((currentTileInBlock.x % 2)?buffer[(currentTileInBlock.y % 2)?3:1]:buffer[(currentTileInBlock.y % 2)?2:0], BUFFERSIZE);

				previousTile = currentTile;
			}
		}
		if(gb.buttons.released(BUTTON_A))
		{
			placingTiles = false;
			previousTile = -1;
		}

		// Update buffers
		if(previousCameraPosition.x / 80 != cameraPosition.x / 80 || previousCameraPosition.y / 64 != cameraPosition.y / 64)
		{
			Point cameraPositionNormalized = Point(cameraPosition.x / 80, cameraPosition.y / 64);

			// Horizontal change
			if(previousCameraPosition.x / 80 != cameraPosition.x / 80)
			{
				if((cameraPosition.x / 80) - (previousCameraPosition.x / 80) > 0) // Moved right
				{
					file.seek((((cameraPositionNormalized.x + 1) + (mapWidth * cameraPositionNormalized.y)) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.x % 2)?buffer[(cameraPositionNormalized.y % 2)?2:0]:buffer[(cameraPositionNormalized.y % 2)?3:1], BUFFERSIZE);

					file.seek((((cameraPositionNormalized.x + 1 + mapWidth) + (mapWidth * cameraPositionNormalized.y)) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.x % 2)?buffer[(cameraPositionNormalized.y % 2)?0:2]:buffer[(cameraPositionNormalized.y % 2)?1:3], BUFFERSIZE);
				}
				else // Moved left
				{
					file.seek(((cameraPositionNormalized.x + (mapWidth * cameraPositionNormalized.y)) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.x % 2)?buffer[(cameraPositionNormalized.y % 2)?3:1]:buffer[(cameraPositionNormalized.y % 2)?2:0], BUFFERSIZE);

					file.seek((((cameraPositionNormalized.x + mapWidth) + (mapWidth * cameraPositionNormalized.y)) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.x % 2)?buffer[(cameraPositionNormalized.y % 2)?1:3]:buffer[(cameraPositionNormalized.y % 2)?0:2], BUFFERSIZE);
				}
			}

			// Vertical change
			if(previousCameraPosition.y / 64 != cameraPosition.y / 64)
			{
				if((cameraPosition.y / 64) - (previousCameraPosition.y / 64) > 0)
				{
					file.seekSet(((((cameraPositionNormalized.y + 1) * mapWidth) + cameraPositionNormalized.x) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.y % 2)?buffer[(cameraPositionNormalized.x % 2)?1:0]:buffer[(cameraPositionNormalized.x % 2)?3:2], BUFFERSIZE);

					file.seekSet(((((cameraPositionNormalized.y + 1) * mapWidth) + cameraPositionNormalized.x + 1) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.y % 2)?buffer[(cameraPositionNormalized.x % 2)?0:1]:buffer[(cameraPositionNormalized.x % 2)?2:3], BUFFERSIZE);
				}
				else
				{
					file.seekSet((((cameraPositionNormalized.y * mapWidth) + cameraPositionNormalized.x) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.y % 2)?buffer[(cameraPositionNormalized.x % 2)?3:2]:buffer[(cameraPositionNormalized.x % 2)?1:0], BUFFERSIZE);

					file.seekSet((((cameraPositionNormalized.y * mapWidth) + cameraPositionNormalized.x + 1) * BUFFERSIZE) + 2);
					file.read((cameraPositionNormalized.y % 2)?buffer[(cameraPositionNormalized.x % 2)?2:3]:buffer[(cameraPositionNormalized.x % 2)?0:1], BUFFERSIZE);
				}
			}
		}
	}
}