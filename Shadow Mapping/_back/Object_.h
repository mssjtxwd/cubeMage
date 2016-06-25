#include "tga.h"
using namespace std;
struct Vertex
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct VT
{
	GLfloat  u;
	GLfloat  v;
};
struct VN
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct Face
{
	int a;
	int b;
	int c;
};
struct FT
{
	int a;
	int b;
	int c;
};
struct FN
{
	int a;
	int b;
	int c;
};
struct material{
	string name;
	GLfloat Ka[4];
	GLfloat Kd[4];
	GLfloat Ks[4];
	GLfloat Ns;
	string texture;
};
struct S{
	int num_f;
	string usema;
	vector<Face> face;
	vector<FT> ft;
	vector<FN> fn;
};
class Object
{
public:
	void draw(GLfloat x,GLfloat y,GLfloat z,GLfloat s);
	int readfile(string str);
	int readmaterial();
private:
	string materialset;
	int num_v;
	int num_vn;
	int num_vt;
	int num_s;
	int num_ma;
	vector<Vertex> vertex;
	vector<VN> vn;
	vector<VT> vt;
	vector<S> sset;

	map<string,material> masMap;
};
int Object::readmaterial()
{
	string url="./"+materialset;
	ifstream ofile(url.c_str());
	string line;
	struct material currentma;
	int flag=0;
	while(getline(ofile,line)){
		istringstream input(line);
		string s;
		input>>s;
		if(strcmp(s.c_str(),"newmtl")==0){
			if(flag!=0){
				masMap[currentma.name]=currentma;
			}
			flag=1;
			memset(&currentma, 0, sizeof(currentma));
			input>>currentma.name;
		}
		if(strcmp(s.c_str(),"Ka")==0){
			input>>currentma.Ka[0]>>currentma.Ka[1]>>currentma.Ka[2];
			currentma.Ka[3]=1;
		}
		if(strcmp(s.c_str(),"Kd")==0){
			input>>currentma.Kd[0]>>currentma.Kd[1]>>currentma.Kd[2];
			currentma.Kd[3]=1;
		}
		if(strcmp(s.c_str(),"Ks")==0){
			input>>currentma.Ks[0]>>currentma.Ks[1]>>currentma.Ks[2];
			currentma.Ks[3]=1;
		}
		if(strcmp(s.c_str(),"Ns")==0){
			input>>currentma.Ns;
		}
		if(strcmp(s.c_str(),"map_Kd")==0){
			input>>currentma.texture;
		}
	}
	masMap[currentma.name]=currentma;
	return 0;
}
int Object::readfile(string str)
{
	int flag=0;
	S currents;
	string currentma="";
	num_v=0;
	num_vn=0;
	num_vt=0;
	num_s=0;
	ifstream ofile(str.c_str());
	string line;
	//getline(ofile,line);
	while(getline(ofile,line)){
		if(line!="")
		{
			if(line[0]=='m'){
				istringstream input(line);
				string s;
				input>>s>>materialset;
			}
			if(line[0]=='v'){
				if(line[1]=='n'){
					istringstream input(line);
					string s;
					VN currentvn;
					input>>s>>currentvn.x>>currentvn.y>>currentvn.z;
					vn.push_back(currentvn);
					num_vn++;
				}
				else if(line[1]=='t'){
					istringstream input(line);
					string s;
					VT currentvt;
					input>>s>>currentvt.u>>currentvt.v;
					vt.push_back(currentvt);
					num_vt++;
				}
				else{
					istringstream input(line);
					string s;
					Vertex currentv;
					input>>s>>currentv.x>>currentv.y>>currentv.z;
					vertex.push_back(currentv);
					num_v++;
				}
			}
			if(line[0]=='s'){
				if(flag!=0){
					currents.usema=currentma;
					sset.push_back(currents);
					num_s++;
				}
				flag=1;
				currents.face.clear();
				currents.ft.clear();
				currents.fn.clear();
				currents.num_f=0;
			}
			if(line[0]=='u'){
				istringstream input(line);
				string s;
				input>>s>>currentma;
			}
			if(line[0]=='f'){
				istringstream input(line);
				string s;
				Face currentF;
				FT currentFT;
				FN currentFN;
				input>>s;
				int temp;
				for(int i=0;i<3;i++){
					int k;
					input>>s;
					temp=0;
					for(k=0;s[k]!='/';k++){
						temp=temp*10+s[k]-48;
					}
					if(i==0)
						currentF.a=temp;
					else if(i==1)
						currentF.b=temp;
					else if(i==2)
						currentF.c=temp;
					temp=0;
					for(k=k+1;s[k]!='/';k++){
						temp=temp*10+s[k]-48;
					}
					if(i==0)
						currentFT.a=temp;
					else if(i==1)
						currentFT.b=temp;
					else if(i==2)
						currentFT.c=temp;
					temp=0;
					int length=s.length();
					for(k=k+1;k<s.length();k++){
						temp=temp*10+s[k]-48;
					}
					if(i==0)
						currentFN.a=temp;
					else if(i==1)
						currentFN.b=temp;
					else if(i==2)
						currentFN.c=temp;
				}
				currents.face.push_back(currentF);
				currents.ft.push_back(currentFT);
				currents.fn.push_back(currentFN);
				currents.num_f++;
			}
		}
	}
	currents.usema=currentma;
	sset.push_back(currents);
	num_s++;
	return 0;
}
void Object::draw(GLfloat x,GLfloat y,GLfloat z,GLfloat s)
{
	glPushMatrix();
		glScalef(s,s,s);
		glTranslatef(x/s,y/s,z/s);
			for(int j=0;j<num_s;j++)
			{
				material setmaterial= masMap[sset[j].usema];
				glMaterialfv(GL_FRONT, GL_AMBIENT, (float*)setmaterial.Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, (float*)setmaterial.Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)setmaterial.Ks);
				glMaterialf(GL_FRONT, GL_SHININESS, setmaterial.Ns);
				string maurl=setmaterial.texture;
				TextureImage *texture = new TextureImage();
				LoadTGA(texture,maurl.c_str());
				glBindTexture(GL_TEXTURE_2D,texture->texID);
				for(int i=0;i<sset[j].num_f;i++)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glBegin(GL_TRIANGLES);
						glTexCoord2f(vt[sset[j].ft[i].a-1].u,vt[sset[j].ft[i].a-1].v);
						glNormal3f(vn[sset[j].fn[i].a-1].x*s,vn[sset[j].fn[i].a-1].y*s,vn[sset[j].fn[i].a-1].z*s);
						glVertex3f(vertex[sset[j].face[i].a-1].x,vertex[sset[j].face[i].a-1].y,vertex[sset[j].face[i].a-1].z);
						glTexCoord2f(vt[sset[j].ft[i].b-1].u,vt[sset[j].ft[i].b-1].v);
						glNormal3f(vn[sset[j].fn[i].b-1].x*s,vn[sset[j].fn[i].b-1].y*s,vn[sset[j].fn[i].b-1].z*s);
						glVertex3f(vertex[sset[j].face[i].b-1].x,vertex[sset[j].face[i].b-1].y,vertex[sset[j].face[i].b-1].z);
						glTexCoord2f(vt[sset[j].ft[i].c-1].u,vt[sset[j].ft[i].c-1].v);
						glNormal3f(vn[sset[j].fn[i].c-1].x*s,vn[sset[j].fn[i].c-1].y*s,vn[sset[j].fn[i].c-1].z*s);
						glVertex3f(vertex[sset[j].face[i].c-1].x,vertex[sset[j].face[i].c-1].y,vertex[sset[j].face[i].c-1].z);
					glEnd();
				}
			}
	glPopMatrix();
}
