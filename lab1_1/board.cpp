#include "board.h"
#include <time.h>
#include <string>
#include "framework.h"

board::board(int size)
{
	grid_size = size;
	b_fields = new field* [grid_size];
	for (int i = 0; i < grid_size; i++)
	{
		b_fields[i] = new field[grid_size];
	}

	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			b_fields[i][j] = field(field_state::empty);
		}
	}
	ships = new ship* [10];
}

board::~board()
{
	for (int i = 0; i < grid_size; i++)
	{
		delete[] b_fields[i];
	}
	delete[] b_fields;
}

void board::change_size(int new_size)
{
	for (int i = 0; i < grid_size; ++i) {
		delete[] b_fields[i];
	}
	delete[] b_fields;

	grid_size = new_size;
	b_fields = new field * [grid_size];
	for (int i = 0; i < grid_size; ++i) {
		b_fields[i] = new field[grid_size];
		for (int j = 0; j < grid_size; ++j) {
			b_fields[i][j] = field(field_state::empty);
		}
	}
}

field* board::operator[](int index) {
	if (index >= 0 && index < grid_size) {
		return b_fields[index];
	}
	else {
		return nullptr;
	}
}

board& board::operator=(const board& other) {
	if (this != &other) {
		for (int i = 0; i < grid_size; ++i) {
			delete[] b_fields[i];
		}
		delete[] b_fields;

		grid_size = other.grid_size;
		b_fields = new field * [grid_size];
		for (int i = 0; i < grid_size; ++i) {
			b_fields[i] = new field[grid_size];
			for (int j = 0; j < grid_size; ++j) {
				b_fields[i][j] = other.b_fields[i][j];
			}
		}
		ships = other.ships;
	}
	return *this;
}

board::board(const board& other) : grid_size(other.grid_size) {
	b_fields = new field * [grid_size];
	for (int i = 0; i < grid_size; ++i) {
		b_fields[i] = new field[grid_size];
		for (int j = 0; j < grid_size; ++j) {
			b_fields[i][j] = other.b_fields[i][j];
		}
	}
	ships = other.ships;
}

ship::ship(ship_type type)
{
	this->type = type;
	this->hits = 0;
	this->size = 0;
	switch (type)
	{
	case ship_type::destroyer:
		this->size = 1;
		break;
	case ship_type::cruiser:
		this->size = 2;
		break;
	case ship_type::battleship:
		this->size = 3;
		break;
	case ship_type::carrier:
		this->size = 4;
		break;
	}
}

bool ship::is_sunk()
{
	return hits == size;
}

void ship::hit()
{
	hits++;
	auto str = L"Ship hit: " + std::to_wstring(this->hits) + L"/" + std::to_wstring(this->size);
	OutputDebugString(str.c_str());
}

int ship::get_size()
{
	return size;
}

int ship::get_hits()
{
	return hits;
}

bool board::check_if_viable(int row, int column, ship* ship_to_place, int direction)
{
	if (direction == 0) {
		if (column > 0) {
			if (b_fields[row][column - 1].s != nullptr) {
				return false;
			}
			if (row > 0) {
				if (b_fields[row - 1][column - 1].s != nullptr) {
					return false;
				}
			}
			if (row + 1 < grid_size) {
				if (b_fields[row + 1][column - 1].s != nullptr) {
					return false;
				}
			}
		}
		if (column + ship_to_place->size < grid_size) {
			if (b_fields[row][column + ship_to_place->size].s != nullptr) {
				return false;
			}
			if (row > 0) {
				if (b_fields[row - 1][column + ship_to_place->size].s != nullptr) {
					return false;
				}
			}
			if (row + 1 < grid_size) {
				if (b_fields[row + 1][column + ship_to_place->size].s != nullptr) {
				return false;
				}
			}
		}
	}
	else {
		if (row > 0) {
			if (b_fields[row - 1][column].s != nullptr) {
				return false;
			}
			if (column > 0) {
				if (b_fields[row - 1][column - 1].s != nullptr) {
					return false;
				}
			}
			if (column + 1 < grid_size) {
				if (b_fields[row - 1][column + 1].s != nullptr) {
					return false;
				}
			}
		}
		if (row + ship_to_place->size < grid_size) {
			if (b_fields[row + ship_to_place->size][column].s != nullptr) {
				return false;
			}
			if (column > 0) {
				if (b_fields[row + ship_to_place->size][column - 1].s != nullptr) {
					return false;
				}
			}
			if (column + 1 < grid_size) {
				if (b_fields[row + ship_to_place->size][column + 1].s != nullptr) {
					return false;
				}
			}
		}
	}
	return true;
}

void board::place_ship(ship* ship_to_place, int i)
{
	this->ships[i] = ship_to_place;
	int random_row = rand() % grid_size;
	int random_column = rand() % grid_size;
	int random_direction = rand() % 2;
	bool is_valid = false;
	while (!is_valid) {
		if (random_direction == 1) {
			if (random_row + ship_to_place->size < grid_size) {
				is_valid = true;
				for (int i = 0; i < ship_to_place->size; ++i) {
					if (b_fields[random_row + i][random_column].type == field_state::ship) {
						is_valid = false;
						break;
					}
				}
				if (random_column > 0 && is_valid)
				{
					for (int i = 0; i < ship_to_place->size; ++i) {
						if (b_fields[random_row + i][random_column - 1].type == field_state::ship) {
							is_valid = false;
							break;
						}
					}
				}
				if (random_column + 1 < grid_size && is_valid)
				{
					for (int i = 0; i < ship_to_place->size; ++i) {
						if (b_fields[random_row + i][random_column + 1].type == field_state::ship) {
							is_valid = false;
							break;
						}
					}
				}
				is_valid = is_valid && check_if_viable(random_row, random_column, ship_to_place, random_direction) && is_valid;
				if (is_valid) {
					for (int i = 0; i < ship_to_place->size; ++i) {
						b_fields[random_row + i][random_column].s = ship_to_place;
						b_fields[random_row + i][random_column].type = field_state::ship;
					}
				}
			}
		}
		else {
			if (random_column + ship_to_place->size < grid_size) {
				is_valid = true;
				for (int i = 0; i < ship_to_place->size; ++i) {
					if (b_fields[random_row][random_column + i].s != nullptr) {
						is_valid = false;
						break;
					}
				}
				if (random_row > 0 && is_valid)
				{
					for (int i = 0; i < ship_to_place->size; ++i) {
						if (b_fields[random_row - 1][random_column + i].s != nullptr) {
							is_valid = false;
							break;
						}
					}
				}
				if (random_row + 1 < grid_size && is_valid)
				{
					for (int i = 0; i < ship_to_place->size; ++i) {
						if (b_fields[random_row + 1][random_column + i].s != nullptr) {
							is_valid = false;
							break;
						}
					}
				}
				is_valid = is_valid && check_if_viable(random_row, random_column, ship_to_place, random_direction);
				if (is_valid) {
					for (int i = 0; i < ship_to_place->size; ++i) {
						b_fields[random_row][random_column + i].s = ship_to_place;
						b_fields[random_row][random_column + i].type = field_state::ship;
					}
				}
			}
		}
		if (!is_valid) {
			random_row = rand() % grid_size;
			random_column = rand() % grid_size;
			random_direction = rand() % 2;
		}
	}
}

void board::place_ships()
{
	place_ship(new ship(ship_type::carrier), 0);
	place_ship(new ship(ship_type::battleship), 1);
	place_ship(new ship(ship_type::battleship), 2);
	place_ship(new ship(ship_type::cruiser), 3);
	place_ship(new ship(ship_type::cruiser), 4);
	place_ship(new ship(ship_type::cruiser), 5);
	place_ship(new ship(ship_type::destroyer), 6);
	place_ship(new ship(ship_type::destroyer), 7);
	place_ship(new ship(ship_type::destroyer), 8);
	place_ship(new ship(ship_type::destroyer), 9);
}

bool board::check_win()
{
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			if (b_fields[i][j].type == field_state::ship)
			{
				return false;
			}
		}
	}
	return true;
}

bool in(field* val, field** arr, int size) {
	for (int i = 0; i < size; ++i) {
		if (arr[i] == val) {
			return true;
		}
	}
	return false;
}

void board::set_neighbours(ship* s)
{
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			if (b_fields[i][j].s == s)
			{
				if (i > 0 and b_fields[i-1][j].type == field_state::empty)
				{
					b_fields[i - 1][j].type = field_state::neutral;
				}
				if (i < grid_size - 1 and b_fields[i + 1][j].type == field_state::empty)
				{
					b_fields[i + 1][j].type = field_state::neutral;
				}
				if (j > 0 and b_fields[i][j - 1].type == field_state::empty)
				{
					b_fields[i][j - 1].type = field_state::neutral;
				}
				if (j < grid_size - 1 and b_fields[i][j + 1].type == field_state::empty)
				{
					b_fields[i][j + 1].type = field_state::neutral;
				}
				if (j < grid_size - 1 and i < grid_size - 1 and b_fields[i + 1][j + 1].type == field_state::empty)
				{
					b_fields[i][j + 1].type = field_state::neutral;
				}
				if (j > 0 and i > 0 and b_fields[i - 1][j - 1].type == field_state::empty)
				{
					b_fields[i - 1][j - 1].type = field_state::neutral;
				}
				if (j < grid_size - 1 and i > 0 and b_fields[i - 1][j + 1].type == field_state::empty)
				{
					b_fields[i - 1][j + 1].type = field_state::neutral;
				}
				if (j > 0 and i < grid_size - 1 and b_fields[i + 1][j - 1].type == field_state::empty)
				{
					b_fields[i + 1][j - 1].type = field_state::neutral;
				}
			}
		}
	}
}