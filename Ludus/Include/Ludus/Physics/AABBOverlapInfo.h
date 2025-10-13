#pragma once

struct AABBOverlapInfo
{
	bool IsARightOfB;
	bool IsALeftOfB;
	bool IsAAboveB;
	bool IsABelowB;
	bool IsXAxisCollision;
	bool IsYAxisCollision;
};
