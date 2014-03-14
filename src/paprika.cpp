#include <paprikaapi.hpp>

int main(int argc, char *argv[])
{
    paprika::PaprikaAPI p;
    p.input(argv[1]);
    return 0;
}
