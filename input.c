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

  search();
  printf("\n");
  program();
  return 0;
}

void search(void) {
  char ch;
  for (; (ch = fgetc(fp)) != EOF;) {
    tokens[j] = scan(ch);
  }
  tokens[j] = "$$";

  for (int i = 0; i < j; i++) {
    printf("%s ", tokens[i]);
  }
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
  exit(1);
}

char * scan(char ch) {

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
      if (isdigit(fpeek())) {
        next();
        j++;
        return "number";
      } else {
        error();
      }
      while (isdigit(fpeek())) {
        next();
      }
      break;
    default:
      if (isdigit(ch)) {
        while ((isdigit(fpeek())) | (fpeek() == '.')) {
          next();
          if (fpeek() == '.') {
            next();
            while (isdigit(fpeek())) {
              next();
            }
            j++;
            return "number";
          }
        }
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
  if(!(strcmp(tokens[k], expected))){
    printf("\t<%s>\n \t\t%s\n\t</%s>\n", expected, expected, expected);
    k++;
  } else {
    printf("Parse Error 0\n");
    exit(0);
  }
}

void program(){
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
  printf("\t<stmt_list>\n");
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
  printf("\t</stmt_list>\n");
}

void stmt(void){
  printf("<stmt>\n");
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
  printf("</stmt>\n");
}

void expr(void){
  printf("<expr>\n");
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
  }
  printf("</expr>\n");
}

void term_tail(void){
  printf("<term_tail>\n");
  if(!(strcmp(tokens[k], "plus"))){
    add_op();
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "minus"))){
    add_op();
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "lparen"))){
    term();
    term_tail();
  } else if(!(strcmp(tokens[k], "id"))){
  } else if(!(strcmp(tokens[k], "read"))) {
  } else if(!(strcmp(tokens[k], "write"))) {
  } else if(!(strcmp(tokens[k], "$$"))) {
  } else {
    printf("Parse Error 5\n");
    exit(0);
  }
  printf("</term_tail>\n");
}

void term(void){
  printf("<term>\n");
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
  printf("</term>\n");
}

void factor_tail(void){
  printf("<factor_tail>\n");
  if(!(strcmp(tokens[k], "times"))){
    mult_op();
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "div"))){
    mult_op();
    factor();
    factor_tail();
  } else if(!(strcmp(tokens[k], "plus"))){
  } else if(!(strcmp(tokens[k], "minus"))) {
  } else if(!(strcmp(tokens[k], "read"))) {
  } else if(!(strcmp(tokens[k], "write"))) {
  } else if(!(strcmp(tokens[k], "$$"))) {
  } else {
    printf("Parse Error 7\n");
    exit(0);
  }
  printf("</factor_tail>\n");
}

void factor(void){
  printf("<factor>\n");
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
  printf("</factor>\n");
}

void add_op(void){
  printf("<add_op>\n");
  if(!(strcmp(tokens[k], "plus"))){
    match("plus");
  } else if(!(strcmp(tokens[k], "minus"))){
    match("minus");
  } else {
    printf("Parse Error 9\n");
  }
  printf("</add_op>\n");
}

void mult_op(void){
  printf("<mult_op>\n");
  if(!(strcmp(tokens[k], "times"))){
    match("times");
  } else if(!(strcmp(tokens[k], "div"))){
    match("div");
  } else {
    printf("Parse Error 10\n");
  }
  printf("</mult_op>\n");
}
