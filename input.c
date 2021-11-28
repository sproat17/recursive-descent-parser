#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char * scan(char ch);
void program(void);
void stmt_list(void);
void stmt(void);
void expr(void);
void term_tail(void);
void term(void);
void factor_tail(void);
void factor(void);
void add_op(void);
void mult_op(void);
void search(void);
int fpeek(void);

int j = 0;
int k = 0;
int depth = 0;
char * hold[16];
char * tokens[16];
char name[20];
FILE * fp;

int main(void) {
  printf("Parser ");
  scanf("%s", name);

  fp = fopen(name, "r");

  while (fp == NULL) {
    printf("File %s does not exist\n", name);
    printf("Scanner ");
    scanf("%s", name);
    fp = fopen(name, "r");
  }

  char ch;
  for (; (ch = fgetc(fp)) != EOF;) {
    tokens[j] = scan(ch);
  }

  tokens[j] = "$$";

  for (int i = 0; i < j+1; i++) {
    if(hold[i] == NULL){
      hold[i] = tokens[i];
    }
  }

  program();
  return 0;
}

int fpeek(void) {
  int c = getc(fp);
  ungetc(c, fp);
  return c;
}

void next(void) {
  fseek(fp, 1, SEEK_CUR);
}

void error(void) {
  printf("error\n");
  exit(0);
}

char * scan(char ch) {
  char * num = malloc(sizeof(64));
  int i = 0;
  switch (ch) {
    case ' ':
      break;
    case '\n':
      break;
    case '/':
      if (fpeek() == '/') {
        while (fpeek() != '\n') {
          next();
        }
        break;
      } else if (fpeek() != '*') {
        j++;
        return "division";
        break;
      }
      while ((fpeek() != '*') | (fpeek() != '/') | (fpeek() != EOF)) {
        next();
        if ((fpeek() == '/') | (fpeek() == EOF)) {
          next();
          break;
        }
      }
      break;
    case '(':
      j++;
      return "lparen";
      break;
    case ')':
      j++;
      return "rparen";
      break;
    case '+':
      j++;
      return "plus";
      break;
    case '-':
      j++;
      return "minus";
      break;
    case ':':
      if (fpeek() == '=') {
        next();
        j++;
        return "assign";
      } else {
        error();
      }
      break;
    case '*':
      j++;
      return "times";
      break;

    case '.':
      num[i] = '.';
      i++;
      if (isdigit(fpeek())) {
        num[i] = fpeek();
        next();
        i++;
      } else {
        error();
      }
      while (isdigit(fpeek())) {
         num[i] = fpeek();
         i++;
         next();
      }
      j++;
      hold[j] = num;
      return "number";
    default:
      if (isdigit(ch)) {
        num[i] = ch;
        i++;
        while ((isdigit(fpeek())) | (fpeek() == '.')) {
          num[i] = fpeek();
          i++;
          next();
          if (fpeek() == '.') {
            num[i] = fpeek();
            i++;
            next();
            while (isdigit(fpeek())) {
              num[i] = fpeek();
              i++;
              next();
            }
            hold[j] = num;
            j++;
            return "number";
          }
        }
        hold[j] = num;
        j++;
        return "number";

      } else if (isalpha(ch)) {
        char * longest = malloc(sizeof(64));
        int k = 0;
        longest[k] = ch;
        k++;
        while (isalpha(fpeek()) | (isdigit(fpeek()))) {
          longest[k] = fpeek();
          next();
          k++;
        }
        if (!strcmp(longest, "read")) {
          j++;
          return "read";
        }
        if (!strcmp(longest, "write")) {
          j++;
          return "write";
        }
        if ((strcmp(longest, "write")) && (strcmp(longest, "read"))) {
          hold[j] = longest;
          j++;
          return "id";
        }
        memset(longest, 0, strlen(longest));
        break;
    }
    error();
  }
  return "error\n";
  exit(0);
}

void match(char *expected){
  depth += 6;
  if(!(strcmp(tokens[k], expected))){
    printf("%*c<%s>\n", depth, ' ', expected);
    depth += 6;
    printf("%*c%s\n", depth, ' ', hold[k]);
    depth -= 6;
    printf("%*c</%s>\n", depth, ' ', expected);
    k++;
    depth -= 6;
  } else {
    printf("Parse Error 0\n");
    exit(0);
  }
}

void program(void) {
  printf("<Program>\n");
  if(!(strcmp(tokens[k], "id"))){
    stmt_list();
  } else if(!(strcmp(tokens[k], "read"))) {
    stmt_list();
  } else if(!(strcmp(tokens[k], "write"))) {
    stmt_list();
  } else if(!(strcmp(tokens[k], "$$"))) {
    stmt_list();
  } else {
    printf("Parse Error 1\n");
    exit(0);
  }
  printf("</Program>\n");
}

void stmt_list(void){
  depth += 6;
  printf("%*c<stmt_list>\n", depth, ' ');
  if(!(strcmp(tokens[k], "id"))){
    stmt();
    stmt_list();
  } else if(!(strcmp(tokens[k], "read"))) {
    stmt();
    stmt_list();
  } else if(!(strcmp(tokens[k], "write"))) {
    stmt();
    stmt_list();
  } else if(!(strcmp(tokens[k], "$$"))) {
  } else {
    printf("Parse Error 2");
    exit(0);
  }
  printf("%*c</stmt_list>\n", depth, ' ');
  depth -= 6;
}

void stmt(void){
  depth += 6;
  printf("%*c<stmt>\n", depth, ' ');
  if(!(strcmp(tokens[k], "id"))){
    match("id");
    match("assign");
    expr();
  } else if(!(strcmp(tokens[k], "read"))){
    match("read");
    match("id");
  } else if(!(strcmp(tokens[k], "write"))){
    match("write");
    expr();
  } else {
    printf("Parse Error 3\n");
  }
  printf("%*c</stmt>\n", depth, ' ');
  depth -= 6;
}

void expr(void){
  depth += 6;
  printf("%*c<expr>\n", depth, ' ');
  if(!(strcmp(tokens[k], "id"))){
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "number"))){
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "lparen"))){
    term();
    term_tail();
  } else {
    printf("Parse Error 4\n");
    exit(0);
  }
  printf("%*c</expr>\n", depth, ' ');
  depth -= 6;
}

void term_tail(void){
  depth += 6;
  printf("%*c<term_tail>\n", depth, ' ');
  if(!(strcmp(tokens[k], "plus"))){
    add_op();
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "minus"))){
    add_op();
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "rparen"))){
  } else if(!(strcmp(tokens[k], "id"))){
  } else if(!(strcmp(tokens[k], "read"))) {
  } else if(!(strcmp(tokens[k], "write"))) {
  } else if(!(strcmp(tokens[k], "$$"))) {
  } else {
    printf("Parse Error 6\n");
    exit(0);
  }
  printf("%*c</term_tail>\n", depth, ' ');
  depth -= 6;
}

void term(void){
  depth += 6;
  printf("%*c<term>\n", depth, ' ');
  if(!(strcmp(tokens[k], "id"))){
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "number"))){
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "lparen"))){
    factor();
    factor_tail();
  } else {
    printf("Parse Error 6\n");
  }
  printf("%*c</term>\n", depth, ' ');
  depth -= 6;
}

void factor_tail(void){
  depth += 6;
  printf("%*c<factor_tail>\n", depth, ' ');
  if(!(strcmp(tokens[k], "times"))){
    mult_op();
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "division"))){
    mult_op();
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "plus"))){
  } else if(!(strcmp(tokens[k], "minus"))) {
  } else if(!(strcmp(tokens[k], "rparen"))) {
  } else if(!(strcmp(tokens[k], "id"))){
  } else if(!(strcmp(tokens[k], "read"))) {
  } else if(!(strcmp(tokens[k], "write"))) {
  } else if(!(strcmp(tokens[k], "$$"))) {
  } else {
    printf("Parse Error 7\n");
    exit(0);
  }
  printf("%*c</factor_tail>\n", depth, ' ');
  depth -= 6;
}

void factor(void){
  depth += 6;
  printf("%*c<factor>\n", depth, ' ');
  if(!(strcmp(tokens[k], "id"))){
    match("id");
  } else if(!(strcmp(tokens[k], "number"))){
    match("number");
  } else if(!(strcmp(tokens[k], "lparen"))){
    match("lparen");
    expr();
    match("rparen");
  } else {
    printf("Parse Error 8\n");
  }
  printf("%*c</factor>\n", depth, ' ');
  depth -= 6;
}

void add_op(void){
  depth += 6;
  printf("%*c<add_op>\n", depth, ' ');
  if(!(strcmp(tokens[k], "plus"))){
    match("plus");
  } else if(!(strcmp(tokens[k], "minus"))){
    match("minus");
  } else {
    printf("Parse Error 9\n");
  }
  printf("%*c</add_op>\n", depth, ' ');
  depth -= 6;
}

void mult_op(void){
  depth += 6;
  printf("%*c<mult_op>\n", depth, ' ');
  if(!(strcmp(tokens[k], "times"))){
    match("times");
  } else if(!(strcmp(tokens[k], "division"))){
    match("division");
  } else {
    printf("Parse Error 6\n");
  }
  printf("%*c</mult_op>\n", depth, ' ');
  depth -= 6;
}
