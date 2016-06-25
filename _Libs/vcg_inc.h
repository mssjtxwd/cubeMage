/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#ifndef _VCG_INC_
#define _VCG_INC_

#include <iostream>
#include <cassert>
#include "gl_inc.h"

#include "wrap/io_trimesh/import.h"
#include "wrap/io_trimesh/export.h"
#include "vcg/complex/complex.h"


class MyVertex;
class MyEdge;
class MyFace;

typedef vcg::UsedTypes<
	vcg::Use<MyVertex>::AsVertexType,
	vcg::Use<MyEdge>  ::AsEdgeType,
	vcg::Use<MyFace>  ::AsFaceType >
MyUsedTypes;

class MyVertex : public vcg::Vertex<MyUsedTypes,
	vcg::vertex::Coord3f,
	vcg::vertex::Normal3f,
	vcg::vertex::BitFlags > { };

class MyEdge : public vcg::Edge<MyUsedTypes,
	vcg::edge::VertexRef,
	vcg::edge::EFAdj,
	vcg::edge::BitFlags > { };

class MyFace : public vcg::Face<MyUsedTypes,
	vcg::face::VertexRef,
	vcg::face::Normal3f,
	vcg::face::FFAdj,
	vcg::face::BitFlags > { };

typedef	vcg::tri::TriMesh<
	std::vector<MyVertex>,
	std::vector<MyEdge>,
	std::vector<MyFace> >
GLMesh;


inline const glm::vec3& vcg_to_glm(const GLMesh::CoordType& v3)
{
	return *((glm::vec3*)(&v3[0]));
}

// using vcg::tri::io::Importer<> may take too long time to link ...
inline void vcg_loadFromPLY(GLMesh& mesh, const char* file)
{
	double tt = omp_get_wtime();
	int mask;
	int oerr = vcg::tri::io::ImporterPLY<GLMesh>::Open(mesh, file, mask);
	if( oerr != 0 ) {
		std::cout << "Load PLY file ERROR: "
			<< vcg::tri::io::ImporterPLY<GLMesh>::ErrorMsg(oerr) << '\n';
		std::cin.get(); // hold the screem
	}
	//vcg::tri::Clean<GLMesh>::RemoveDuplicateVertex(mesh);
	vcg::tri::UpdateNormal<GLMesh>::PerFaceNormalized(mesh);
	vcg::tri::UpdateNormal<GLMesh>::PerVertexNormalized(mesh);
	vcg::tri::UpdateTopology<GLMesh>::FaceFace(mesh);
	vcg::tri::UpdateTopology<GLMesh>::AllocateEdge(mesh);

	vcg::tri::Allocator<GLMesh>::CompactVertexVector(mesh);
	vcg::tri::Allocator<GLMesh>::CompactEdgeVector(mesh);
	vcg::tri::Allocator<GLMesh>::CompactFaceVector(mesh);

	//vcg::tri::io::ExporterOBJ<GLMesh>::Save(mesh, "out.obj", loadmask);
	std::cout << "PLY file read time (s): " << omp_get_wtime()-tt << '\n';
}

inline void vcg_drawMesh(GLMesh& mesh)
{
	assert(mesh.FN()==mesh.face.size());

	glBegin(GL_TRIANGLES);
	for(int i=0; i<mesh.FN(); ++i){
		GLMesh::FaceType& f = mesh.face[i];
		glNormal3fv(&f.N()[0]);
		glVertex3fv(&f.V(0)->P()[0]);
		glVertex3fv(&f.V(1)->P()[0]);
		glVertex3fv(&f.V(2)->P()[0]);
	}
	glEnd();
}

inline void vcg_drawEdges(GLMesh& mesh)
{
	assert(mesh.EN()==mesh.edge.size());

	glBegin(GL_LINES);
	for(int i=0; i<mesh.EN(); ++i){
		GLMesh::EdgeType& e = mesh.edge[i];
		glVertex3fv(&(e.V(0)->P()[0]));
		glVertex3fv(&(e.V(1)->P()[0]));
	}
	glEnd();
}

inline void vcg_drawOneEdge(GLMesh& mesh, int index)
{
	assert(mesh.EN()==mesh.edge.size());

	GLMesh::EdgeType& e = mesh.edge[index%mesh.EN()];
	GLMesh::FaceType* fa = e.EFp();
	GLMesh::FaceType* fb = fa->FFp(e.EFi());

	glBegin(GL_TRIANGLES);
	
		glNormal3fv(&fa->N()[0]);
		glVertex3fv(&fa->V(0)->P()[0]);
		glVertex3fv(&fa->V(1)->P()[0]);
		glVertex3fv(&fa->V(2)->P()[0]);

		glNormal3fv(&fb->N()[0]);
		glVertex3fv(&fb->V(0)->P()[0]);
		glVertex3fv(&fb->V(1)->P()[0]);
		glVertex3fv(&fb->V(2)->P()[0]);
	
	glEnd();

}

inline void vcg_drawSilhEdges(GLMesh& mesh, glm::vec4& lpos)
{
	assert(mesh.EN()==mesh.edge.size());

	glBegin(GL_LINES);
	for(int i=0; i<mesh.EN(); ++i){
		GLMesh::EdgeType& e = mesh.edge[i];
		GLMesh::FaceType* fa = e.EFp();
		GLMesh::FaceType* fb = fa->FFp(e.EFi());
		glm::vec3 l0 = lpos.w==0 ? glm::vec3(lpos) :
			glm::vec3(lpos)/lpos.w-vcg_to_glm(e.V(0)->P());
		int sa = glm::dot(l0, vcg_to_glm(fa->N()))>=0 ? 1 : -1;
		int sb = glm::dot(l0, vcg_to_glm(fb->N()))>=0 ? 1 : -1;
		if( sa*sb < 0 ){
			glVertex3fv(&e.V(0)->P()[0]);
			glVertex3fv(&e.V(1)->P()[0]);
		}
	}
	glEnd();
}


#endif // #ifndef _VCG_INC_

