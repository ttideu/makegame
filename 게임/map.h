#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 40
#define NUM_MAPS 5  // 맵의 개수 (필요한 만큼 조정하세요)

extern int map[MAP_HEIGHT][MAP_WIDTH];
extern int currentMapIndex;

void LoadMap(int index);
void NextMap();

#endif