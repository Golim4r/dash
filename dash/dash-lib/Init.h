/* 
 * dash-lib/Init.h
 *
 * author(s): Karl Fuerlinger, LMU Munich 
 */
/* @DASH_HEADER@ */

#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include "dart.h"

//
// thin wrappers around the corresponding
// DART functions
//

namespace dash
{
  void init(int *argc, char ***argv);
  void finalize();

  int    myid();
  size_t size();

void barrier();
};


#endif /* INIT_H_INCLUDED */