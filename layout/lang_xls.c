#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../share/include/libxls/xls.h"
#include "lang_xls.h"

typedef struct
{
    bool xls_is_exist;
    int xls_null_str_num;
    int row_total;
    int col_total;
} xls_info_t;

static xls_info_t xls_info = {false, 0, 0, 0};
static char ***buffer = NULL;
static bool language_xls_init_state = false;

static char *dup_or_empty(const char *src)
{
    const char *value = src == NULL ? "" : src;
    size_t len = strlen(value);
    char *dst = malloc(len + 1);

    if (dst == NULL)
    {
        return NULL;
    }

    memcpy(dst, value, len + 1);
    return dst;
}

static char *trim_ascii_prefix(char *text)
{
    unsigned char *p = (unsigned char *)text;

    while (*p != '\0')
    {
        if (isalnum(*p) || *p == '"' || *p == '(')
        {
            break;
        }
        if (*p >= 0x80)
        {
            p++;
            continue;
        }
        p++;
    }

    while (*p == '"' || isspace(*p))
    {
        p++;
    }

    return (char *)p;
}

static void sanitize_cell(char *text)
{
    char *src = text;
    char *dst = text;

    while (*src != '\0')
    {
        unsigned char c = (unsigned char)*src++;

        if (c < 0x20 || c == 0x7f)
        {
            break;
        }
        *dst++ = (char)c;
    }
    *dst = '\0';

    src = text;
    while (*src != '\0' && (isspace((unsigned char)*src) || *src == '"'))
    {
        src++;
    }

    if (src != text)
    {
        memmove(text, src, strlen(src) + 1);
    }

    dst = text + strlen(text);
    while (dst > text && (isspace((unsigned char)dst[-1]) || dst[-1] == '"'))
    {
        dst--;
    }
    *dst = '\0';

    if (strcmp(text, "*failed to decode utf16*") == 0)
    {
        text[0] = '\0';
    }
}

static void clean_language_name(const char *raw, char *cleaned, size_t cleaned_size)
{
    char tmp[128];
    char *trimmed;

    if (cleaned_size == 0)
    {
        return;
    }

    if (raw == NULL)
    {
        cleaned[0] = '\0';
        return;
    }

    strncpy(tmp, raw, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    sanitize_cell(tmp);
    trimmed = trim_ascii_prefix(tmp);

    strncpy(cleaned, trimmed, cleaned_size - 1);
    cleaned[cleaned_size - 1] = '\0';
}

static void normalize_lookup_text(const char *src, char *dst, size_t dst_size)
{
    size_t out = 0;

    if (dst_size == 0)
    {
        return;
    }

    if (src == NULL)
    {
        dst[0] = '\0';
        return;
    }

    while (*src != '\0' && out + 1 < dst_size)
    {
        unsigned char c = (unsigned char)*src++;

        if (c >= 0x80)
        {
            continue;
        }
        if ((c >= 'A') && (c <= 'Z'))
        {
            c = (unsigned char)(c - 'A' + 'a');
        }
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
        {
            dst[out++] = (char)c;
        }
    }

    dst[out] = '\0';
}

static bool english_cell_matches(const char *cell, const char *fallback)
{
    char normalized_cell[128];
    char normalized_fallback[128];

    normalize_lookup_text(fallback, normalized_fallback, sizeof(normalized_fallback));
    if (normalized_fallback[0] == '\0')
    {
        return true;
    }

    normalize_lookup_text(cell, normalized_cell, sizeof(normalized_cell));
    return strcmp(normalized_cell, normalized_fallback) == 0;
}

char ***lang_xls_init(int sheet_num)
{
    xlsWorkBook *pWb = xls_open(XLS_TMP_PATH, CODE);
    xlsWorkSheet *pWs;
    int i;
    int j;

    if (pWb == NULL)
    {
        printf("[language_xls] open failed: %s\n", XLS_TMP_PATH);
        return NULL;
    }

    pWs = xls_getWorkSheet(pWb, sheet_num);
    xls_parseWorkSheet(pWs);

    xls_info.row_total = pWs->rows.lastrow + 1;
    xls_info.col_total = pWs->rows.lastcol + 1;
    printf("[language_xls] rows=%d cols=%d\n", xls_info.row_total, xls_info.col_total);

    buffer = malloc(sizeof(char **) * xls_info.row_total);
    if (buffer == NULL)
    {
        xls_close_WS(pWs);
        xls_close_WB(pWb);
        return NULL;
    }

    for (i = 0; i < xls_info.row_total; i++)
    {
        buffer[i] = malloc(sizeof(char *) * xls_info.col_total);
        if (buffer[i] == NULL)
        {
            xls_close_WS(pWs);
            xls_close_WB(pWb);
            return NULL;
        }

        for (j = 0; j < xls_info.col_total; j++)
        {
            const char *cell = (const char *)pWs->rows.row[i].cells.cell[j].str;
            buffer[i][j] = dup_or_empty(cell);
            if (buffer[i][j] == NULL)
            {
                xls_close_WS(pWs);
                xls_close_WB(pWb);
                return NULL;
            }
            sanitize_cell(buffer[i][j]);
            if (buffer[i][j][0] == '\0')
            {
                xls_info.xls_null_str_num++;
            }
        }
    }

    xls_close_WS(pWs);
    xls_close_WB(pWb);

    xls_info.xls_is_exist = true;
    return buffer;
}

bool lang_xls_file_state_get(void)
{
    return xls_info.xls_is_exist;
}

int lang_xls_null_str_num_get(void)
{
    return xls_info.xls_null_str_num;
}

int lang_xls_language_num_get(void)
{
    return xls_info.col_total;
}

int lang_xls_str_num_get(void)
{
    return xls_info.row_total;
}

bool init_language_xls_info(void)
{
    if (lang_xls_init(0) == NULL)
    {
        printf("[language_xls] init failed\n");
        return false;
    }

    if (lang_xls_file_state_get() == false)
    {
        printf("[language_xls] file state invalid\n");
        return false;
    }

    if (lang_xls_language_num_get() < XLS_LANG_COL_TOTAL)
    {
        printf("[language_xls] column count %d < %d\n", lang_xls_language_num_get(), XLS_LANG_COL_TOTAL);
        return false;
    }

    language_xls_init_state = true;
    printf("[language_xls] init success\n");
    return true;
}

const char *lang_xls_str_get(int row, int col)
{
    if (buffer == NULL)
    {
        return "";
    }
    if (row < 0 || row >= xls_info.row_total)
    {
        return "";
    }
    if (col < 0 || col >= xls_info.col_total)
    {
        return "";
    }
    return buffer[row][col];
}

static const char *lang_xls_str_get_by_english(const char *english_fallback, int col)
{
    int row;

    if (buffer == NULL)
    {
        return "";
    }
    if (col < 0 || col >= xls_info.col_total)
    {
        return "";
    }

    for (row = 0; row < xls_info.row_total; row++)
    {
        if (!english_cell_matches(buffer[row][XLS_LANG_COL_ENGLISH], english_fallback))
        {
            continue;
        }
        return buffer[row][col];
    }

    return "";
}

const char *lang_xls_language_name_get(int col)
{
    static char cleaned[XLS_LANG_COL_TOTAL][64];
    static bool logged[XLS_LANG_COL_TOTAL] = {false};
    static const char *fallback[XLS_LANG_COL_TOTAL] = {
        "",
        "English",
        "العربية",
        "Magyar",
        "Maďarský",
        "Română",
        "Srpski",
        "Deutsch",
        "Polski",
        "Portuguese",
        "French",
        "中文",
    };

    if (col < 0 || col >= XLS_LANG_COL_TOTAL)
    {
        return "";
    }

    const char *raw = lang_xls_str_get_by_english("English", col);
    clean_language_name(raw, cleaned[col], sizeof(cleaned[col]));

    if (cleaned[col][0] == '\0' ||
        (col == XLS_LANG_COL_CHINESE && strcmp(cleaned[col], "英语") == 0))
    {
        if (!logged[col])
        {
            printf("[language_name] col=%d raw=%s cleaned=%s fallback=%s\n",
                   col,
                   raw == NULL ? "(null)" : raw,
                   cleaned[col],
                   fallback[col]);
            logged[col] = true;
        }
        return fallback[col];
    }

    if (!logged[col])
    {
        printf("[language_name] col=%d raw=%s cleaned=%s\n",
               col,
               raw == NULL ? "(null)" : raw,
               cleaned[col]);
        logged[col] = true;
    }
    return cleaned[col];
}

bool is_language_xls_inited(void)
{
    return language_xls_init_state;
}
