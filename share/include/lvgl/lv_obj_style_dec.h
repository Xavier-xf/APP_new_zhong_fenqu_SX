
/**
 * @file lv_obj_style_dec.h
 *
 */

#ifndef LV_OBJ_STYLE_DEC_H
#define LV_OBJ_STYLE_DEC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/**
 * Macro to declare the most important style set/get API functions.
 *
 * Get the value of a style property from an object in the object's current state
 * -----------------------------------------------------------------------------
 *  - Get the value of a style property from an object in the object's current state.
 *  - Transition animation is taken into account.
 *  - If the property is not set in the object's styles check the parent(s) if the property can be inherited
 *  - If still not found return a default value.
 *  - For example:
 *      `lv_style_int_t w = lv_obj_get_style_border_width(btn1, LV_BTN_PART_MAIN);`
 *
 * Set a local style property for an object in a given state
 * ---------------------------------------------------------
 *  - For example:
 *      `lv_obj_set_style_local_border_width(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, 2);`
 *
 * Get a local style property's value of an object in a given state
 * ----------------------------------------------------------------
 *  - Return the best matching property in the given state.
 *  - E.g. if `state` parameter is LV_STATE_PRESSED | LV_STATE_CHECKED` but the property defined only in
 *    `LV_STATE_PRESSED` and `LV_STATE_DEFAULT` the best matching state is `LV_STATE_PRESSED`
 *    (because it has higher precedence) and it will be returned.
 *  - If the property is not found even in `LV_STATE_DEFAULT` `-1` is returned.
 *  - For example:
 *      `//Type of result should be lv_style_int_t/lv_opa_t/lv_color_t/const void * according to the type of the property`
 *      `lv_style_int_t result;`
 *      `lv_obj_get_style_local_border_width(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, &result);`
 *      `if(weight > 0) ...the property is found and loaded into result...`
 *
 * Get the value from a style in a given state
 * -------------------------------------------
 * - The same rules applies to the return value then for "lv_obj_get_style_local_...()" above
 * - For example
 *      `int16_t weight = lv_style_get_border_width(&style1, LV_STATE_PRESSED, &result);`
 *      `if(weight > 0) ...the property is found and loaded into result...`

 * Set a value in a style in a given state
 * ---------------------------------------
 * - For example
 *      `lv_style_set_border_width(&style1, LV_STATE_PRESSED, 2);`
 */

#ifdef __cplusplus
#define _OBJ_GET_STYLE(prop_name, func_name, value_type, style_type)                                \
    static inline value_type lv_obj_get_style_##func_name(const lv_obj_t * obj, uint8_t part)       \
    {                                                                                               \
        return (value_type)_lv_obj_get_style##style_type(obj, part, LV_STYLE_##prop_name);          \
    }
#else
#define _OBJ_GET_STYLE(prop_name, func_name, value_type, style_type)                                \
    static inline value_type lv_obj_get_style_##func_name(const lv_obj_t * obj, uint8_t part)       \
    {                                                                                               \
        return _lv_obj_get_style##style_type(obj, part, LV_STYLE_##prop_name);                      \
    }
#endif
	

#define _OBJ_SET_STYLE_LOCAL(prop_name, func_name, value_type, style_type)                                                      \
    static inline void lv_obj_set_style_local_##func_name(lv_obj_t * obj, uint8_t part, lv_state_t state, value_type value)     \
    {                                                                                                                           \
        _lv_obj_set_style_local##style_type(obj, part, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);            \
    }

#define _OBJ_SET_STYLE(prop_name, func_name, value_type, style_type)                                                     \
    static inline void lv_style_set_##func_name(lv_style_t * style, lv_state_t state, value_type value)                  \
    {                                                                                                                    \
        _lv_style_set##style_type(style, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);                   \
    }

#define _LV_OBJ_STYLE_SET_GET_DECLARE(prop_name, func_name, value_type, style_type)                                      \
    _OBJ_GET_STYLE(prop_name, func_name, value_type, style_type)                                                         \
    _OBJ_SET_STYLE_LOCAL(prop_name, func_name, value_type, style_type)                                                   \
    _OBJ_SET_STYLE(prop_name, func_name, value_type, style_type)


/*混合属性*/
/*
*圆角
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(RADIUS, radius, lv_style_int_t, _int) 
/*
*将溢出的内容剪切到圆角上
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(CLIP_CORNER, clip_corner, bool, _int) 
/*
*小部件内部元素的大小
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SIZE, size, lv_style_int_t, _int)
/*
*使用此值使对象在两侧更宽
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_WIDTH, transform_width, lv_style_int_t, _int)  
/*
*使用此值使对象在两侧都较高
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_HEIGHT, transform_height, lv_style_int_t, _int)
/*
*旋转类似图像的对象，他的单位是0.1度，对于45度，使用450度。
*/ 
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_ANGLE, transform_angle, lv_style_int_t, _int) 
/*
* 缩放类似图像的对象
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_ZOOM, transform_zoom, lv_style_int_t, _int) 
/*
*继承，按此比例缩小对象的所有不透明度值，由于继承了子对象，因此也会受到影响
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OPA_SCALE, opa_scale, lv_opa_t, _opa) 

/*填充和边距属性*/
/*
* 填充：可在边缘的内侧设置空间，意思是“我不要我的孩子们里我的身体太近，所以要保留这个空间”。填充内部设置了孩子之间的“差距”。
* 边距：在边缘的外侧设置空间，意思是“我想要我周围的空间”。如果使用了布局，或自动调整，则这些属性通常由Container对象使用。但是，其他小部件也使用他们来设置间距。
*/

/*
*  在顶部设置填充
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_TOP, pad_top, lv_style_int_t, _int)
/*
*  在底部设置填充
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_BOTTOM, pad_bottom, lv_style_int_t, _int)
/*
* 在左侧设置填充
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_LEFT, pad_left, lv_style_int_t, _int)
/*
* 在右侧设置填充
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_RIGHT, pad_right, lv_style_int_t, _int)
/*
*  设置子对象之间对象内部的填充
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_INNER, pad_inner, lv_style_int_t, _int)

/*
* 在顶部设置边距
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_TOP, margin_top, lv_style_int_t, _int)
/*
* 在底部设置边距
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_BOTTOM, margin_bottom, lv_style_int_t, _int)
/*
*  在左边设置边距
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_LEFT, margin_left, lv_style_int_t, _int)
/*
*  在右侧设置边距
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_RIGHT, margin_right, lv_style_int_t, _int)

/*
*  背景属性
*/
/*
*  将混合模式设置为背景 ，可以是：LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE
* 默认值：LV_BLEND_MODE_NORMAL
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_BLEND_MODE, bg_blend_mode, lv_blend_mode_t, _int)

/*
*  指定渐变应从何处开始。0：最左/最上位置，255：最右/最下位置。默认值：0
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_MAIN_STOP, bg_main_stop, lv_style_int_t, _int)
/*
*  指定渐变应在何处停止。 0:最左/最上位置，255：最右/最下位置。默认值：255
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_STOP, bg_grad_stop, lv_style_int_t, _int)
/*
*  指定渐变的方向。可以是LV_GRAD_DIR_NONE/HOR/VER。默认值：LV_GRAD_DIR_NONE。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_DIR, bg_grad_dir, lv_grad_dir_t, _int)
/*
* 背景颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_COLOR, bg_color, lv_color_t, _color)
/*
*  指定背景渐变的颜色。右侧或底部的颜色是：bg_grad_dir != LV_GRAD_DIR_NONE。默认值LV_COLOR_WHITE
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_COLOR, bg_grad_color, lv_color_t, _color)
/*
*  背景透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_OPA, bg_opa, lv_opa_t, _opa)


/*边框属性*/

/*
*  边框的宽度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_WIDTH, border_width, lv_style_int_t, _int)
/*
*   指定要绘制边框的哪一侧。可以是：LV_BORDER_SIDE_NONE/LEFT/RIGHT/TOP/BOTTOM/FULL 。默认值LV_BORDER_SIDE_FULL 
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_SIDE, border_side, lv_border_side_t, _int)
/*
*  设置边框的混合模式。LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值： LV_BLEND_MODE_NORMAL
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_BLEND_MODE, border_blend_mode, lv_blend_mode_t, _int)
/*
*  如果为ture，在绘制所有子级之后绘制边框。默认值：false
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_POST, border_post, bool, _int)
/*
*   指定边框的颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_COLOR, border_color, lv_color_t, _color)
/*
*  指定边框的不透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_OPA, border_opa, lv_opa_t, _opa)


/*
*  轮廓属性。轮廓类似于边框，但绘制在对象外部。
*/


/*
*  指定轮廓的宽度 
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_WIDTH, outline_width, lv_style_int_t, _int)
/*
*  设置对象和轮廓之间的空间
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_PAD, outline_pad, lv_style_int_t, _int)
/*
*  设置轮廓的混合模式。可以是 LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值： LV_BLEND_MODE_NORMAL 。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_BLEND_MODE, outline_blend_mode, lv_blend_mode_t, _int)
/*
*  指定轮廓的颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_COLOR, outline_color, lv_color_t, _color)
/*
*  设定轮廓的不透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_OPA, outline_opa, lv_opa_t, _opa)

/*
*  阴影属性，阴影是对象下方的模糊区域
*/
/*
*  设置轮廓的宽度(模糊大小)。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_WIDTH, shadow_width, lv_style_int_t, _int)
/*
*  设置阴影的X偏移量
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OFS_X, shadow_ofs_x, lv_style_int_t, _int)
/*
*  设置阴影的Y偏移量
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OFS_Y, shadow_ofs_y, lv_style_int_t, _int)
/*
* 在每个方向上使阴影大于背景的值达到此值。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_SPREAD, shadow_spread, lv_style_int_t, _int)
/*
*设置阴影的混合模式。 LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值： LV_BLEND_MODE_NORMAL 。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_BLEND_MODE, shadow_blend_mode, lv_blend_mode_t, _int)
/*
*  设置阴影的颜色。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_COLOR, shadow_color, lv_color_t, _color)
/*
*设置阴影的不透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OPA, shadow_opa, lv_opa_t, _opa)




/*
*  图案属性。 图案是在背景中间绘制或者重复以填充整个背景的图像
*/

/*
*   图案在空间的对其方式
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_ALIGN, pattern_align, lv_align_t, _int)
/*
*   图案将作为马赛克重复。false：将团放置在背景中间。默认值：false
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_REPEAT, pattern_repeat, bool, _int)
/*
*设置图案的混合模式。 LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值： LV_BLEND_MODE_NORMAL 。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_BLEND_MODE, pattern_blend_mode, lv_blend_mode_t, _int)
/*
* 将此颜色混合到图案图像中。如果是文本，他将是文本颜色。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_RECOLOR, pattern_recolor, lv_color_t, _color)
/*
*   指定图案的不透明度。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_OPA, pattern_opa, lv_opa_t, _opa)
/*
*  重新着色的强度。默认值：LV_OPA_TRANSP(不重新着色)
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_RECOLOR_OPA, pattern_recolor_opa, lv_opa_t, _opa)
/*
*  图像文件的路径。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_IMAGE, pattern_image, const void *, _ptr)

/*
*   数值属性：值是绘制到背景的任意文本。它可以是创建标签对象的轻量级替代
*/

/*
*   文本的字母空间（暂时不明白意思）
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_LETTER_SPACE, value_letter_space, lv_style_int_t, _int)

/*
*   文本的行距。默认值是0。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_LINE_SPACE, value_line_space, lv_style_int_t, _int)
/*
*   设置文本的混合模式
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_BLEND_MODE, value_blend_mode, lv_blend_mode_t, _int)
/*
*   与路线原始位置的X偏移量
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OFS_X, value_ofs_x, lv_style_int_t, _int)
/*
*   与路线原始位置的Y偏移量
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OFS_Y, value_ofs_y, lv_style_int_t, _int)

/*
*   文本的对齐方式。默认是LV_ALIGN_CENTER
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_ALIGN, value_align, lv_align_t, _int)

/*
*   文本的颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_COLOR, value_color, lv_color_t, _color)

/*
*   文本的不透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OPA, value_opa, lv_opa_t, _opa)

/*
*   指向文本字体的指针。默认值“NULL”
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_FONT, value_font, const lv_font_t *, _ptr)

/*
*   指向要显示文本的指针。仅保存指针！
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_STR, value_str, const char *, _ptr)


/*
*   文本属性
*/
/*
*   文本的字母空间
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_LETTER_SPACE, text_letter_space, lv_style_int_t, _int)
/*
*   文本的行距
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_LINE_SPACE, text_line_space, lv_style_int_t, _int)
/*
*   添加文本修饰 可以是 LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH 。默认值：LV_TEXT_DECOR_NONE。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_DECOR, text_decor, lv_text_decor_t, _int)
/*
*设置文本的混合模式。 LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值： LV_BLEND_MODE_NORMAL 。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_BLEND_MODE, text_blend_mode, lv_blend_mode_t, _int)

/*
*   设置文本的颜色
*/  
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_COLOR, text_color, lv_color_t, _color)
/*
*   设置文本选择的字体颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_SEL_COLOR, text_sel_color, lv_color_t, _color)
/*
*   设置文本选择的背景颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_SEL_BG_COLOR, text_sel_bg_color, lv_color_t, _color)
/*
*   设置文本透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_OPA, text_opa, lv_opa_t, _opa)
/*
*   设置文本字体的指针
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_FONT, text_font, const lv_font_t *, _ptr)


/*
*   线属性
*/
/*
*   线的宽度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_WIDTH, line_width, lv_style_int_t, _int)
/*
*设置线条的混合模式。可以是 LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE。默认值：LV_BLEND_MODE_NORMAL。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_BLEND_MODE, line_blend_mode, lv_blend_mode_t, _int)

/*
*   破折号的宽度。仅对水平或者垂直线绘制虚线。0：禁用破折号。默认值：0
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_DASH_WIDTH, line_dash_width, lv_style_int_t, _int)

/*
*   两条虚线之间的间隙，仅对水平或者垂直线绘制虚线。0：禁用破折号。默认值：0
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_DASH_GAP, line_dash_gap, lv_style_int_t, _int)

/*
*      true绘制圆角的线尾，默认值:false
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_ROUNDED, line_rounded, bool, _int)

/*
* 设置线条的颜色
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_COLOR, line_color, lv_color_t, _color)
/*
*   设置线条的不透明度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_OPA, line_opa, lv_opa_t, _opa)


/*
* 图片属性
*/
/*
*   设置图像的混合模式
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_BLEND_MODE, image_blend_mode, lv_blend_mode_t, _int)

/*
*   将此颜色混合到图案中去。如果是符号（文本），他将是文本的颜色。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_RECOLOR, image_recolor, lv_color_t, _color)
/*
*   图像的不透明度。
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_OPA, image_opa, lv_opa_t, _opa)
/*
*   重新着色的强度。默认是（LV_OPA_TRANSP 不重新着色）
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_RECOLOR_OPA, image_recolor_opa, lv_opa_t, _opa)



/*
*   转换属性
*/

/*用于描述状态更改动画的属性*/
/*
*   过渡时间
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_TIME, transition_time, lv_style_int_t, _int)
/*
*   转换前的延迟
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_DELAY, transition_delay, lv_style_int_t, _int)

/*
*   应用其上应用过渡的属性
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_1, transition_prop_1, lv_style_int_t, _int)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_2, transition_prop_2, lv_style_int_t, _int)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_3, transition_prop_3, lv_style_int_t, _int)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_4, transition_prop_4, lv_style_int_t, _int)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_5, transition_prop_5, lv_style_int_t, _int)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_6, transition_prop_6, lv_style_int_t, _int)
#if LV_USE_ANIMATION
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PATH, transition_path, const lv_anim_path_t *, _ptr)
#else
/*For compatibility*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PATH, transition_path, const void *, _ptr)
#endif

/*
*   比例刻度属性
*/

/*
*   比例尺的宽度。默认值LV_DPI/8
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_WIDTH, scale_width, lv_style_int_t, _int)

/*
*   在标准区域的比例尺外侧绘制的边框宽度。默认值：0
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_BORDER_WIDTH, scale_border_width, lv_style_int_t, _int)

/*
*   在结束区域的刻度外侧上绘制边框的宽度。默认值：0
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_BORDER_WIDTH, scale_end_border_width, lv_style_int_t, _int)

/*
*   结束区域中比例线的宽度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_LINE_WIDTH, scale_end_line_width, lv_style_int_t, _int)

/*
*   在正常区域中，在比例尺线上对该颜色进行渐变
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_GRAD_COLOR, scale_grad_color, lv_color_t, _color)

/*
*   在结束区域中比例线的宽度
*/
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_COLOR, scale_end_color, lv_color_t, _color)

#undef _LV_OBJ_STYLE_SET_GET_DECLARE
#undef _OBJ_SET_STYLE_LOCAL
#undef _OBJ_SET_STYLE
#undef _OBJ_GET_STYLE

static inline void lv_obj_set_style_local_pad_all(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_top(obj, part, state, value);
    lv_obj_set_style_local_pad_bottom(obj, part, state, value);
    lv_obj_set_style_local_pad_left(obj, part, state, value);
    lv_obj_set_style_local_pad_right(obj, part, state, value);
}

static inline void lv_style_set_pad_all(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_top(style, state, value);
    lv_style_set_pad_bottom(style, state, value);
    lv_style_set_pad_left(style, state, value);
    lv_style_set_pad_right(style, state, value);
}

static inline void lv_obj_set_style_local_pad_hor(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_left(obj, part, state, value);
    lv_obj_set_style_local_pad_right(obj, part, state, value);
}

static inline void lv_style_set_pad_hor(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_left(style, state, value);
    lv_style_set_pad_right(style, state, value);
}

static inline void lv_obj_set_style_local_pad_ver(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_top(obj, part, state, value);
    lv_obj_set_style_local_pad_bottom(obj, part, state, value);
}

static inline void lv_style_set_pad_ver(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_top(style, state, value);
    lv_style_set_pad_bottom(style, state, value);
}

static inline void lv_obj_set_style_local_margin_all(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_top(obj, part, state, value);
    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
    lv_obj_set_style_local_margin_left(obj, part, state, value);
    lv_obj_set_style_local_margin_right(obj, part, state, value);
}

static inline void lv_style_set_margin_all(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_top(style, state, value);
    lv_style_set_margin_bottom(style, state, value);
    lv_style_set_margin_left(style, state, value);
    lv_style_set_margin_right(style, state, value);
}

static inline void lv_obj_set_style_local_margin_hor(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_left(obj, part, state, value);
    lv_obj_set_style_local_margin_right(obj, part, state, value);
}

static inline void lv_style_set_margin_hor(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_left(style, state, value);
    lv_style_set_margin_right(style, state, value);
}

static inline void lv_obj_set_style_local_margin_ver(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_top(obj, part, state, value);
    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
}

static inline void lv_style_set_margin_ver(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_top(style, state, value);
    lv_style_set_margin_bottom(style, state, value);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_STYLE_DEC_H*/
