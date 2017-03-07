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
	bool opening_lr_door;
	bool opening_fb_door;
	double health;
	bool petting_cat;
	bool free;
	HDdouble max_force;
	User () {
		room = location::A;
		has_key = false;
		room_transition = false;
		opening_lr_door = false;
		opening_fb_door = false;
		health = 100.0;
		petting_cat = false;
		free = false;
		max_force = 100;
	}

	enum location getRoom() {
		return room;
	}
	double getHealth() {
		return health;
	}
};