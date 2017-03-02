// store user data

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
	double health;
	User () {
		room = location::A;
		has_key = false;
		room_transition = false;
		health = 100.0;
	}

	enum location getRoom() {
		return room;
	}
	double getHealth() {
		return health;
	}
};