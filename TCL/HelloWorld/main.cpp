#include <tcl.h>
#include <stdio.h>

int Tcl_AppInit(Tcl_Interp *interp)
{
    return (Tcl_Init(interp) == (TCL_ERROR))?TCL_ERROR:TCL_OK;
}

int main(int argc, char *argv[])
{
    Tcl_Main(argc, argv, Tcl_AppInit);

    return 0;
}
