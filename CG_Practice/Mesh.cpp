#include "Mesh.h"

void Mesh::open(const char* filename) {
	FILE* fp;
	char header[256] = { 0 };
	double pos[3];
	int index = 0;
	_minPos.Set(100000.0);
	_maxPos.Set(-100000.0);

	fopen_s(&fp, filename, "r");
	while (fscanf(fp, "%s %lf %lf %lf", header, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (header[0] == 'v' && header[1] == NULL) {
			for (int i = 0; i < 3; i++) {
				if (_minPos[i] > pos[i]) _minPos[i] = pos[i];
				if (_maxPos[i] < pos[i]) _maxPos[i] = pos[i];
			}
			_vertices.push_back(new Vertex(index++, Vec3<double>(pos[0], pos[1], pos[2])));
		}
	}
	
	index = 0;
	int indices[3];
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d %d %d", header, &indices[0], &indices[1], &indices[2]) != EOF) {
		if (header[0] == 'f' && header[1] == NULL) {
			auto v0 = _vertices[indices[0] - 1];
			auto v1 = _vertices[indices[1] - 1];
			auto v2 = _vertices[indices[2] - 1];
			_faces.push_back(new Face(index++, v0, v1, v2));
		}
	}		
	fclose(fp);
	moveToCenter(10.0);
	BuildList();
	computeNormal();
	//printf("Vertices num. : %d\n", _vertices.size());
	//printf("Faces num. : %d\n", _faces.size());
}

void Mesh::BuildList() {
	for (auto f : _faces) {
		for (auto fv : f->_vertices) {
			fv->_nbFaces.push_back(f);
		}
	}

	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			int index = nf->getIndex(v);
			v->_nbVertices.push_back(nf->v((index + 1) % 3));
			v->_nbVertices.push_back(nf->v((index + 3 - 1) % 3));
		}
	}
}

void Mesh::computeNormal() {
	for (auto f : _faces) {
		auto v0 = f->v(1)->_pos - f->v(0)->_pos;
		auto v1 = f->v(2)->_pos - f->v(0)->_pos;
		f->_normal = v0.Cross(v1);
		f->_normal.Normalize();
	}

	for (auto v : _vertices) {
		v->_normal.Clear();
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= (double)v->_nbFaces.size();
	}
}

void Mesh::reset() {
	for (auto v : _vertices) {
		v->_pos = v->_pos0;
	}
	moveToCenter(10.0);
	computeNormal();
	printf("reset\n");
}

void Mesh::smoothing(int iter) {
	vector<Vec3<double>> newPos;
	for (int it = 0; it < iter; it++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;
			for (auto nv : v->_nbVertices) {
				pos += nv->_pos;
			}
			pos /= v->_nbVertices.size() + 1;
			newPos.push_back(pos);
		}

		for (int i = 0; i < _vertices.size(); i++) {
			auto v = _vertices[i];
			v->_pos = newPos[i];
		}
	}

	computeNormal();
	printf("smoothing : %d\n", iter);
}

void Mesh::laplacianSmoothing(int iter, double lam) {
	vector<Vec3<double>> newPos;
	for (int it = 0; it < iter; it++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;
			Vec3<double> lapPos;
			lapPos.Clear();
			for (auto nv : v->_nbVertices) {
				lapPos += (nv->_pos - pos);
			}
			lapPos /= v->_nbVertices.size() + 1;
			lapPos *= lam;
			pos += lapPos;

			newPos.push_back(pos);
		}

		for (int i = 0; i < _vertices.size(); i++) {
			auto v = _vertices[i];
			v->_pos = newPos[i];
		}
	}

	computeNormal();
	printf("Laplace smoothing : %d\n", iter);
}

void Mesh::taubinSmoothing(int iter, double lam, double mu) {
	vector<Vec3<double>> newPos;
	for (int it = 0; it < iter; it++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;
			Vec3<double> lapPos;
			Vec3<double> tauPos;
			lapPos.Clear();
			tauPos.Clear();
			for (auto nv : v->_nbVertices) {
				lapPos += (nv->_pos - pos);
			}
			lapPos /= v->_nbVertices.size() + 1;
			lapPos *= lam;
			tauPos = lapPos;
			tauPos *= mu;
			pos += lapPos + tauPos;

			newPos.push_back(pos);
		}

		for (int i = 0; i < _vertices.size(); i++) {
			auto v = _vertices[i];
			v->_pos = newPos[i];
		}
	}

	computeNormal();
	printf("Taubin smoothing : %d\n", iter);
}

void Mesh::cotanSmoothing(int iter) {
	int count = 0;					// 실험 값들을 몇번만 출력할지 정함
	vector<Vec3<double>> newPos;
	for (int it = 0; it < 1; it++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;

			Vec3<double> cotPos;
			cotPos.Clear();

			double Wij = 0;
			double PWij = 0;
			double cotAlpBet = 0;

			Vec3<double> WijP;
			WijP.Clear();

			for (auto nv : v->_nbVertices) {
				//int check = 1;	// 근접 페이스가 두 번만 작동하는지 확인
				for (auto nf : nv->_nbFaces) {		// 인접 버텍스의 인접 페이스들을 돌아봄				
					for (int i = 0; i < 3; i++) {	// 인접 페이스들 중 몇 번째 버텍스가 pi와 같은지 확인
						if (nf->v(i) == v) {		// 만약 인접 버텍스의 인접 페이스 중 기존의 pi와 같은 버텍스를 가진 버텍스가 있다면 연산 시작			
							auto DotV = nf->v(3 - (nf->getIndex(v) + nf->getIndex(nv)));	// pi와 pi의 인접 버텍스를 제외한 다른 버텍스를 DotV에 저장
							Vec3<double> toPiV = pos - DotV->_pos;							// DotV에서 pi로의 벡터 저장
							Vec3<double> toPjV = nv->_pos - DotV->_pos;						// DotV에서 pi의 인접 버텍스로의 벡터 저장
							auto radian = acos(toPiV.Dot(toPjV) / (toPiV.Length() * toPjV.Length()));	// 두 벡터 사이의 라디안 도출
							auto angle = radian / (3.14f / 180);									// 두 벡터 사이의 각도 도출

							cotAlpBet += (1 / tan(radian));										// nv->_nbFaces를 돌며 코탄젠트 값 모두 더함

							// 여러가지 값들을 테스트
							//if (count < 6) {
							//	//printf("카운트 검사기 : %d : ", check++);
							//	//printf("%d %d %d : ", nf->getIndex(v), nf->getIndex(nv), 3 - (nf->getIndex(v) + nf->getIndex(nv)));
							//	//printf("%d %lf, %lf\n", count, radian, angle);
							//	printf("%lf\n", cotAlpBet);
							//	count++;
							//}
						}
					}
				}
				Wij += cotAlpBet / 2;			// Wij누적
				PWij = cotAlpBet / 2;			// pi의 인접 버텍스와 곱해줄 값 누적
				cotPos += (nv->_pos * PWij);	
				cotAlpBet = 0;					// 초기화
				PWij = 0;
			}
			cotPos /= Wij;

			// Wij값 디버깅
			//if(count++ < 6)
			//	printf("Wij = %lf\n", Wij);

			Wij = 0;

			newPos.push_back(cotPos);
		}

		for (int i = 0; i < _vertices.size(); i++) {
			auto v = _vertices[i];
			v->_pos = newPos[i];
		}
	}

	computeNormal();
	printf("Cotangent Smoothing : %d\n", iter);
}

void Mesh::moveToCenter(double scailing) {
	auto crossVec = _maxPos - _minPos;
	double maxLength = fmax(fmax(crossVec.x(), crossVec.y()), crossVec.z());
	auto center = (_minPos + _maxPos) / 2.0;
	Vec3<double> newCenter(0.0, 0.0, 0.0);

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto grad = pos - center;
		grad /= maxLength;
		grad *= scailing;
		pos = newCenter;
		pos += grad;
		v->_pos = pos;
	}
}

void Mesh::drawPoint() {
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPopMatrix();
}

void Mesh::drawWire() {
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glBegin(GL_LINES);
	for (auto f : _faces) {
		for (int j = 0; j < 3; j++) {
			auto v0 = f->v((j + 1) % 3);
			auto v1 = f->v((j + 2) % 3);
			auto n0 = f->v((j + 1) % 3)->_normal;
			auto n1 = f->v((j + 2) % 3)->_normal;

			glNormal3f(n0.x(), n0.y(), n0.z());
			glVertex3f(v0->x(), v0->y(), v0->z());

			glNormal3f(n1.x(), n1.y(), n1.z());
			glVertex3f(v1->x(), v1->y(), v1->z());
		}
	}
	glEnd();

	glPopMatrix();
}

void Mesh::drawSurface(bool smooth) {
	glPushMatrix();
	glEnable(GL_LIGHTING);
	smooth == true ? glShadeModel(GL_SMOOTH) : glShadeModel(GL_FLAT);
	for (auto f : _faces) {
		glBegin(GL_TRIANGLES);
		if (!smooth)
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		for (int j = 0; j < 3; j++) {
			auto v = f->v(j);
			if (smooth)
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			glVertex3f(v->x(), v->y(), v->z());
		}
		glEnd();
	}
	glPopMatrix();
}