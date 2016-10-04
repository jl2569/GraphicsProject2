#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  double d = (n[0]* C[0])+(n[1]* C[1])+(n[2]* C[2]);
  double a = (n[0]* Rd[0]) +(n[1]* Rd[1]) +(n[2]* Rd[2]);
  double b = (n[0]* Ro[0]) +(n[1]* Ro[1]) +(n[2]* Ro[2]) + d;
  double c  = -b/a;
  double e  = (Ro[0] + Rd[0] *(c * Ro[1]) + Rd[1] *(c * Ro[2]) + Rd[2]);
  
  return e;
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

int line = 1;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);    
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }  
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);      
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);      
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%f", &value);
  // Error check this..
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}

void valuesetter(int type,char* key ,double value, Object** objects ){
	if (type == 0){
	}else {
		if ((strcmp(key, "radius") == 0)){
			objects[1]->sphere.radius = value;
		}
	}
}
void vectorsetter(int type,char* key ,double* value, Object** objects ){
	if (type == 1){
		if ((strcmp(key, "color") == 0)){
			for (int i=0;i<=2;i++){
			   // objects[1]->sphere.color[i] = value[i];
			}
		}else if ((strcmp(key, "position") == 0)){
			for (int i=0;i<=2;i++){
			    objects[1]->sphere.center[i] = value[i];
			}
		} 
	}else {
		if ((strcmp(key, "color") == 0)){			
		for (int i=0;i<=2;i++){
			    //objects[2]->plane.color[i] = value[i];
				
			}
		}else if ((strcmp(key, "position") == 0)){
			for (int i=0;i<=2;i++){
				printf("fuck me");
			    //objects[2]->plane.center[i] = value[i];
			}
		}else{
			for (int i=0;i<=2;i++){
			    //objects[2]->plane.normal[i] = value[i];
			}
		}
		
	}
}

void read_scene(char* filename ) {
  Object** objects;
  objects = malloc(sizeof(Object*)*2);
  objects[0] = malloc(sizeof(Object));
  int type = 0;
 
  objects[0]->kind = 0;

  int c;
  FILE* json = fopen(filename, "r"); 
  
  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }
  
  skip_ws(json);
  
  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return ;
    }
    if (c == '{') {
      skip_ws(json);
    
      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
	exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);

      if (strcmp(value, "camera") == 0) {
		  type = 0;
      } else if (strcmp(value, "sphere") == 0) {
		  type = 1;
      } else if (strcmp(value, "plane") == 0) {
		  type =2;
      } else {
	fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
	exit(1);
      }

      skip_ws(json);

      while (1) {
	// , }
	c = next_c(json);
	if (c == '}') {
	  // stop parsing this object
	  break;
	} else if (c == ',') {
	  // read another field
	  skip_ws(json);
	  char* key = next_string(json);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);
	  if ((strcmp(key, "width") == 0) ||
	      (strcmp(key, "height") == 0) ||
	      (strcmp(key, "radius") == 0)) {
	    double value = next_number(json);
		valuesetter(type, key,value, objects);
	  } else if ((strcmp(key, "color") == 0) ||
		     (strcmp(key, "position") == 0) ||
		     (strcmp(key, "normal") == 0)) {
	    double* value = next_vector(json);
		vectorsetter(type, key,value,objects);
	  } else {
	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
		    key, line);
	    //char* value = next_string(json);
	  }
	  skip_ws(json);
	} else {
	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
	  exit(1);
	}
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
	// noop
	skip_ws(json);
      } else if (c == ']') {
		  return;
	fclose(json);
      } else {
	fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	exit(1);
      }
    }
  }

}

int main(int c, char** argv) {
  read_scene(argv[1]);
  
  return 0;
}