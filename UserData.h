//#include "FrictionlessPlane.h"



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
	double health;
	User () {
		room = location::A;
		has_key = false;
		health = 100.0;
	}

	enum location getRoom() {
		return room;
	}
	bool hasKey() {
		return has_key;
	}
	double getHealth() {
		return health;
	}
};