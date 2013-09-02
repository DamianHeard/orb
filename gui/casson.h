#ifndef CASSON_H
#define CASSON_H

#include <qstring.h>

#define LN(ch)   (ch=='u') ? 0 : ((ch=='v') ? 1 : ((ch=='w') ? 2 : 3))

const int vertex_at_faces[4][4] =
        {{9,2,3,1},
         {3,9,0,2},
         {1,3,9,0},
         {2,0,1,9}};

typedef struct CassonFormat CassonFormat;
typedef struct EdgeInfo EdgeInfo;
typedef struct TetEdgeInfo TetEdgeInfo;

struct CassonFormat
{
	SolutionType	type;
	bool		vertices_known;
	int		num_tet;
	EdgeInfo	*head;
};

struct EdgeInfo
{
	int		index,
			one_vertex,
			other_vertex,
			singular_index;
	double 		singular_order,
			e_inner_product,
			v_inner_product1,
			v_inner_product2;

	TetEdgeInfo	*head;
	EdgeInfo	*prev,
			*next;
};

struct TetEdgeInfo
{
	int		tet_index,f1,f2, curves[8];
	double		dihedral_angle;
	TetEdgeInfo	*prev,
			*next;
};


#endif
