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

#if !defined( __CACHE_H__ )
#define __CACHE_H__

#include <vector>
#include <map>

////#include "../Fluid/mmgr.h"

template< class C >
class Cache
{
private:
	std::vector< unsigned int > cache_history;
	C ** cache;
	unsigned short cachesize; // How many elements are available

	unsigned int _maxsize;
	bool _autofree;
public:
	
	void addEntry( unsigned int id, C *data )
	{
		if( id >= cachesize )
			return;

		while( cache_history.size() > _maxsize )
		{
			unsigned int id = *( cache_history.begin() );
			
			if( _autofree )
				delete cache[id];

			cache[id] = 0;
			cache_history.erase( cache_history.begin() );
		}

		cache[id] = data;
		cache_history.push_back( id );
	}

	C *findEntry( unsigned int id )
	{
		if( id >= cachesize )
			return 0;

		return cache[id];
	}

	void setCacheSize( unsigned short elements ) 
	{
		if( autofree() )
		{
			for( unsigned int i = 0; i < cachesize; ++i )
				delete cache[i];
		}

		delete [] cache; cache = NULL;

		cachesize = elements;
		cache = new C*[ elements ];
		memset( cache, 0, elements * sizeof( C* ) );
   	}

	Cache()
	{
		_autofree = false;
		_maxsize = 1;

		cachesize = 0;
		cache = 0;
	}

	void setMaxSize( unsigned int data ) { _maxsize = data; }
	unsigned int maxSize() const { return _maxsize; }

	virtual ~Cache()
	{
		if( _autofree )
		{
			for( unsigned int i = 0; i < cachesize; ++i )
				delete cache[i];
		}
		
		delete [] cache; cache = NULL;
	}

	void setAutofree( bool autofree ) { _autofree = autofree; }
	bool autofree() const { return _autofree; }

  void Clear ()
  {
        setCacheSize (cachesize);
  }
  
};

#endif
