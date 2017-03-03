// store user data

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

enum location {
	A,
	B,
	C,
	D
};

class User {
public:
	enum location room;
	bool has_key;
	bool room_transition;
	bool opening_door;
	double health;
	User () {
		room = location::A;
		has_key = false;
		room_transition = false;
		opening_door = false;
		health = 100.0;
	}

	enum location getRoom() {
		return room;
	}
	double getHealth() {
		return health;
	}
};