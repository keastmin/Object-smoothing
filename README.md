# Object-smoothing
라플라시안 스무딩, 타우빈 스무딩, 코탄젠트 스무딩을 OpenGL을 통해 구현하였다.  


**사용 API**  
- OpenGL  
- GLUT  


**시뮬레이션 환경**  
- CPU : intel i7-10 10700K  
- GPU : RTX 3070 BLACK EDITION OC D6 8GB  
- RAM : samsung DDR4-3200(8GB) x 2


**컨트롤**  
- 마우스 좌클릭 : 화면 이동 회전
- 마우스 휠클릭 : 화면 상하좌우 이동  
- 마우스 우클릭 : 화면 줌  
- P, W, S, A : 렌더링 방식  
- F : 스무딩  
- L : 라플라시안 스무딩  
- T : 타우빈 스무딩  
- C : 코탄젠트 스무딩
- R : 리셋

**결과**  
- 라플라시안 스무딩  
![Image Alt Text](https://github.com/keastmin/Object-smoothing/blob/main/image/lapls.PNG)  
가중치를 0.6으로 주었을 때 모습  
라플라시안 스무딩은 pi의 인접 버텍스만의 연산이 아닌 pi의 인접 버텍스들로 부터 pi로 향하는 벡터들을 모두 lapPos라는 Vec3<double>형의 변수에 더해주고 그 후 pi와 그 인접 버텍스들의 개수만큼 나누어 주었다. 그 후 가중치를 곱하고 마지막으로 기존 버텍스에서 새로운 버텍스 지점까지 향하는 벡터를 pos라는 pi의 포지션값에 대입해주었다.

- 타우빈 스무딩  
![Image Alt Text](https://github.com/keastmin/Object-smoothing/blob/main/image/taus.PNG)  
람다 값은 0.6, 뮤 값은 -0.5로 주었을 때 모습  
타우빈 스무딩은 기본적으로 라플라시안 스무딩과 유사하나 뮤라는 가중치 값만큼 확장된 버텍스들을 기존 라플라시안 스무딩의 수축된 버텍스들과 더해주어 비교적 덜 수축되게 만든 스무딩이다. 이는 라플라시안 스무딩을 구현할 때의 코드에 tauPos라는 Vec3<double>형 변수를 만들어 pi의 인접 버텍스들이 pi로 향하는 벡터들을 모두 더한 lapPos를 tauPos에 그대로 대입하여 lapPos에는 람다값, tauPos에는 뮤값을 곱하여 그 둘을 더한 값을 기존의 pos에 더해주는 방식으로 구현하였다. 이를 통해 수축과 확장이 같이 작용하여 상대적으로 뾰족한 부분이 유지되는 모습을 확인할 수 있다.    

- 코탄젠트 스무딩  
![Image Alt Text](https://github.com/keastmin/Object-smoothing/blob/main/image/cotas.PNG)  
코탄젠트 스무딩을 하기 위해선 가장 먼저 해야할 일은 pi와 pi의 인접 버텍스(이후 nv)가 동시에 존재하고 있는 페이스 두 개를 먼저 찾는 것이라고 생각했다. 그렇기에 nv의 인접 페이스(이후 nf)를 for문을 돌려가며 그 nf의 버텍스 들을 하나씩 검사하여 pi와 일치하는 버텍스가 있다면 pi와 nv가 동시에 존재하는 페이스로 인식하고 그 페이스에 대하여 pi와 nv를 제외한 나머지 엣지를 찾은 뒤 그 나머지 엣지에서 pi와 nv로 향하는 벡터를 각각 Vec3<double> toPiV와 Vec3<double> toPjV에 담아두었다. 그리고 그 두 개의 벡터로 acos(toPiV.Dot(toPjV) / (toPiV.Length() * toPjV.Length()))로 라디안을 구하고 radian / (3.14f / 180)로 각도를 구했다. 이렇게 pi와 nv가 마주보고 있는 엣지에 대한 각도를 찾을 수 있었고 총 두개의 인접 페이스가 찾아질테니 알파와 베타가 모두 double 자료형의 cotAlpBet이라는 변수에 코탄젠트 연산이 되어 저장된다. 그 후 라플라시안 스무딩의 수식이 변형된 코탄젠트 스무딩 수식에 대입을 하였다.  
그러나 이런 과정이 구현에 실패 요인이 되었는데 그 이유는 math 라이브러리의 tan(탄젠트)함수에 라디안 값이 아닌 각도를 계산해서 매개변수로 주었기 때문에다. 후에 그 부분을 수정하여 구현하였다.  
