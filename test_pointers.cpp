#include <iostream>
using namespace std;

class bibliografia
{
public:
	bibliografia() {}
	virtual void probolh()
	{
		cout << "from biliografia..." << endl;
	}
};

class sinedria : public bibliografia
{
public:
	sinedria(){size = 0;}
	virtual void probolh()
	{
		cout << "from sinedria..." << endl;
	}
private:
	int size;
};

int main()
{
	bibliografia **test;
	test = new bibliografia*[2];
	test[0] = new bibliografia();
	test[1] = new sinedria();
	test[0]->probolh();
	test[1]->probolh();

    delete test[0];
    delete test[1];
    delete[] test;
	return 0;

}