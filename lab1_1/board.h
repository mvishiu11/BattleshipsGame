#pragma once
#define NOMINMAX
#include <array>
#include "framework.h"

enum class field_state
{
	empty,
	ship,
	miss,
	hit,
	neutral
};

enum class ship_type
{
	no_type = 0,
	destroyer = 1,
	cruiser = 2,
	battleship = 3,
	carrier = 4
};

class ship
{
public:
			int size;
			int hits;
			ship_type type;
			ship(ship_type type);
			ship() : size(0), hits(0), type(ship_type::no_type) {}
			bool is_sunk();
			void hit();
			int get_size();
			int get_hits();
};

class field
{
public:
	field_state type = field_state::empty;
	ship *s;
	field() : type(field_state::empty), s(nullptr) {}
	field(field_state t) : type(t), s(nullptr) {}
	field(field_state t, ship* s) : type(t), s(s) {}
	ship_type get_ship_type() { return s->type; };
};

class board
{
public:
	int grid_size;
	field** b_fields;
	ship** ships;
	board(int size);
	~board();
	void change_size(int new_size);
	bool check_edges(int row, int column, ship* ship_to_place, int direction);
	void place_ship(ship* ship_to_place, int i);
	void set_neighbours(ship* s);
	void place_ships();
	bool check_win();
	field* operator[](int index);
	board& operator=(const board& other);
	board(const board& other);
};