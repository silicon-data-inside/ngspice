/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Thomas L. Quarles
**********/

#include "ngspice/config.h"
#include "ngspice/cktdefs.h"
#include "ngspice/devdefs.h"
#include "ngspice/sperror.h"
#include "string.h"


static GENinstance *
find_instance(GENinstance *here, IFuid name)
{
    for (; here; here = here->GENnextInstance)
        if (here->GENname == name)
            return here;

    return NULL;
}


int
CKTfndDev(CKTcircuit *ckt, int *type, GENinstance **fast, IFuid name, GENmodel *modfast, IFuid modname)
{
    GENinstance *here;
    GENmodel *mods;

    if (fast &&
       *fast)
    {
        /* already have fast, so nothing much to do just get & set type */
        if (type)
            *type = (*fast)->GENmodPtr->GENmodType;
        return OK;
    }

    if (modfast) {
        /* have model, just need device */
        mods = modfast;
        here = find_instance(mods->GENinstances, name);
        if (here) {
            if (fast)
                *fast = here;

            if (type)
                *type = mods->GENmodType;

            return OK;
        }
        return E_NODEV;
    }

    if (*type >= 0 && *type < DEVmaxnum) {
        /* have device type, need to find model & device */
        /* look through all models */
        for (mods = ckt->CKThead[*type];
             mods ;
             mods = mods->GENnextModel)
        {
            /* and all instances */
            if (!modname || mods->GENmodName == modname) {
                here = find_instance(mods->GENinstances, name);
                if (here) {
                    if (fast)
                        *fast = here;
                    return OK;
                }
                if (mods->GENmodName == modname)
                    return E_NODEV;
            }
        }
        return E_NOMOD;
    } else if (*type == -1) {
        /* look through all types (UGH - worst case - take forever) */
        for (*type = 0; *type < DEVmaxnum; (*type)++) {
            /* need to find model & device */
            /* look through all models */
            for (mods = ckt->CKThead[*type]; mods;
                 mods = mods->GENnextModel)
            {
                /* and all instances */
                if (!modname || mods->GENmodName == modname) {
                    here = find_instance(mods->GENinstances, name);
                    if (here) {
                        if (fast)
                            *fast = here;
                        return OK;
                    }
                    if (mods->GENmodName == modname)
                        return E_NODEV;
                }
            }
        }
        *type = -1;
        return E_NODEV;
    } else
        return E_BADPARM;
}
