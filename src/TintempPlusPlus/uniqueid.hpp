#ifndef UID_HPP
#define UID_HPP

/**@brief Unique ID generator, will counts from 1 - n by application life time.
 * source: https://www.oreilly.com/library/view/c-cookbook/0596007612/ch08s09.html
 */
class UniqueID 
{
protected:
	static int nextID;

public:
	int id;

	UniqueID();
	UniqueID(const UniqueID& orig);
	UniqueID& operator=(const UniqueID& orig);
};

int UniqueID::nextID = 0;

UniqueID::UniqueID() {
	id = ++nextID;
}

UniqueID::UniqueID(const UniqueID& orig) {
	id = orig.id;
}

UniqueID& UniqueID::operator=(const UniqueID& orig) {
	id = orig.id;
	return(*this);
}
#endif