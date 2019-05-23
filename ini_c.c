#include "ini_c.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct ini_node {
    char key[_KEY_LEN];
    char value[_VAL_LEN];
    struct ini_node *next;
};

struct ini_section {
    char section[_KEY_LEN];
    struct ini_node *param_list;
    struct ini_section *next;
};

struct ini_section *_config_list = NULL;

/** \brief read and parse ini file
 *
 * \param file const char*  target ini file
 * \return int  0:ok,1:err
 *
 */
int ini_read_file(const char * file)
{
    int ret = 0;
    char buffer[_BUF_LEN] = {0};
    char cur_section[_BUF_LEN] = {0};
    struct ini_section * list = _config_list;
    struct ini_node * param = NULL;
    int i = 0;
    FILE *ini_fp = fopen(file, "r");
    if (!ini_fp) {
        printf("Open ini file error : [%s]\n", strerror(errno));
        return 1;
    }

    for ( ; fgets(buffer, _BUF_LEN, ini_fp); ) {
        /* check if is section*/
        char * section_s = strchr(buffer, '[');
        char * section_e = strchr(buffer, ']');
        if (section_s && section_e) {
            memset(cur_section, 0, _BUF_LEN);
            memcpy(cur_section, section_s + 1, section_e - section_s - 1);
            printf("Section : [%s]\n", cur_section);
            if (!_config_list) {
                _config_list = (struct ini_section *)malloc(sizeof(struct ini_section));
                list = _config_list;
            } else {
                list->next = (struct ini_section *)malloc(sizeof(struct ini_section));
                list = list->next;
            }
            if (!list) {
                perror("malloc memory for section failed.\nExit ... ...\n");
                exit(1);
            }
            memset(list, 0, sizeof(struct ini_section));
            memcpy(list->section, section_s + 1, section_e - section_s - 1);
            param = list->param_list;
            continue;
        }

        /* ignore the config that do not belong to any section */
        if (*cur_section == 0)
            continue;

        /* process the config pair */
        for (i = 0; i < _BUF_LEN; i++) {
            if ((buffer[i] == '\r') || (buffer[i] == '\n'))  {
                buffer[i] = 0;
                break;
            }
        }
        char *pos = strchr(buffer, '=');
        if (!param) {
            list->param_list = (struct ini_node  *)malloc(sizeof(struct ini_node ));
            param = list->param_list;
        } else {
            param->next = (struct ini_node  *)malloc(sizeof(struct ini_node ));
            param = param->next;
        }
        if (!param) {
            perror("malloc memory for param failed.\nExit ... ...\n");
            exit(1);
        }
        memset(param, 0, sizeof(struct ini_node));
        memccpy(param->key, buffer, '=', pos - buffer);
        memccpy(param->value, pos + 1, 0, _VAL_LEN);
        printf("find key[%s], value[%s]\n", param->key, param->value);
    }

    fclose(ini_fp);

    return ret;
}

/** \brief write ini file
 *
 * \param file const char*  target ini file
 * \return int  0:ok,1:err
 *
 */
int ini_write_file(const char * file)
{
    int ret = 0;
    char buffer[_BUF_LEN] = {0};
    struct ini_section *list = _config_list;
    FILE *ini_fp = fopen(file, "w");
    if (!ini_fp) {
        printf("Open ini file error : [%s]\n", strerror(errno));
        return 1;
    }

    for ( ; list; ) {
        struct ini_node *param = list->param_list;

        snprintf(buffer, _BUF_LEN, "[%s]\n", list->section);
        fputs(buffer, ini_fp);
        printf("1\n%s\n", list->section);
        for ( ; param; ) {
            snprintf(buffer, _BUF_LEN, "%s=%s\n", param->key, param->value);
            fputs(buffer, ini_fp);
            printf("2\n%s,%s\n", param->key, param->value);
            param = param->next;
        }
        list = list->next;
    }

    fclose(ini_fp);

    return ret;
}

/** \brief get the value of specified key
 *
 * \param section const char*   [in]target section
 * \param key const char*       [in]target key
 * \param value char*           [out]target value
 * \return int                  0:ok,1:err
 *
 */
int ini_get_value(const char *section, const char *key, char *value)
{
    // int ret = 0;
    struct ini_section *list = _config_list;

    for ( ; list; list = list->next) {
        if (strcmp(section, list->section) != 0) {
            continue;
        }
        struct ini_node *param = list->param_list;
        for ( ; param; ) {
            if (strcmp(key, param->key) == 0) {
                strcpy(value, param->value);
                return 0;
            }
            param = param->next;
        }
    }

    return 1;
}

/** \brief get the value of specified key
 *
 * \param section const char*   [in]target section
 * \param key const char*       [in]target key
 * \param value const char*     [in]target value
 * \return int                  0:ok,1:err
 *
 */
int ini_set_value(const char *section, const char *key, const char *value)
{
    struct ini_section *list = _config_list;
    struct ini_section *pre_list = NULL;
    struct ini_node *pre_param = NULL;

    for ( ; list; ) {
        if (strcmp(section, list->section) != 0) {
            pre_list = list;
            list = list->next;
            continue;
        }
        struct ini_node *param = list->param_list;
        for ( ; param; ) {
            if (strcmp(key, param->key) == 0) {
                strcpy(param->value, value);
                return 0;
            }
            pre_param = param;
            param = param->next;
        }
        if (!param) {
            param = (struct ini_node  *)malloc(sizeof(struct ini_node ));
            if (!param) {
                perror("malloc memory for param failed.\nExit ... ...\n");
                exit(1);
            }
            memset(param, 0, sizeof(struct ini_node));
            strcpy(param->key, key);
            strcpy(param->value, value);
            if (pre_param)
                pre_param->next = param;
            else
                list->param_list = param;
            return 0;
        }
    }

    if (!list) {
        list = (struct ini_section *)malloc(sizeof(struct ini_section));
        if (!list) {
            perror("malloc memory for section failed.\nExit ... ...\n");
            exit(1);
        }
        memset(list, 0, sizeof(struct ini_section));
        strcpy(list->section, section);
        pre_list->next = list;
        struct ini_node *param = (struct ini_node  *)malloc(sizeof(struct ini_node ));
        if (!param) {
            perror("malloc memory for param failed.\nExit ... ...\n");
            exit(1);
        }
        list->param_list = param;
        memset(param, 0, sizeof(struct ini_node));
        strcpy(param->key, key);
        strcpy(param->value, value);
    }

    return 0;
}

/** \brief release the ini config in memory
 *
 * \return int
 *
 */
int ini_release()
{
    int ret = 0;
    struct ini_section *list = _config_list;
    struct ini_section *tmp_list = NULL;

    for ( ; list; ) {
        tmp_list = list->next;
        struct ini_node *param = list->param_list;
        struct ini_node *tmp_param;
        for ( ; param; ) {
            tmp_param = param->next;
            free(param);
            param = tmp_param;
        }
        free(list);
        list = tmp_list;
    }

    return ret;
}

#ifdef __cplusplus
}
#endif // __cplusplus

