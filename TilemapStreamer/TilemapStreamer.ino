#include <Gamebuino-Meta.h>

struct Point
{
	int16_t x, y;
	Point(int16_t x = 0, int16_t y = 0) : x(x), y(y){}
};

File file;

uint16_t mapSize = 512;

Point camPosition;
Point oldCamPosition;
Point cursorPosition = Point(16, 16);
uint8_t currentCursorTile;

uint8_t tileBuffer[11][9];
uint8_t tempBuffer[11];

const uint16_t tilesetData[] =
{
	8, 8, 2, 0, 0, 0, 
	0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,
	0xdf7a,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xdf7a,
	0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,
	0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,
	0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,
	0xdf7a,0xd553,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xd553,0xdf7a,
	0xdf7a,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xdf7a,
	0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,
	0xd3e5,0xd553,0xd553,0xd553,0xd553,0xd553,0xd553,0xd3e5,
	0xd229,0xd3e5,0xd553,0xd553,0xd553,0xd553,0xd3e5,0xd229,
	0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,
	0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,
	0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,
	0xd229,0xd229,0xd3e5,0xd3e5,0xd3e5,0xd3e5,0xd229,0xd229,
	0xd229,0xd3e5,0x8266,0x8266,0x8266,0x8266,0xd3e5,0xd229,
	0xd3e5,0x8266,0x8266,0x8266,0x8266,0x8266,0x8266,0xd3e5
};
Image tilesetImage = Image(tilesetData);

const uint16_t cursorData[] =
{
	7, 9, 0, 0, 0xf81f, 0, 
	0x84b4,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0xf81f,
	0x84b4,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,
	0x84b4,0xdf7a,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,
	0x84b4,0xdf7a,0xdf7a,0xdf7a,0x84b4,0xf81f,0xf81f,
	0x84b4,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0x84b4,0xf81f,
	0x84b4,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0xdf7a,0x84b4,
	0x84b4,0xdf7a,0xdf7a,0x84b4,0x84b4,0x84b4,0x84b4,
	0x84b4,0xdf7a,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,
	0x84b4,0x84b4,0xf81f,0xf81f,0xf81f,0xf81f,0xf81f
};
Image cursorImage = Image(cursorData);

void setup()
{
	gb.begin();

	// Open file for reading and writing
	file = SD.open("Test.map", O_RDWR);

	// Fill tilebuffer with all tiles currently in view
	for(uint8_t x = 0; x < 11; x++)
	{
		for(uint8_t y = 0; y < 9; y++)
		{
			file.seek(x + camPosition.x + (mapSize * (camPosition.y + y)));
			tileBuffer[x][y] = file.read();
		}
	}
}

void loop()
{
	if(gb.update())
	{
		gb.display.fill(WHITE);

		// Draw tilebuffer
		for(uint8_t x = 0; x < 11; x++)
		{
			for(uint8_t y = 0; y < 9; y++)
			{
				tilesetImage.setFrame(tileBuffer[x][y] - 48);
				gb.display.drawImage((-camPosition.x % 8) + (x * 8), (-camPosition.y % 8) + (y * 8), tilesetImage);
			}
		}

		// Draw cursor and green selection rect
		gb.display.setColor((Color) 0x6d45);
		gb.display.drawRect((cursorPosition.x / 8 * 8) - camPosition.x, (cursorPosition.y / 8 * 8) - camPosition.y, 8, 8);
		gb.display.drawImage(cursorPosition.x - camPosition.x, cursorPosition.y - camPosition.y, cursorImage);

		// Draw stats
		gb.display.setColor((Color) 0x1063);
		gb.display.setCursor(1, 1);
		gb.display.print("CPU: ");
		gb.display.print(gb.getCpuLoad());
		gb.display.print(" RAM: ");
		gb.display.print(gb.getFreeRam());
		gb.display.setCursor(1, 64 - 6);
		gb.display.print("X: ");
		gb.display.print(camPosition.x);
		gb.display.print(" Y: ");
		gb.display.print(camPosition.y);

		// Handle movement
		if(gb.buttons.repeat(BUTTON_UP, 1)) cursorPosition.y -= 2;
		if(gb.buttons.repeat(BUTTON_DOWN, 1)) cursorPosition.y += 2;
		if(gb.buttons.repeat(BUTTON_LEFT, 1)) cursorPosition.x -= 2;
		if(gb.buttons.repeat(BUTTON_RIGHT, 1)) cursorPosition.x += 2;

		cursorPosition = Point(max(cursorPosition.x, 0), max(cursorPosition.y, 0));

		camPosition = Point(max(cursorPosition.x - 40, 0), max(cursorPosition.y - 32, 0));

		if(oldCamPosition.x / 8 != camPosition.x / 8 || oldCamPosition.y / 8 != camPosition.y / 8)
		{
			updateTilemap();
			oldCamPosition = camPosition;
		}

		// Handle adding/removing tiles
		if(gb.buttons.pressed(BUTTON_A))
		{
			currentCursorTile = tileBuffer[(cursorPosition.x - camPosition.x) / 8][(cursorPosition.y - camPosition.y) / 8];
			currentCursorTile = (currentCursorTile == 48)?49:48;
		}
		else if(gb.buttons.repeat(BUTTON_A, 1))
		{
			if(tileBuffer[(cursorPosition.x - camPosition.x) / 8][(cursorPosition.y - camPosition.y) / 8] != currentCursorTile)
			{
				tileBuffer[(cursorPosition.x - camPosition.x) / 8][(cursorPosition.y - camPosition.y) / 8] = currentCursorTile;

				file.seek((cursorPosition.x / 8) + (mapSize * (cursorPosition.y / 8)));
				file.write(currentCursorTile);

				file.seek((mapSize * mapSize) + (cursorPosition.y / 8) + (mapSize * (cursorPosition.x / 8)));
				file.write(currentCursorTile);
			}
		}
	}
}

void updateTilemap()
{
	Point newPos = Point(camPosition.x / 8, camPosition.y / 8);
	Point oldPos = Point(oldCamPosition.x / 8, oldCamPosition.y / 8);

	// Check for horizontal movement
	if(newPos.x < oldPos.x)
	{
		// Read vertical strip of new tiles to the left and store them into the tempbuffer
		file.seek((mapSize * mapSize) + oldPos.y + (mapSize * (newPos.x)));
		file.read(&tempBuffer, 9);

		// Shift all tiles in tilebuffer to the right and add new tiles on the left
		for(int8_t y = 0; y < 9; y++)
		{
			for(int8_t x = 10; x >= 0; x--)
			{
				if(x > 0) tileBuffer[x][y] = tileBuffer[x - 1][y];
				else tileBuffer[0][y] = tempBuffer[y];
			}
		}
	}
	else if(newPos.x > oldPos.x)
	{
		// Read vertical strip of new tiles to the right and store them into the tempbuffer
		file.seek((mapSize * mapSize) + oldPos.y + (mapSize * (newPos.x + 10)));
		file.read(&tempBuffer, 9);

		// Shift all tiles in tilebuffer to the left and add new tiles on the right
		for(int8_t y = 0; y < 9; y++)
		{
			for(int8_t x = 0; x < 11; x++)
			{
				if(x < 10) tileBuffer[x][y] = tileBuffer[x + 1][y];
				else tileBuffer[10][y] = tempBuffer[y];
			}
		}
	}

	// Check for vertical movement
	if(newPos.y < oldPos.y)
	{
		// Read horizontal strip of new tiles above and store them into the tempbuffer
		file.seek(newPos.x + (mapSize * (newPos.y)));
		file.read(&tempBuffer, 11);

		// Shift all tiles in tilebuffer downwards and add new tiles above
		for(int8_t x = 0; x < 11; x++)
		{
			for(int8_t y = 8; y >= 0; y--)
			{
				if(y > 0) tileBuffer[x][y] = tileBuffer[x][y - 1];
				else tileBuffer[x][0] = tempBuffer[x];
			}
		}
	}
	else if(newPos.y > oldPos.y)
	{
		// Read horizontal strip of new tiles below and store them into the tempbuffer
		file.seek(newPos.x + (mapSize * (newPos.y + 8)));
		file.read(&tempBuffer, 11);

		// Shift all tiles in tilebuffer upwards and add new tiles below
		for(int8_t x = 0; x < 11; x++)
		{
			for(int8_t y = 0; y < 9; y++)
			{
				if(y < 8) tileBuffer[x][y] = tileBuffer[x][y + 1];
				else tileBuffer[x][8] = tempBuffer[x];
			}
		}
	}
}