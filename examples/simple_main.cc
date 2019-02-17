#include "simple_common.h"

int main(void)
{
    CustomData data = {};
    memset(&data, 0, sizeof(data));

    Engine catalyst;
    return catalyst.run("examples/simple.cc", (void *)&data, sizeof(data));
}
