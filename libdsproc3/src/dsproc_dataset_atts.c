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
*    $Revision: 16063 $
*    $Author: ermold $
*    $Date: 2012-11-30 08:19:22 +0000 (Fri, 30 Nov 2012) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_dataset_atts.c
 *  Dataset Attribute Functions.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @privatesection */

/*******************************************************************************
 *  Static Functions Visible Only To This Module
 */

/*******************************************************************************
 *  Private Functions Visible Only To This Library
 */

/** @publicsection */

/*******************************************************************************
 *  Internal Functions Visible To The Public
 */

/*******************************************************************************
 *  Public Functions
 */

/**
 *  Change an attribute for a dataset or variable.
 *
 *  This function will define the specified attribute if it does not exist.
 *  If the attribute does exist and the overwrite flag is set, the data type
 *  and value will be changed.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent    - pointer to the parent CDSGroup or CDSVar
 *  @param  overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
 *  @param  name      - attribute name
 *  @param  type      - attribute data type
 *  @param  length    - attribute length
 *  @param  value     - pointer to the attribute value
 *
 *  @return
 *    - 1 if successful
 *    - 0 if:
 *        - the parent object is not a group or variable
 *        - the parent group or variable definition is locked
 *        - the attribute definition is locked
 *        - a memory allocation error occurred
 */
int dsproc_change_att(
    void        *parent,
    int          overwrite,
    const char  *name,
    CDSDataType  type,
    size_t       length,
    void        *value)
{
    CDSAtt *att = cds_change_att(
        parent, overwrite, name, type, length, value);

    if (!att) {
        dsproc_set_status(DSPROC_ECDSCHANGEATT);
        return(0);
    }

    return(1);
}

/**
 *  Get an attribute from a dataset or variable.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *
 *  @return
 *    - pointer to the attribute
 *    - NULL if the attribute does not exist
 */
CDSAtt *dsproc_get_att(
    void        *parent,
    const char  *name)
{
    return(cds_get_att(parent, name));
}

/**
 *  Get a copy of an attribute value from a dataset or variable.
 *
 *  This function will get a copy of an attribute value converted to a
 *  text string. If the data type of the attribute is not CDS_CHAR the
 *  cds_array_to_string() function is used to create the output string.
 *
 *  Memory will be allocated for the returned string if the output string
 *  is NULL. In this case the calling process is responsible for freeing
 *  the allocated memory.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  length - pointer to the length of the output string
 *                     - input:
 *                         - length of the output string
 *                         - ignored if the output string is NULL
 *                     - output:
 *                         - number of characters written to the output string
 *                         - 0 if the attribute value has zero length
 *                         - (size_t)-1 if a memory allocation error occurs
 *  @param  value  - pointer to the output string
 *                   or NULL to dynamically allocate the memory needed.
 *
 *  @return
 *    - pointer to the output string
 *    - NULL if:
 *        - the attribute does not exist or has zero length (length = 0)
 *        - a memory allocation error occurs (length == (size_t)-1)
 */
char *dsproc_get_att_text(
    void       *parent,
    const char *name,
    size_t     *length,
    char       *value)
{
    CDSAtt *att        = cds_get_att(parent, name);
    size_t  tmp_length = 0;

    if (!length) {
        length = &tmp_length;
    }

    if (!att) {
        *length = 0;
        return((char *)NULL);
    }

    value = cds_get_att_text(att, length, value);

    if (*length == (size_t)-1) {
        dsproc_set_status(DSPROC_ENOMEM);
    }

    return(value);
}

/**
 *  Get a copy of an attribute value from a dataset or variable.
 *
 *  This function will get a copy of an attribute value casted into
 *  the specified data type. The functions cds_string_to_array() and
 *  cds_array_to_string() are used to convert between text (CDS_CHAR)
 *  and numeric data types.
 *
 *  Memory will be allocated for the returned array if the output array
 *  is NULL. In this case the calling process is responsible for freeing
 *  the allocated memory.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  type   - data type of the output array
 *  @param  length - pointer to the length of the output array
 *                     - input:
 *                         - length of the output array
 *                         - ignored if the output array is NULL
 *                     - output:
 *                         - number of values written to the output array
 *                         - 0 if the attribute value has zero length
 *                         - (size_t)-1 if a memory allocation error occurs
 *  @param  value  - pointer to the output array
 *                   or NULL to dynamically allocate the memory needed.
 *
 *  @return
 *    - pointer to the output array
 *    - NULL if:
 *        - the attribute value has zero length (length == 0)
 *        - a memory allocation error occurs (length == (size_t)-1)
 */
void *dsproc_get_att_value(
    void         *parent,
    const char   *name,
    CDSDataType   type,
    size_t       *length,
    void         *value)
{
    CDSAtt *att        = cds_get_att(parent, name);
    size_t  tmp_length = 0;

    if (!length) {
        length = &tmp_length;
    }

    if (!att) {
        *length = 0;
        return((void *)NULL);
    }

    value = cds_get_att_value(att, type, length, value);

    if (*length == (size_t)-1) {
        dsproc_set_status(DSPROC_ENOMEM);
    }

    return(value);
}

/**
 *  Set the value of an attribute in a dataset or variable.
 *
 *  This function will define the specified attribute if it does not exist.
 *  If the attribute does exist and the overwrite flag is set, the value will
 *  be set by casting the specified value into the data type of the attribute.
 *  The functions cds_string_to_array() and cds_array_to_string() are used to
 *  convert between text (CDS_CHAR) and numeric data types.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent    - pointer to the parent CDSGroup or CDSVar
 *  @param  overwrite - overwrite flag (1 = TRUE, 0 = FALSE)
 *  @param  name      - attribute name
 *  @param  type      - attribute data type
 *  @param  length    - attribute length
 *  @param  value     - pointer to the attribute value
 *
 *  @return
 *    - 1 if successful
 *    - 0 if:
 *        - the parent object is not a group or variable
 *        - the parent group or variable definition is locked
 *        - the attribute definition is locked
 *        - a memory allocation error occurred
 */
int dsproc_set_att(
    void        *parent,
    int          overwrite,
    const char  *name,
    CDSDataType  type,
    size_t       length,
    void        *value)
{
    CDSAtt *att = cds_set_att(
        parent, overwrite, name, type, length, value);

    if (!att) {
        dsproc_set_status(DSPROC_ECDSSETATT);
        return(0);
    }

    return(1);
}

/**
 *  Set the value of an attribute in a dataset or variable.
 *
 *  The cds_string_to_array() function will be used to set the attribute
 *  value if the data type of the attribute is not CDS_CHAR.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 *
 *  @return
 *    - 1 if successful
 *    - 0 if:
 *        - the attribute does not exist
 *        - the attribute definition is locked
 *        - a memory allocation error occurred
 */
int dsproc_set_att_text(
    void        *parent,
    const char  *name,
    const char *format, ...)
{
    va_list args;
    char   *string;
    size_t  length;
    int     retval;

    va_start(args, format);
    string = msngr_format_va_list(format, args);
    va_end(args);

    if (!string) {

        ERROR( DSPROC_LIB_NAME,
            "Could not set attribute value for: %s/_atts_/%s\n"
            " -> memory allocation error\n",
            cds_get_object_path(parent), name);

        dsproc_set_status(DSPROC_ENOMEM);
        return(0);
    }

    length = strlen(string) + 1;
    retval = dsproc_set_att_value(parent, name, CDS_CHAR, length, string);

    free(string);

    return(retval);
}

/**
 *  Set the value of an attribute in a dataset or variable.
 *
 *  This function will set the value of an attribute by casting the
 *  specified value into the data type of the attribute. The functions
 *  cds_string_to_array() and cds_array_to_string() are used to convert
 *  between text (CDS_CHAR) and numeric data types.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  type   - data type of the specified value
 *  @param  length - length of the specified value
 *  @param  value  - pointer to the attribute value
 *
 *  @return
 *    - 1 if successful
 *    - 0 if:
 *        - the attribute does not exist
 *        - the attribute definition is locked
 *        - a memory allocation error occurred
 */
int dsproc_set_att_value(
    void        *parent,
    const char  *name,
    CDSDataType  type,
    size_t       length,
    void        *value)
{
    CDSAtt *att = cds_get_att(parent, name);
    int     status;

    if (att) {

        if (att->def_lock) {

            ERROR( DSPROC_LIB_NAME,
                "Could not set attribute value for: %s\n"
                " -> attribute value was defined in the DOD\n",
                cds_get_object_path(att));

            dsproc_set_status(DSPROC_ECDSSETATT);
            return(0);
        }

        status = cds_set_att_value(att, type, length, value);
        if (!status) {
            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }
    }
    else {

        ERROR( DSPROC_LIB_NAME,
            "Could not set attribute value for: %s/_atts_/%s\n"
            " -> attribute does not exist\n",
            cds_get_object_path(parent), name);

        dsproc_set_status(DSPROC_ECDSSETATT);
        return(0);
    }

    return(1);
}

/**
 *  Set the value of an attribute if the current value is NULL.
 *
 *  This function will check if the value for the specified attribute is NULL.
 *
 *  If the attribute does not exist or the value is not NULL, nothing will be
 *  done and the function will return successfully.
 *
 *  If the value is NULL, it will be set by casting the specified value into
 *  the data type of the attribute. The functions cds_string_to_array() and
 *  cds_array_to_string() are used to convert between text (CDS_CHAR) and
 *  numeric data types.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  type   - data type of the specified value
 *  @param  length - length of the specified value
 *  @param  value  - pointer to the attribute value
 *
 *  @return
 *    - 1 if successful
 *    - 0 if a memory allocation error occurred
 */
int dsproc_set_att_value_if_null(
    void        *parent,
    const char  *name,
    CDSDataType  type,
    size_t       length,
    void        *value)
{
    CDSAtt *att = cds_get_att(parent, name);
    int     status;

    if (att && att->length == 0) {
    
        att->def_lock = 0;

        status = cds_set_att_value(att, type, length, value);
        if (!status) {
            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }
    }

    return(1);
}

/**
 *  Set the value of an attribute if the current value is NULL.
 *
 *  This function will check if the value for the specified attribute is NULL.
 *
 *  If the attribute does not exist or the value is not NULL, nothing will be
 *  done and the function will return successfully.
 *
 *  If the value is NULL, it will be set to the specified value. The
 *  cds_string_to_array() function will be used to set the attribute
 *  value if the data type of the attribute is not CDS_CHAR.
 *
 *  If an error occurs in this function it will be appended to the log and
 *  error mail messages, and the process status will be set appropriately.
 *
 *  @param  parent - pointer to the parent CDSGroup or CDSVar
 *  @param  name   - name of the attribute
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 *
 *  @return
 *    - 1 if successful
 *    - 0 if a memory allocation error occurred
 */
int dsproc_set_att_text_if_null(
    void        *parent,
    const char  *name,
    const char *format, ...)
{
    CDSAtt *att = cds_get_att(parent, name);
    int     status;
    va_list args;
    char   *string;
    size_t  length;

    if (att && att->length == 0) {

        att->def_lock = 0;

        va_start(args, format);
        string = msngr_format_va_list(format, args);
        va_end(args);

        if (!string) {

            ERROR( DSPROC_LIB_NAME,
                "Could not set attribute value for: %s/_atts_/%s\n"
                " -> memory allocation error\n",
                cds_get_object_path(parent), name);

            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }

        length = strlen(string) + 1;
        status = cds_set_att_value(att, CDS_CHAR, length, string);

        free(string);

        if (!status) {
            dsproc_set_status(DSPROC_ENOMEM);
            return(0);
        }
    }

    return(1);
}
