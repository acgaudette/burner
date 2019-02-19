#include <math.h>
#include <string.h>
#include "burner.h"
#include "experimental.h"

struct SimpleData {
    Mesh mesh;
    ent obj;
    
    Vertex vertices[4096];
    unsigned int indices[4096];

    size_t vertex_count;
    size_t index_count;
};

typedef SimpleData CustomData;
