LIB "tst.lib"; tst_init();
LIB "fpadim.lib";
ring r = 0,(x,y),dp;
def R = makeLetterplaceRing(5); // constructs a Letterplace ring
setring R; // sets basering to Letterplace ring
ideal G = x*x, y*y,x*y*x; // ideal G contains a
//Groebner basis
lpSickleHil(G); // invokes the procedure with ring parameters
// the factor algebra is finite, so the degree bound given by the Letterplace
// ring is not necessary
lpSickleHil(G,0); // procedure without any degree bound
tst_status(1);$
