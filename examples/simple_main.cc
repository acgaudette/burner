#include <math.h>
#include "burner.h"

int main(void)
{
    struct MyData {
        Mesh mesh;
        ent obj;
    } data;

    return Engine_init((void *)&data);
}
