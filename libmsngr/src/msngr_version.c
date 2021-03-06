/*******************************************************************************
*
*  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
*
********************************************************************************
*
*  Author:
*     name:  Brian Ermold
*     phone: (509) 375-2277
*     email: brian.ermold@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 65881 $
*    $Author: ermold $
*    $Date: 2015-11-13 21:48:23 +0000 (Fri, 13 Nov 2015) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file msngr_version.c
 *  libmsngr library version.
 */

/**
 *  @defgroup MSNGR_VERSION Library Version
 */
/*@{*/

/** @privatesection */

static char *_VersionTag = "$Version: afl-libmsngr-1.7 $";
static char  _Version[64];

/** @publicsection */

/**
 *  libmsngr library version.
 *
 *  @return libmsngr library version
 */
const char *msngr_lib_version(void)
{
    char *tp = _VersionTag;
    char *vp = _Version;

    if (!*vp) {

        /* find colon */

        for (; *tp != ':'; tp++)
            if (*tp == '\0') return((const char *)_Version);

        /* skip spaces */

        for (tp++; *tp == ' '; tp++)
            if (*tp == '\0') return((const char *)_Version);

        /* extract version from tag */

        while (*tp != ' ' && *tp != '$' && *tp != '\0')
            *vp++ = *tp++;

        *vp = '\0';
    }

    return((const char *)_Version);
}

/*@}*/
