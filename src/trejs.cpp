#include "emscripten.h"
#include <tre.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>

EMSCRIPTEN_KEEPALIVE
std::string check_error(int result) {
  if (result == REG_NOMATCH) {
    return "no match";
  } else if (result == REG_BADPAT) {
    return "bad pattern";
  } else if (result == REG_ECOLLATE) {
    return "unknown collating element";
  } else if (result == REG_ECTYPE) {
    return "unknown character class";
  } else if (result == REG_EESCAPE) {
    return "trailing backslash";
  } else if (result == REG_ESUBREG) {
    return "invalid back reference";
  } else if (result == REG_EBRACK) {
    return "[] imbalance";
  } else if (result == REG_EPAREN) {
    return "() imbalance";
  } else if (result == REG_EBRACE) {
    return "{} imbalance";
  } else if (result == REG_BADBR) {
    return "invalid {} content";
  } else if (result == REG_ERANGE) {
    return "invalid use of range operator";
  } else if (result == REG_ESPACE) {
    return "out of memory";
  } else if (result == REG_BADRPT) {
    return "invalid use of repitition operators";
  } else if (result == REG_BADMAX) {
    return "maximum repetition in {} too large";
  }
  return "unknon error";
}


EMSCRIPTEN_KEEPALIVE
std::string findall_(char *exp, char *text) {
  regex_t preg;
  const size_t max_matches = strlen(exp);
  regmatch_t matches[max_matches];

  int result = tre_regcomp(&preg, exp, REG_EXTENDED);

  if (result != REG_OK) {
    return "{\"error\": \"compile: " + check_error(result) + "\"}";
  }
  result = tre_regexec(&preg, text, max_matches, matches, REG_APPROX_MATCHER);

  if (result == REG_NOMATCH) {
    return "[]";
  } else if (result != REG_OK) {
    return "{\"error\": \"match: " + check_error(result) + "\"}";
  }

  std::string response = "[\n";
  std::string txt = std::string(text);
  for(int i = 0; i<max_matches; ++i) {
    if (matches[i].rm_so < 0)
      break;
    if (i > 0)
      response += ",\n";
    response += "  {\"start\": ";
    response += std::to_string(matches[i].rm_so);
    response += ", \"end\": " ;
    response += std::to_string(matches[i].rm_eo);
    response += ", \"value\": \"" ;
    response += txt.substr(matches[i].rm_so, matches[i].rm_eo);
    response += "\"}";
  }
  response += "\n]";

  return response;
}


#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

EMSCRIPTEN_KEEPALIVE
extern "C" const char *findall(char *exp, char *text) {
  std::string result = findall_(exp, text);
  char *cstr = new char[result.size() + 1];
  std::strcpy(cstr, result.c_str());
  return cstr;
}
