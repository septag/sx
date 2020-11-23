//
// Copyright 2018 Sepehr Taghdisian (septag@github). All rights reserved.
// License: https://github.com/septag/sx#license-bsd-2-clause
//
// parts of this code is copied from bx library: https://github.com/bkaradzic/bx
// Copyright 2011-2019 Branimir Karadzic. All rights reserved.
// License: https://github.com/bkaradzic/bx#license-bsd-2-clause
//
//
// math.h - 1.5    Scalar and Vector math functions
//                 Contains vector primitives and vector/fpu math functions, event functions implemented in libm
//                 Some essential functions are taken from bx library (https://github.com/bkaradzic/bx)
// Easings:
//      Reference: https://easings.net/
//                 https://github.com/r-lyeh-archived/tween
//
// Conventions:
//      The lib prefers Right-Handed system, although there are functions for both LH or RH system
//      for calulating view/projection matrices
//      Rotations are CCW
//      Matrices are Column-Major, but the representation is row-major, which means:
//          mat->m[r][c] -> which R is the row, and C is column index
//          transform vector (v) by matrix (M) = M.v
//          matrix transforms are multiplied in reverse: Scale->Rotation->Translate = TxRxS
//
// C++ Programmers:
//     Some useful operators for basic vector and matrix arithmatic are included. See the end of math-vec.h
//
#pragma once

#include "sx.h"
#include "math-types.h"
#include "math-scalar.h"
#include "math-vec.h"
#include "math-easing.h"

//
// Version history
//      v1.0.0      Initial release
//      v1.1.0      Matrices are now column-major (in memory)
//                  Added SX_VECx_ constants
//      v1.1.1      Fixed through the whole API for RH proj and view calculations
//      v1.2.0      Added new primitives like color and rect
//      v1.2.1      Moved std-math.h to C unit
//      v1.3        Added tx3d (transform), box and plane primitives
//      v1.4        Added more primitives, and quaternion lerping
//      v1.5        Divided math.h into multiple headers for faster build times
