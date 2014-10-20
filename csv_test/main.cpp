#include <iostream>
#include <string>
#include <csv.h>

using namespace std;

static int put_comma;

void cb1 (void *s, size_t i, void *p) {
  if (put_comma)
    putc(',', stdout);
  csv_fwrite(stdout, s, i);
  put_comma = 1;
}

void cb2 (int c, void *p) {
  put_comma = 0;
  putc('\n', stdout);
}

int main(){
  string csvstring="Test, 123\nFun?,HELL NO\nTest is done?,YES";
  struct csv_parser p;
  csv_init(&p, 0);
  char c;
  for(int i=0; i < csvstring.length(); i++){
    c = csvstring[i];
    if(csv_parse(&p, &c, 1, cb1, cb2, NULL) != 1){
      fprintf(stderr, "Error: %s\n", csv_strerror(csv_error(&p)));
      exit(EXIT_FAILURE);
    }
  }
  csv_fini(&p, cb1, cb2, NULL);
  csv_free(&p);
  return 0;
}
