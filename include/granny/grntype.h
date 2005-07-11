/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#ifndef __GRNTYPE_H__
#define __GRNTYPE_H__

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;


class Point
{
	public:
		Point() {for (int i=0;i<4;i++) points[i]=0.0f;}
		~Point() {}
		float points[4];
};

class gPolygon
{
	public:
		gPolygon() {}
		~gPolygon() {}
		dword nodes[6];
};

#endif
