#include<bits/stdc++.h>
#include<GL/gl.h>
#include <GL/glut.h>
#include<windows.h>

using namespace std;

struct Point {
  float x, y;
  Point() {}
  Point(float f, float s) {
    x = f;
    y = s;
  }
};

bool LLFlag = true;
bool Running = false;
float x, y; // points
int timex = 200, disp_id = 0;

Point pivot;
stack<Point> hull3;
vector< stack<Point> > HULLS;
vector<Point> points2[8], hull2[8];
vector<Point> points, points3, hull, redo;

int random(int mn, int mx) {
  static bool first = true;
  if (first) {
    srand( time(NULL) );
    first = false;
  }
  return mn + rand() % (( mx + 1 ) - mn);
}

void wait(int seconds) {
  int endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

void mouse(int button, int state, int mx, int my) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    int wH = glutGet(GLUT_WINDOW_HEIGHT);
    x = mx;
    y = wH - my;
    if (disp_id == 1) points.push_back( Point(x, y) );
    if (disp_id == 3) points3.push_back( Point(x, y) );
    //cout << x << " " << y << endl;
    if (disp_id == 1 || disp_id == 3) {
      PlaySound("point.wav", NULL, SND_ASYNC|SND_FILENAME);
    }
    redo.clear();
  }
  glutPostRedisplay();
}

int orientation(Point p, Point q, Point r) {
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0.0) return 0; // colinear
	return (val > 0.0) ? 1 : 2; // clock or counterclock wise
}

void convex_hull() { /// Jarvis's Algorithm for Simulation 1
	if (points.size() < 3) return;
	hull.clear();
	int L = 0;
	for (int i = 1; i < points.size(); i++) {
		if (points[i].x < points[L].x) L = i;
	}
	int p = L, q;
	do {
    hull.push_back(points[p]);
		q = (p + 1) % points.size();
		for (int i = 0; i < points.size(); i++) {
      if (orientation(points[p], points[i], points[q]) == 2) {
        q = i;
      }
		}
		p = q;
	} while (p != L);
	// Draw Result
  glBegin(GL_LINE_LOOP);
	for (int i = 0; i < hull.size() - 1; i++) {
    glColor3ub(70, 120, 175);
    glVertex2i(hull[i].x, hull[i].y);
    glVertex2i(hull[i + 1].x, hull[i + 1].y);
	}
	glEnd();
}

void convex_hull2(int id) { /// Jarvis's Algorithm for Simulation 2
	if (points2[id].size() < 3) return;
	hull2[id].clear();
	int L = 0;
	for (int i = 1; i < points2[id].size(); i++) {
		if (points2[id][i].x < points2[id][L].x) L = i;
	}
	int p = L, q;
	do {
    hull2[id].push_back(points2[id][p]);
		q = (p + 1) % points2[id].size();
		for (int i = 0; i < points2[id].size(); i++) {
      if (orientation(points2[id][p], points2[id][i], points2[id][q]) == 2) {
        q = i;
      }
		}
		p = q;
	} while (p != L);
	// Draw Result
  glBegin(GL_LINE_LOOP);
	for (int i = 0; i < hull2[id].size() - 1; i++) {
    if (id % 2 == 0) glColor3ub(70, 120, 175);
    if (id % 2) glColor3ub(190, 5, 55);
    glVertex2i(hull2[id][i].x, hull2[id][i].y);
    glVertex2i(hull2[id][i + 1].x, hull2[id][i + 1].y);
	}
	glEnd();
}

void convex_hull_display3();

void print() {
  LLFlag = false;
  for (auto i : HULLS) {
    hull3 = i;
    convex_hull_display3();
    wait(1);
  }
  LLFlag = true;
  Running = false;
}

// returns -1 if a -> b -> c forms a counter-clockwise turn,
// +1 for a clockwise turn, 0 if they are collinear
int ccw(Point a, Point b, Point c) {
  int area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
  if (area > 0) return -1;
  else if (area < 0) return 1;
  return 0;
}

// returns square of Euclidean distance between two points
int sqrDist(Point a, Point b)  {
  int dx = a.x - b.x, dy = a.y - b.y;
  return dx * dx + dy * dy;
}

// used for sorting points according to polar order w.r.t the pivot
bool POLAR_ORDER(Point a, Point b)  {
  int order = ccw(pivot, a, b);
  if (order == 0) return sqrDist(pivot, a) < sqrDist(pivot, b);
  return (order == -1);
}

void convex_hull3() { /// Graham's Scan Algorithm for Simulation 3
  if (points3.size() < 3) return;
  /// find the point having the least y coordinate (pivot),
  /// ties are broken in favour of lower x coordinate
  int leastY = 0;
  for (int i = 1; i < points3.size(); i++) {
    if (points3[i].y < points3[leastY].y) leastY = i;
  }
  /// swap the pivot with the first point
  swap(points3[0], points3[leastY]);
  /// sort the remaining point according to polar order about the pivot
  pivot = points3[0];
  sort(points3.begin() + 1, points3.end(), POLAR_ORDER);
  hull3.push(points3[0]);
  hull3.push(points3[1]);
  hull3.push(points3[2]);
  HULLS.push_back( hull3 );
  for (int i = 3; i < points3.size(); i++) {
    stack<Point> TEMP = hull3;
    cout << "TEMP STACK" << endl;
    while (!TEMP.empty()) {
      cout << TEMP.top().x << " " << TEMP.top().y << '\n';
      TEMP.pop();
    }
    cout << endl;
    Point top = hull3.top();
    hull3.pop();
    while (ccw(hull3.top(), top, points3[i]) != -1) {
      top = hull3.top();
      hull3.pop();
    }
    hull3.push(top);
    hull3.push(points3[i]);
    HULLS.push_back( hull3 );
  }
  puts("DONE");
  print();
}

void draw_grid() {
  glColor3ub(35, 35, 35);
  for (int x = 0; x < 800; x += 10) {
    for (int y = 0; y < 600; y += 10) {
      glBegin(GL_LINE_LOOP);
      glVertex2i(x, y);
      glVertex2i(x + 10, y);
      glVertex2i(x + 10, y + 10);
      glVertex2i(x, y + 10);
      glEnd();
    }
  }
  // axis lines
  glColor3ub(70, 70, 70);
  glBegin(GL_LINES);
  glVertex2i(0.0, 300.0);
  glVertex2i(800.0, 300.0);
  glVertex2i(400.0, 0);
  glVertex2i(400.0, 600.0);
  glEnd();
}

void print_xy(float x, float y) {
  if (x == 0.0 && y == 0.0) return;
  string s = "(" + to_string((int)x) + ", " + to_string((int)y) + ")";
  //cout << s << endl;
  glColor3ub(255, 255, 255);
  glRasterPos2f(x + 7, y);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
  }
}

void draw_point(float x, float y) {
  glColor3ub(190, 190, 190);
  glPointSize(5.0);
  glEnable(GL_POINT_SMOOTH);
  print_xy(x, y); // printing co-ordinate beside the point
  glBegin(GL_POINTS);
  glVertex2i(x, y);
  glEnd();
  glDisable(GL_POINT_SMOOTH);
}

void draw_point2(float x, float y, int id) {
  if (id % 2 == 0) glColor3ub(70, 120, 175);
  if (id % 2) glColor3ub(190, 5, 55);
  glPointSize(5.0);
  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  glVertex2i(x, y);
  glEnd();
  glDisable(GL_POINT_SMOOTH);
}

void draw_title() {
  string s = "CONVEX HULL";
  glColor3ub(255, 255, 255);
  glRasterPos2f(350, 550);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
  if (disp_id == 1) {
    s = "( jarvis' algoritm )";
    glRasterPos2f(370, 535);
  } else if (disp_id == 2) {
    s = "( shuffle animation )";
    glRasterPos2f(360, 535);
  } else if (disp_id == 3) {
    s = "( graham scan )";
    glRasterPos2f(370, 535);
  }
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
  }
}

void write_rules() {
  string s = "~ click on the grid to provide points ~";
  glColor3ub(255, 255, 255);
  if (disp_id != 2) { 
    glRasterPos2f(325, 510);
    for (char c : s) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
  }
  s = "Instructions:";
  glRasterPos2f(40, 100);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
  }
  if (disp_id == 2) {
    s = "- Press Q to + speed.";
    glRasterPos2d(45, 85);
    for (char c : s) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    s = "- Press W to - speed.";
    glRasterPos2d(45, 70);
    for (char c : s) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    s = "- Press ESC to exit.";
    glRasterPos2d(45, 55);
    for (char c : s) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    return;
  }
  s = "- Press Z for undo.";
  glRasterPos2f(45, 85);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
  }
  s = "- Press X for redo.";
  glRasterPos2f(45, 70);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
  }
  if (disp_id == 3) {
    s = "- Press S for start.";
    glRasterPos2f(45, 55);
    for (char c : s) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
  }
  s = "- Press ESC to exit.";
  if (disp_id == 1) glRasterPos2f(45, 55);
  else glRasterPos2d(45, 42);
  for (char c : s) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
  }
}

void write_in_def() {
  string s = "2D CONVEX  HULL  SIMULATION";
  glColor3ub(255, 255, 255);
  glRasterPos2f(270, 400);
  for (int i = 0; i < s.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
  }
  glPointSize(3);
  glBegin(GL_LINE_LOOP);
  glVertex2f(250, 370);
  glVertex2f(580, 370);
  glVertex2f(580, 440);
  glVertex2f(250, 440);
  glEnd();
  s = "Press 1 for Simulation A";
  glRasterPos2f(340, 300);
  for (int i = 0; i < s.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, s[i]);
  }
  s = "Press 2 for Simulation B";
  glRasterPos2f(340, 250);
  for (int i = 0; i < s.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, s[i]);
  }
  s = "Press 3 for Simulation C";
  glRasterPos2f(340, 200);
  for (int i = 0; i < s.size(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, s[i]);
  }
}

void def_display() {
  glClear(GL_COLOR_BUFFER_BIT);
  write_in_def();
  glFlush();
}

void convex_hull_display() {
  glClear(GL_COLOR_BUFFER_BIT);
  draw_grid();
  draw_title();
  write_rules();
  for (int i = 0; i < points.size(); i++) {
    draw_point(points[i].x, points[i].y);
  }
  convex_hull();
  glFlush();
}

void convex_hull_display2() {
  glClear(GL_COLOR_BUFFER_BIT);
  draw_grid();
  draw_title();
  write_rules();
  for (int c = 0; c < 8; c++) {
    for (int i = 0; i < points2[c].size(); i++) {
      draw_point2(points2[c][i].x, points2[c][i].y, c);
    }
    convex_hull2(c);
  }
  glFlush();
}

void convex_hull_display3() {
  glClear(GL_COLOR_BUFFER_BIT);
  draw_grid();
  draw_title();
  write_rules();
  for (int i = 0; i < points3.size(); i++) {
    draw_point(points3[i].x, points3[i].y);
  }
  if (!hull3.empty()) {
    stack<Point> temp = hull3;
    Point last = temp.top();
    temp.pop();
    if (LLFlag) glBegin(GL_LINE_LOOP);
    else glBegin(GL_LINES);
    bool lolF = true;
    while (!temp.empty()) {
      Point p = temp.top();
      temp.pop();
      if (lolF && !LLFlag) glColor3ub(0, 0, 255), lolF = false;
      else glColor3ub(255, 0, 0);
      glVertex2i(last.x, last.y);
      glVertex2i(p.x, p.y);
      last = p;
    }
    glEnd();
  }
  glFlush();
}

void clear_() {
  redo.clear();
  points.clear();
  points3.clear();
  HULLS.clear();
  while (!hull3.empty()) hull3.pop();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case '1':
      if (disp_id == 0) {
        disp_id = 1;
        clear_();
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        glutDisplayFunc(convex_hull_display);
      }
      break;
    case '2':
      if (disp_id == 0) {
        disp_id = 2;
        timex = 200;
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        glutDisplayFunc(convex_hull_display2);
      }
      break;
    case '3':
      if (disp_id == 0) {
        disp_id = 3;
        clear_();
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        glutDisplayFunc(convex_hull_display3);
      }
      break;
    case 'r':
      if (disp_id == 3) {
        clear_();
        glutDisplayFunc(convex_hull_display3);
      }
      break;
    case 's':
      if (disp_id == 3) {
        Running = true;
        HULLS.clear();
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        while (!hull3.empty()) hull3.pop();
        convex_hull3();
      }
      break;
    case 'z':
      if (disp_id == 1) {
        if (!points.empty()) {
          //PlaySound("point.wav", NULL, SND_ASYNC|SND_FILENAME);
          redo.push_back(points[points.size() - 1]);
          points.pop_back();
        }
      } else if (disp_id == 3) {
        if (!points3.empty()) {
          //PlaySound("point.wav", NULL, SND_ASYNC|SND_FILENAME);
          redo.push_back(points3[points3.size() - 1]);
          points3.pop_back();
          HULLS.clear();
          while (!hull3.empty()) hull3.pop();
        }
        glutPostRedisplay();
      }
      break;
    case 'x':
      if (disp_id == 1) {
        if (!redo.empty()) {
          PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\point.wav"), NULL, SND_ASYNC|SND_FILENAME);
          points.push_back(redo[redo.size() - 1]);
          redo.pop_back();
        }
      } else if (disp_id == 3) {
        if (!redo.empty()) {
          PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\point.wav"), NULL, SND_ASYNC|SND_FILENAME);
          points3.push_back(redo[redo.size() - 1]);
          redo.pop_back();
        }
        glutPostRedisplay();
      }
      break;
    case 'q':
      if (disp_id == 2 && timex > 50) timex -= 10;
      break;
    case 'w':
      if (disp_id == 2) timex += 10;
      break;
    case 27: // ESC
      if (disp_id == 0) {
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        exit(0);
      } else if (disp_id > 0) {
        disp_id = 0;
        PlaySound(TEXT("C:\\Users\\humae\\Desktop\\Convex Hull Simulation\\bin\\click.wav"), NULL, SND_ASYNC|SND_FILENAME);
        glutDisplayFunc(def_display);
      }
      break;
   }
}

void update(int value) {
  if (disp_id == 2) {
    int rangeX1 = 80, rangeX2 = 280;
    int rangeY1 = 300, rangeY2 = 500;
    for (int c = 0; c < 8; c++) {
      points2[c].clear();
      if (c == 4) {
        rangeX1 = 80, rangeX2 = 280;
        rangeY1 = 120, rangeY2 = 320;
      }
      for (int i = 0; i < 120; i++) {
        points2[c].push_back( Point(random(rangeX1, rangeX2), random(rangeY1, rangeY2)) );
      }
      rangeX1 += 150;
      rangeX2 += 150;
    }
  }
  glutPostRedisplay();
  glutTimerFunc(timex, update, 0);
}

void initialize() {
  glClearColor(0.0039, 0.06, 0.13, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluOrtho2D(0.0, 800.0, 0.0, 600.0);
  glClear(GL_COLOR_BUFFER_BIT);
  //draw_grid();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(300, 100);
  glutCreateWindow("Convex Hull Simulation");
  glutDisplayFunc(def_display);
	glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutTimerFunc(timex, update, 0);
  initialize();
  glutMainLoop();
  return 0;
}

