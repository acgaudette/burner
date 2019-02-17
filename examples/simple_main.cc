#include "simple_common.h"
#include <assert.h>

int main(void)
{
    CustomData data = {};

    assert(data.vertex_count == 0);
    assert(data.index_count == 0);

    return Engine_init("examples/simple.cc", (void *)&data);
}
