LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = 0,(x,y,z),Dp;
int d =4; // degree bound
def R = freeAlgebra(r,d);
setring R;
ideal I = x*y + y*z, x*x + x*y - y*x - y*y;
option(redSB); option(redTail);
ideal J = std(I);
J;
tst_status(1);$
