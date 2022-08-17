#include "macros.h"


PUTS:         .equlab 0x101C

              .section libcode
              .public _Stub_puts
_Stub_puts:
			  
              
			  and     ##0x00ff      ; zero extend char, we always succeed
			  xba
              jsl     PUTS
              return
