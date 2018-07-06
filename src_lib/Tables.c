/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "Tilengine.h"

#define BLEND_SIZE	(1 << 16)

static uint8_t* _blend_tables[MAX_BLEND];
static int instances = 0;

bool CreateBlendTables (void)
{
	int a,b;
	TLN_Blend c;

	/* increase reference count */
	instances += 1;
	if (instances > 1)
		return true;

	/* get memory */
	for (c=BLEND_MIX25; c<MAX_BLEND; c++)
	{
		_blend_tables[c] = malloc (BLEND_SIZE);
		if (_blend_tables[c] == NULL)
			return false;
	}

	/* build tables */
	for (a=0; a<256; a++)
	{
		for (b=0; b<256; b++)
		{
			const int offset = (a<<8) + b;
			_blend_tables[BLEND_MIX25 ][offset] = (a + b + b) / 3;
			_blend_tables[BLEND_MIX50 ][offset] = (a + b) >> 1;
			_blend_tables[BLEND_MIX75 ][offset] = (a + a + b) / 3;
			_blend_tables[BLEND_ADD   ][offset] = (a+b) > 255? 255 : (a+b);
			_blend_tables[BLEND_SUB   ][offset] = (a-b) < 0? 0 : (a-b);
			_blend_tables[BLEND_MOD   ][offset] = (a*b)/255;
			_blend_tables[BLEND_CUSTOM][offset] = a;
		}
	}
	return true;
}

void DeleteBlendTables (void)
{
	TLN_Blend c;

	/* decrease reference count */
	if (instances > 0)
		instances -= 1;
	if (instances != 0)
		return;

	for (c=BLEND_MIX25; c<MAX_BLEND; c++)
	{
		if (_blend_tables[c] != NULL)
			free (_blend_tables[c]);
	}
}

/* returns blend table according to selected blend mode */
uint8_t* SelectBlendTable (TLN_Blend mode)
{
	return _blend_tables[mode];
}