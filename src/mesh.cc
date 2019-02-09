#include "mesh.h"

Mesh::Mesh():
	vertices(nullptr),
	vertex_count(0),
	indices(nullptr),
	index_count(0)
{}

Mesh::Mesh(
	Vertex *vertices,
	size_t vertex_count,
	float *positions,
	unsigned int *indices,
	size_t index_count
):
vertices(vertices),
vertex_count(vertex_count),
indices(indices),
index_count(index_count)
{
	for (size_t i = 0; i < vertex_count; ++i) {
		vertices[i] = Vertex {
			{
				positions[3 * i    ],
				positions[3 * i + 1],
				positions[3 * i + 2],
			},
			{
				0, 0, 0
			}
		};
	}
}

Mesh::Mesh(
	Vertex *vertices,
	size_t vertex_count,
	float *positions,
	float *normals,
	unsigned int *indices,
	size_t index_count
):
vertices(vertices),
vertex_count(vertex_count),
indices(indices),
index_count(index_count)
{
	for (size_t i = 0; i < vertex_count; ++i) {
		vertices[i] = Vertex {
			{
				positions[3 * i    ],
				positions[3 * i + 1],
				positions[3 * i + 2],
			},
			{
				normals[3 * i    ],
				normals[3 * i + 1],
				normals[3 * i + 2],
			}
		};
	}
}
