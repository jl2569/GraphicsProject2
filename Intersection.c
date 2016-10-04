#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Plymorphism in C

typedef struct {
  int kind; // 0 = cylinder, 1 = sphere, 2 = plane
  double color[3];
  union {
    struct {
      double center[3];
      double radius;
    } cylinder;
    struct {	
      double center[3];
      double radius;
    } sphere;
    struct {
      double center[3];
      double normal[3];
    } plane;
  };
} Object;


static inline double sqr(double v) {
  return v*v;
}


static inline void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}
double plane_intersection(double* Ro, double* Rd,
			     double* C, double* n) {
  double a = (n[0]* Rd[0])+(n[1]* Rd[1])+(n[2]* Rd[2]);

  if(fabs(a) < .0001) {
    return -1;
  }
  
  double b[3];
  
  for (int i=0; i<=2;i++){
	  b[i] = C[i]-Ro[i];
  }

  double d = (b[0]* n[0])+(b[1]* n[1])+(b[2]* n[2]) ;

  double t = d/a;

  if (t < 0) {
    return -1;
  }

  return t;
}

double sphere_intersection(double* Ro, double* Rd,
			     double* C, double r) {
  double a = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
  double b = (2 * (Rd[0] * (Ro[0] - C[0]) + Rd[1] * (Ro[1] - C[1]) + Rd[2] * (Ro[2] - C[2])));
  double c = sqr(Ro[0]- C[0]) + sqr(Ro[1]- C[1]) +sqr(Ro[2]- C[2]) - sqr(r);
  double det = sqr(b) - 4 * a * c;
  if (det < 0) return -1;

  det = sqrt(det);
  
  double t0 = (-b - det) / (2*a);
  if (t0 > 0) return t0;

  double t1 = (-b + det) / (2*a);
  if (t1 > 0) return t1;

  return -1;
}

double cylinder_intersection(double* Ro, double* Rd,
			     double* C, double r) {

  double a = (sqr(Rd[0]) + sqr(Rd[2]));
  double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[2] * Rd[2] - Rd[2] * C[2]));
  double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);

  double det = sqr(b) - 4 * a * c;
  if (det < 0) return -1;

  det = sqrt(det);
  
  double t0 = (-b - det) / (2*a);
  if (t0 > 0) return t0;

  double t1 = (-b + det) / (2*a);
  if (t1 > 0) return t1;

  return -1;
}

int main() {

  Object** objects;
  objects = malloc(sizeof(Object*)*2);
  objects[1] = malloc(sizeof(Object));
  objects[1]->kind = 1;
  objects[1]->sphere.radius = 2;
  objects[1]->sphere.center[0] = 0;
  objects[1]->sphere.center[1] = 2;
  objects[1]->sphere.center[2] = 5;
  objects[0] = malloc(sizeof(Object));
  objects[0]->kind = 0;
  objects[0]->plane.center[0] = 0;
  objects[0]->plane.center[1] = -1;
  objects[0]->plane.center[2] = 0;
  objects[0]->plane.normal[0] = 0;
  objects[0]->plane.normal[1] = 1;
  objects[0]->plane.normal[2] = 0;
  
  
  double cx = 0;
  double cy = 0;
  double h = 0.7;
  double w = 0.7;

  int M = 20;
  int N = 20;

  double pixheight = h / M;
  double pixwidth = w / N;
  for (int y = 0; y < M; y += 1) {
    for (int x = 0; x < N; x += 1) {
      double Ro[3] = {0, 0, 0};
      // Rd = normalize(P - Ro)
      double Rd[3] = {
	cx - (w/2) + pixwidth * (x + 0.5),
	cy - (h/2) + pixheight * (y + 0.5),
	1
      };
      normalize(Rd);

      double best_t = INFINITY;
      for (int i=0; objects[i] != 0; i ++) {
	double t = 0;
	switch(objects[i]->name) {
	case "plane":
	  t = plane_intersection(Ro, Rd,
				    objects[i]->plane.center,
				    objects[i]->plane.normal);
		break;
	case "sphere":
	  t = sphere_intersection(Ro, Rd,
				    objects[i]->sphere.center,
				    objects[i]->sphere.radius);
		break;
	default:
	  // Horrible error
	  exit(1);
	}
	if (t > 0 && t < best_t) best_t = t;
	//printf("%lf\n",best_t);
      }
      if (best_t > 0 && best_t != INFINITY) {
	printf("#");
      } else {
	printf(".");
      }
      
    }
    printf("\n");
  }
  
  return 0;
}