#include <tcl.h>
#include <stdio.h>
#include <string.h>

int evod_objproc(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int a;
    char res[8];

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "?int?");
        return TCL_ERROR;
    }
    else
    {
        if(Tcl_GetIntFromObj(interp, objv[1], &a) != TCL_OK) 
        {
            Tcl_SetStringObj(Tcl_GetObjResult(interp),  "Usage: input should be an integer.", -1);
            return TCL_ERROR;
        }
        strcpy(res, (a%2)?"odd":"even");
        Tcl_SetStringObj(Tcl_GetObjResult(interp), res, -1);
        return TCL_OK;
    }
}

int evod_objproc_list(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    int num; 
    int a, i;
    Tcl_Obj **list;
    Tcl_Obj *result = Tcl_NewListObj(0, NULL);

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, objv, "?list?");
        return TCL_ERROR;
    }
    else
    {
        if(Tcl_ListObjGetElements(interp, objv[1], &num, &list) != TCL_OK) 
        {
            Tcl_SetStringObj(Tcl_GetObjResult(interp),  "Internal Error!", -1);
            return TCL_ERROR;
        }

        for(i = 0; i < num; i++)
        {
            if(Tcl_GetIntFromObj(interp, list[i], &a) != TCL_OK)
                Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(-1));
            else
                Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(a%2));
        }

        Tcl_SetObjResult(interp, result);

        return TCL_OK;
    }
}

int Tcl_AppInit(Tcl_Interp *interp)
{
    if (Tcl_Init(interp) == (TCL_ERROR))
        return TCL_ERROR;

    Tcl_CreateObjCommand(interp, "mem", evod_objproc, (ClientData)0, 0);

    return TCL_OK;
}

int main(int argc, char *argv[])
{
    Tcl_Main(argc, argv, Tcl_AppInit);

    return 0;
}
