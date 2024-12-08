%{

#include "y.tab.h"

%}

%%
[ \t\n] { }
[a-zA-Z]+ { return ID; }
, { return COMMA; }
; { return SEMI; }
. { }
%%
