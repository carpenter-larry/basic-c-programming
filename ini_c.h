#ifndef INI_C_H_INCLUDED
#define INI_C_H_INCLUDED

/** \brief Initialization file reader and writer

example :
[sectionA]
keyA1=valA1
keyA2=valA2
keyA3=valA3
[sectionB]
keyB1=valB1
keyB2=valB2
keyB3=valB3

 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define _BUF_LEN     256         /**< buffer size */
#define _KEY_LEN     64          /**< max length of key */
#define _VAL_LEN     128         /**< max length of value */


/** \brief read and parse ini file
 *
 * \param file const char*  target ini file
 * \return int  0:ok,1:err
 *
 */
int ini_read_file(const char * file);

/** \brief write ini file
 *
 * \param file const char*  target ini file
 * \return int  0:ok,1:err
 *
 */
int ini_write_file(const char * file);

/** \brief get the value of specified key
 *
 * \param section const char*   [in]target section
 * \param key const char*       [in]target key
 * \param value char*           [out]target value
 * \return int                  0:ok,1:err
 *
 */
int ini_get_value(const char *section, const char *key, char *value);

/** \brief get the value of specified key
 *
 * \param section const char*   [in]target section
 * \param key const char*       [in]target key
 * \param value const char*     [in]target value
 * \return int                  0:ok,1:err
 *
 */
int ini_set_value(const char *section, const char *key, const char *value);

/** \brief release the ini config in memory
 *
 * \return int
 *
 */
int ini_release();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // INI_C_H_INCLUDED
