#include "osm.h"
#include "osmparser.h"
#include <libxml/parser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


#include <iostream>
using namespace std;

polygon_types osm_way::ptypes;

bool osm_dataset::load(const char* filename,const std::string& parser)
{
	if (parser=="libxml2")
	{
		return osmparser::parse(this,filename);
	}
	return false;	
}

osm_dataset::~osm_dataset()
{
	for(int count=0; count<ways.size(); count++)
		delete ways[count];
	for(int count=0; count<nodes.size(); count++)
		delete nodes[count];
}

std::string osm_dataset::to_string()
{
	std::string result;

	for(int count=0; count<nodes.size(); count++)
	{
		result += nodes[count]->to_string();
	}
	for(int count=0; count<ways.size(); count++)
	{
		result += ways[count]->to_string();
	}
	return result;
}

bounds osm_dataset::get_bounds()
{
	bounds b (-180,-90,180,90);
	for(int count=0; count<nodes.size();count++)
	{
		if(nodes[count]->lon > b.w)
			b.w=nodes[count]->lon;
		if(nodes[count]->lon < b.e)
			b.e=nodes[count]->lon;
		if(nodes[count]->lat > b.s)
			b.s=nodes[count]->lat;
		if(nodes[count]->lat < b.n)
			b.n=nodes[count]->lat;
	}
	return b;
}

osm_node *osm_dataset::next_node()
{
	if(node_i!=nodes.end())
	{
		return *(node_i++);
	}
	return NULL;
	
}
osm_way *osm_dataset::next_way()
{
	if(way_i!=ways.end())
	{
		return *(way_i++);
	}
	return NULL;
}
		
osm_item *osm_dataset::next_item()
{
	osm_item *item=NULL;
	if(next_item_mode==Node)
	{
		item = next_node();
		if(item==NULL)
		{
			next_item_mode=Way;
			rewind_ways();
			item = next_way();
		}
	}
	else
	{
		item = next_way();
	}
	return item;
}
	
std::set<std::string> osm_dataset::get_keys()
{
	std::set<std::string> keys;
	for(int count=0; count<nodes.size(); count++)
	{
		for(std::map<std::string,std::string>::iterator i=
			nodes[count]->keyvals.begin(); i!=nodes[count]->keyvals.end(); i++)
		{
			keys.insert(i->first);
		}
	}
	for(int count=0; count<ways.size(); count++)
	{
		for(std::map<std::string,std::string>::iterator i=
			ways[count]->keyvals.begin(); i!=ways[count]->keyvals.end(); i++)
		{
			keys.insert(i->first);
		}
	}
	return keys;
}

			
		

std::string osm_item::to_string()
{
	std::ostringstream strm;
	strm << "id=" << id << std::endl << "Keyvals: " << std::endl;
	for(std::map<std::string,std::string>::iterator i=keyvals.begin();
			i!=keyvals.end(); i++)
	{
		strm << "Key " << i->first << " Value " << i->second << std::endl; 
	}
	return strm.str();
}

std::string osm_node::to_string()
{
	std::ostringstream strm;
	strm << "Node: "<< osm_item::to_string() << 
			" Lat=" << lat <<" lon="  <<lon << std::endl;
	return strm.str();
}

std::string osm_way::to_string()
{
	std::ostringstream strm;
	strm << "Way: " << osm_item::to_string() << "Nodes in way:";

	for(int count=0; count<nodes.size(); count++)
	{
		if(nodes[count]!=NULL)
		{
			strm << nodes[count]->id << " ";
		}
	}
	strm << std::endl;
	return strm.str();
}

bounds osm_way::get_bounds()
{
	bounds b (-180,-90,180,90);
	for(int count=0; count<nodes.size();count++)
	{
		if(nodes[count]->lon > b.w)
			b.w=nodes[count]->lon;
		if(nodes[count]->lon < b.e)
			b.e=nodes[count]->lon;
		if(nodes[count]->lat > b.s)
			b.s=nodes[count]->lat;
		if(nodes[count]->lat < b.n)
			b.n=nodes[count]->lat;
	}
	return b;
}

bool osm_way::is_polygon()
{
	for(int count=0; count<ptypes.ptypes.size(); count++)
	{
		if(keyvals.find(ptypes.ptypes[count].first) != keyvals.end() &&
		   keyvals[ptypes.ptypes[count].first] == ptypes.ptypes[count].second)
		{
			return true;
		}
	}
	return false;
}
