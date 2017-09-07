#include <iostream>
#include <memory>

class BoolHolder
{
public:
	BoolHolder(bool* state) 
		: state_(state) {}
	
	bool getState() const { return *state_; }
	bool& getState() { return *state_; };	// read/write
private:
	bool* state_;

};

bool globalstate = true;

int main(int argc, char** argv)
{
	BoolHolder test(&globalstate);

	if (test.getState()) {
		std::cout << "The truth is out there..." << std::endl;
	}

	return 0;
}