#ifndef _LANG_XLS_H_
#define _LANG_XLS_H_

#include <stdbool.h>

#define XLS_TMP_PATH "/app/app/language.xls"
#define XLS_NFS_PATH "/mnt/nfs/language.xls"
#define CODE "UTF-8"

typedef enum
{
    XLS_LANG_COL_PAGE = 0,
    XLS_LANG_COL_ENGLISH,
    XLS_LANG_COL_ARABIC,
    XLS_LANG_COL_HUNGARIAN,
    XLS_LANG_COL_SLOVAK,
    XLS_LANG_COL_ROMANIAN,
    XLS_LANG_COL_SERBIAN,
    XLS_LANG_COL_GERMAN,
    XLS_LANG_COL_POLISH,
    XLS_LANG_COL_PORTUGUESE,
    XLS_LANG_COL_FRENCH,
    XLS_LANG_COL_CHINESE,
    XLS_LANG_COL_TOTAL
} XLS_LANG_COL;

char ***lang_xls_init(int sheet_num);

bool lang_xls_file_state_get(void);
int lang_xls_null_str_num_get(void);
int lang_xls_language_num_get(void);
int lang_xls_str_num_get(void);

bool init_language_xls_info(void);
bool is_language_xls_inited(void);

const char *lang_xls_str_get(int row, int col);
const char *lang_xls_language_name_get(int col);

#endif
