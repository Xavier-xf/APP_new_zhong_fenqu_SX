#include "language.h"
#include <stdlib.h>

/***
**   日期:2022-07-07 10:36:31
**   作者: leo.liu
**   函数作用：其他没有显示的字体
**   参数说明:
***/
/*****
 *  年/년
 *  "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "\n",
						   "-","=","\\","[","]",";","'",",",".","/","\n",
						   "_", "+","|","{","}",":","\"","<" ,">","?","\n",
						  "`","~"," "," "," "," "," ","\n",
						  "abc..."," ","\0"
 *
 *
 *  *****/

static LANGUAGE_ID language_cur_id = LANGUAGE_ID_ENGLISH;

/***
** 日期: 2022-04-26 08:55
** 作者: leo.liu
** 函数作用：设置当前语言
** 返回参数说明：
***/
void language_id_set(LANGUAGE_ID id)
{
	language_cur_id = id;
}

/***
** 日期: 2022-04-26 08:56
** 作者: leo.liu
** 函数作用：获取当前语言ID
** 返回参数说明：
***/
LANGUAGE_ID language_id_get(void)
{
	return language_cur_id;
}

/***** 公共字符串资源 *****/
static const char *lang_common_language[LANG_COMMON_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** LANG_COMMON_ID_OFF *****/
	{"OFF", "إيقاف"},
	/***** LANG_COMMON_ID_ON *****/
	{"ON", "تشغيل"},
	/***** LANG_COMMON_ID_VIDEO *****/
	{"Video","فيديو"},
	/***** LANG_COMMON_ID_PHOTO *****/
	{"Photo", "صورة"},
	/***** LANG_COMMON_ID_CANCEL *****/
	{"Cancel", "الغاء"},
	/***** LANG_COMMON_ID_CONFIRM *****/
	{"Confirm?", "تأكيد?"},
	/***** LANG_COMMON_ID_DOOR1 *****/
	{"Door1", "الباب  1"},
	/*****  LANG_COMMON_ID_DOOR2*****/
	{"Door2",  "الباب  2"},
	/***** LANG_COMMON_ID_CCTV1 *****/
	{"CCTV1", "كاميرا مرا 1"},
	/***** LANG_COMMON_ID_CCTV2 *****/
	{"CCTV2","كاميرات مراقبة 2"},
	/***** LANG_COMMON_ID_CCTV3 *****/
	{"CCTV3", "كاميرا مرا 3"},
	/***** LANG_COMMON_ID_CCTV4 *****/
	{"CCTV4","كاميرات مراقبة 4"},

	{"TUYA Playback cache","ذاكرة التخزين المؤقت"},
};
/***
** 日期: 2022-04-28 13:55
** 作者: leo.liu
** 函数作用：获取公共字符串的字符串资源
** 返回参数说明：
***/
const char *language_common_string_get(LANG_COMMON_ID id)
{
	return lang_common_language[id][language_cur_id];
}
/***
**   日期:2022-05-24 10:04:38
**   作者: leo.liu
**   函数作用：根据通道类型获取字符串
**   参数说明:
***/
const char *language_common_ch_string_get(MON_CH ch)
{
	if (ch == MON_CH_DOOR1)
	{
		return language_common_string_get(LANG_COMMON_ID_DOOR1);
	}
	else if (ch == MON_CH_DOOR2)
	{
		return language_common_string_get(LANG_COMMON_ID_DOOR2);
	}
	else if (ch == MON_CH_CCTV1)
	{
		return language_common_string_get(LANG_COMMON_ID_CCTV1);
	}
	else if (ch == MON_CH_CCTV2)
	{
		return language_common_string_get(LANG_COMMON_ID_CCTV2);
	}
	else if (ch == MON_CH_CCTV3)
	{
		return language_common_string_get(LANG_COMMON_ID_CCTV3);
	}
	else
	{
		return language_common_string_get(LANG_COMMON_ID_CCTV4);
	}
}
static const char *layout_home_language[HOME_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/*LANGUAGE_ID_ENGLISH，LANGUAGE_ID_HANYU,LANGUAGE_ID_ELUOSI,LANGUAGE_ID_XIBANYA,LANGUAGE_ID_CHINESE,LANGUAGE_ID_TUERQI,LANGUAGE_ID_BOLAN,LANGUAGE_ID_JIEKE,LANGUAGE_ID_ALABOYU,*/
	/* HOME_LANG_ID_MONITOR */
	{"Monitor","شاشة"},
	/* HOME_LANG_ID_CCTV */
	{"CCTV","كاميرا مراقبه",},
	/* HOME_LNAG_ID_INTER */
	{"Intercom","تلفون داخلي"},
	/* HOME_LANG_ID_PLAYBACK */
	{"Media","اعاده تشغيل"},
	/* HOME_LANG_ID_SECURITY */
	{"Security","حماية"},

	/* HOME_LANG_ID_WEEK_1 */
	{"Mon","الإثنين"},
	/* HOME_LANG_ID_WEEK_2 */
	{"Tue","الثلاثاء"},
	/* HOME_LANG_ID_WEEK_3 */
	{"Wed","الاربعاء"},
	/* HOME_LANG_ID_WEEK_4 */
	{"Thu","الخميس"},
	/* HOME_LANG_ID_WEEK_5 */
	{"Fri","الجمعة"},
	/* HOME_LANG_ID_WEEK_6 */
	{"Sat","السبت"},
	/* HOME_LANG_ID_WEEK_7 */
	{"Sun","الأحد"},

	/*HOME_LANG_ID_MONTH_1 */
	{"January","يناير"},
	/*HOME_LANG_ID_MONTH_2 */
	{"February","فبراير"},
	/*HOME_LANG_ID_MONTH_3 */
	{"March","مارس"},
	/*HOME_LANG_ID_MONTH_4 */
	{"April","أبريل"},
	/*HOME_LANG_ID_MONTH_5 */
	{"May","مايو"},
	/*HOME_LANG_ID_MONTH_6 */
	{"June", "يونيو"},
	/*HOME_LANG_ID_MONTH_7 */
	{"July", "يوليو"},
	/*HOME_LANG_ID_MONTH_8 */
	{"August","أغسطس"},
	/*HOME_LANG_ID_MONTH_9 */
	{"September","سبتمبر"},
	/*HOME_LANG_ID_MONTH_10 */
	{"October","اكتوبر"},
	/*HOME_LANG_ID_MONTH_11 */
	{"November","نوفمبر"},
	/*HOME_LANG_ID_MONTH_12 */
	{"December", "ديسمبر"},
	/*****  HOME_LANG_ID_TUYA_PREVIEW *****/
	{"in use in the mobile app", "قيد الاستخدام في تطبيق الهاتف المحمول"},
	/*****  HOME_LANG_ID_TUYA_TALK *****/
	{"in use in the mobile app","قيد الاستخدام في تطبيق الهاتف المحمول"},
	{"At home", "فى المنزل"},
	{"Leave home", "غادر المنزل"},
	{"Sleep mode", "بيات شتوى"},
	{"No CCTV camera connected","لا توجد كاميرا متصلة"},
	{"No outdoor unit connected","لا يوجد توصيل آلة الباب"},
};

/***
** 日期: 2022-04-26 09:16
** 作者: leo.liu
** 函数作用：获取home页面的字符串资源
** 返回参数说明：年/년 !@
***/
const char *layout_home_string_get(LAYOUT_HOME_LANG_ID id)
{
	return layout_home_language[id][language_cur_id];
}

static const char *layout_setting_record_language[SETTING_RECROD_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_RECORD_LANG_ID_SETTING *****/
	{"Configuration", "إعدادات"},
	/***** SETTING_RECORD_LANG_ID_RECORD_WIFI *****/
	{"Recording","واي فاي/تسجيل"},
	/***** SETTING_RECORD_LANG_ID_VOLUME *****/
	{"Volume", "الصوت"},
	/***** SETTING_RECORD_LANG_ID_STORAGE *****/
	{"Storage", "تخزين"},
	/***** SETTING_RECORD_LANG_ID_DATE *****/
	{"Time", "الوقت"},
	/***** SETTING_RECORD_LANG_ID_DISPLAY *****/
	{"Display","عرض"},
	/***** SETTING_RECORD_LANG_ID_ETC *****/
	{"Others", "إلخ"},

	/***** SETTING_RECORD_LANG_ID_AUTO_RECORD  *****/
	{"Auto recording", "التسجيل التلقائي"},
	/***** SETTING_RECORD_LANG_ID_MOTION_DETECTION  *****/
	{"Motion detection","كشف الحركة"},
	/***** SETTING_RECORD_LANG_ID_WIFI *****/
	{"Wi-Fi","واي فاي"},
	/***** SETTING_RECORD_LANG_ID_ALWAYS *****/ 
	{"Loop Monitoring","رصد الحلقات"},

	/***** SETTING_RECORD_LANG_ID_ALWAYS_10SEC *****/
	{"10 S","10 ثواني",},
	/***** SETTING_RECORD_LANG_ID_ALWAYS_30SEC *****/
	{"30 S","30 ثواني"},
	/***** SETTING_RECORD_LANG_ID_ALWAYS_60SEC *****/
	{"60 S","60 ثواني"},

	{"Door1 always on","door1 دائما"},
	{"Door2 always on","door2 دائما"},
	{"Door 1 ring","حلقة الباب 1"},
	{"Door 2 ring","حلقة الباب 2"},
	{"intercom","اسلكية تخاطب"},
	{"Serious","خطيرة"},
	{"Moderate","خفيف"},
	{"Mild","العامة"},
	{"8K","8K"},

	{"List of display","إعدادات"},
	{"Screen Saver time","إعدادات"},
	{"Open Frame Time", "إعدادات"},
	{"Configuration","إعدادات"},
};

/***
** 日期: 2022-04-28 10:48
** 作者: leo.liu
** 函数作用：获取setting recrod页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_record_string_get(LAYOUT_SETTING_RECORD_LANG_ID id)
{
	return layout_setting_record_language[id][language_cur_id];
} 


/***
** 日期: 2022-04-28 10:48
** 作者: leo.liu
** 函数作用：获取home weather页面的字符串资源
** 返回参数说明：
***/

static const char *layout_home_weather_language[HOME_WEATHER_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
{	
	{"Heavy rain","مطر غزير"},

	{"Thunderstorm","عاصفة رعدية"},

	{"Sandstorm","عاصفة رملية"},

	{"Light snow","الثلج"},

	{"Snow","ثلج"},

	{"Pogonip","الضباب الجامد"},

	{"Rainstorm","عاصفة ممطرة"},

	{"Isolated showers","الاستحمام الجزئي"},

	{"Floating dust","تراب"},

	{"Lightning","صاعقة"},

	{"Light showers","امطار خفيفة"},

	{"Rain","مطر"},

	{"Rain and snow","البرد"},

	{"Dust tornado","اعصار الغبار"},

	{"Ice pellet","حبيبات الجليد"},

	{"Strong sandstorm","حبيبات الجليد"},

	{"Blowing sand","جانسا"},

	{"Light to moderate rain","خفيف إلى معتدل المطر"},

	{"Mostly clear","في الغالب مشمس"},

	{"Sunny","صافي"},

	{"Fog","ضباب"},

	{"Shower","دش"},

	{"Heavy showers","أمطار غزيرة"},

	{"Heavy snow","ثلوج كثيفة"},

	{"Torrential rain","مطر غزير"},

	{"Blizzard","عاصفة ثلجية"},

	{"Hail","وابل"},

	{"Light to moderate snow","ثلوج خفيفة إلى معتدلة"},

	{"Partly cloudy","أقل غائمة"},

	{"Light snow showers","زخات ثلج صغيرة"},

	{"Moderate snow","ثلوج متوسطة"},

	{"Overcast","سلبي"},

	{"Hoarfrost","إبرة الجليد"},

	{"Rainstorm","مطر غزير"},

	{""," "},

	{"Thunderstorms with hail","عواصف رعدية مصحوبة ببرد"},

	{"Freezing rain","مطر مجمد"},

	{"Snow showers","زخات الثلوج"},

	{"Light rain","مطر خفيف"},

	{"Smog","ضباب"},

	{"Moderate rain","أمطار معتدلة"},

	{"Cloudy","غائم جزئيا"},

	{"Thunderstorm","عاصفة رعدية"},

	{"Moderate to heavy rain","أمطار معتدلة إلى غزيرة"},

	{"Rainstorm","مطر غزير"},

	{"sunny","مشمس"},
	
	
};

const char *layout_home_weather_string_get(LAYOUT_HOME_WEATHER_LANG_ID id)
{
	return layout_home_weather_language[id][language_cur_id];
}

static const char *layout_home_weather_milieu_language[HOME_WEATHER_LABEL_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
{
	{"humidity:","رطوبة:"},
	{"air pressure:","ضغط جوي:"},
	{"pm10:","مساءا 10:"},
	{"pm2.5:","مساء 25.:"},
	{"%","%"}
};

const char *layout_home_weather_milieu_string_get(LAYOUT_HOME_WEATHER_MILIEU_LANG_ID id)
{
	return layout_home_weather_milieu_language[id][language_cur_id];
}


static const char *layout_setting_volume_language[SETTING_VOLUME_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_VOLUME_LANG_ID_TOUCH_TONE *****/
	{"Touch tone",  "نغمة اللمس"},
	/***** SETTING_VOLUME_LANG_ID_DOOR1_TONE *****/
	{"Door1 calling tone","نغمة اتصال باب 1"},
	/***** SETTING_VOLUME_LANG_ID_DOOR2_TONE *****/
	{"Door2 calling tone","نغمة اتصال باب 2"},
	{"Door calling tone", "نغمة اتصال باب "},
	/*****  SETTING_VOLUME_LANG_ID_INTERPHONE_TONE *****/
	{"Intercom Ring tone", "نغمة اتصال تلفون داخلي"},
	/***** SETTING_VOLUME_LANG_ID_TONE1 *****/
	{"Sound1",  "1 الصوت"},
	{"Sound2",  "2 الصوت"},
	{"Sound3",  "3 الصوت"},
	{"Sound4", "4 الصوت"},
	{"Sound5", "5 الصوت"},
	{"Sound6", "6 الصوت"},
};
/***
** 日期: 2022-04-29 08:14
** 作者: leo.liu
** 函数作用：获取setting volume 页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_volume_string_get(LAYOUT_SETTING_VOLUME_LANG_ID id)
{
	return layout_setting_volume_language[id][language_cur_id];
}



static const char *layout_setting_ring_language[SETTING_RING_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
{
	{"Schedule 1","نغمة الرنين 1"},
	{"Schedule 2","نغمة الرنين 2"},
	{"Schedule 3","نغمة الرنين 3"},
	{"Schedule","جدول"},
	{"Ring time","وقت الرنين"},
	{"Ring mode","وضع الحلقة"},
	{"Standard","معيار"},
	{"Customization","مخصصه"},
	{ "Ring select", "اختيار صوت الجرس"},
	{"Ring volume","نغمات الرنين"},
	{ "No SD card","لا توجد بطاقة SD"},
	{"SD card inserted""إدخال بطاقة SD"},
	{"CANCEL","إلغاء الأمر"},
	{"OK","موافق"},
	{"Error","خطأ"},
	
};
/***
** 日期: 2022-04-29 08:14
** 作者: leo.liu
** 函数作用：获取setting volume 页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_ring_string_get(LAYOUT_SETTING_RING_LANG_ID id)
{
	return layout_setting_ring_language[id][language_cur_id];
}



static const char *layout_setting_storage_language[SETTING_STORAGE_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_STORAGE_LANG_ID_FORMAT_SD *****/
	{"Format SD card", "تهيئة بطاقة SD"},
	/***** SETTING_STORAGE_LANG_ID_DELETION_FILE *****/
	{"Files deletion","اختيار ملف الحذف"},
	/***** SETTING_STORAGE_LANG_ID_MSG_FORMAT_SD *****/
	{"Formatting…","جارٍ تهيئة بطاقة SD ..."},
	/***** SETTING_STORAGE_LANG_ID_MSG_DEL_FILE *****/
	{"Deleting…","حذف الملف"},

	{"SD Card capacity","بطاقة بطاقة SD"},
	{"Flash capacity","سعة الذاكرة الداخلية"},
	{"Back up local photos to SD card","نسخ الصور المحلية إلى بطاقة SD"},
};
/***
** 日期: 2022-04-29 15:33
** 作者: leo.liu
** 函数作用：获取setting storage 页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_storage_string_get(LAYOUT_SETTING_STORAGE_LANG_ID id)
{
	return layout_setting_storage_language[id][language_cur_id];
}

static const char *layout_setting_systime_language[SETTING_DATE_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_DATE_LANG_ID_SYS_TIME *****/
	{"System Time","وقت النظام"},
};

/***
** 日期: 2022-05-04 09:52
** 作者: leo.lius
** 函数作用：获取setting date/time 页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_systime_string_get(LAYOUT_SETTING_DATE_LANG_id id)
{
	return layout_setting_systime_language[id][language_cur_id];
}

static const char *layout_setting_display_language[SETTING_DISPLAY_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_DISPLAY_LANG_ID_SCREEN_ADJ *****/
	{"Screen Adjustment","ضبط الشاشة"},
	/***** SETTING_DISPLAY_LANG_ID_STANDBY_MODE *****/
	{"Standby setting","أدخل وضع الاستعداد"},
	/***** SETTING_DISPLAY_LANG_ID_DISPLAY_CLEAN *****/
	{"Display cleaning mode","عرض وضع التنظيف"},
	/***** SETTING_DISPLAY_LANG_ID_DISPLAY_BRIGHTNESS *****/
	{"Brightness","عرض"},

	/***** SETTING_DISPLAY_LANG_ID_LCD_OFF *****/
	{"LCD OFF(After 30sec)","إيقاف تشغيل شاشة LCD(بعد 30  ثانية)"},
	/***** SETTING_DISPLAY_LANG_ID_OPEN_FRAME *****/
	{"Screen Saver","حافظة الشاشة"},
	/***** SETTING_DISPLAY_LANG_ID_FINISH_TIME *****/
	{"Clean Time","وقت التنظيف"},
};

/***
** 日期: 2022-05-04 10:00
** 作者: leo.liu
** 函数作用：获取setting disply 页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_display_string_get(LAYOUT_SETTING_DISPLAY_LANG_ID id)
{
	return layout_setting_display_language[id][language_cur_id];
}

static const char *layout_setting_etc_language[SETTING_ETC_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_ETC_LANG_ID_LANGUAGE *****/
	{"Language","اللغة"},
	/***** SETTING_ETC_LANG_ID_OPEN_THE_DOOR *****/
	{"Opening the door", "يفتح الباب"},
	/***** SETTING_ETC_LANG_ID_SET_ID *****/
	{"Extension ID setting", "تحديد الهوية"},
	/***** SETTING_ETC_LANG_ID_DOOR1_OPEN_MODULE *****/
	{"Door1 opening module","وحدة فتح الباب  1"},
	/***** SETTING_ETC_LANG_ID_SECURITY_PASSWORD *****/
	{"Modify Password","تعديل كلمة المرور"},
	/***** SETTING_ETC_LANG_ID_SYSTEM_RESET *****/
	{"Factory Default","المصنع الافتراضي"},
	{"Weather forecast","النشرة الجوية"},
	{"Security setting","إعداد الأمان"},
	{"CCTV manage","إدارة الدوائر التلفزيونية المغلقة"},
	/***** SETTING_ETC_LANG_ID_VERSION *****/
	{"Version","الإصدار"},
	/***** SETTING_ETC_LANG_ID_NUM_DOOR2_OPEN *****/
	{"The number of door2 open buttons","عدد أزرار فتح الباب  2"},

	/***** SETTING_ETC_LANG_ID_LANGUAGE_SUB *****/
	{"English", "بالعربية"},
	/***** SETTING_ETC_LANG_ID_FAST_MODE *****/
	{"Fast mode","الوضع السريع"},
	/***** SETTING_ETC_LANG_ID_STANDBY_MODE *****/
	{"Standard mode","الوضع القياسي"},
	/***** SETTING_ETC_LANG_ID_DOOR_CAMERA *****/
	{"Door camera", "كاميرا الباب"},
	/***** SETTING_ETC_LANG_ID_DIGITAL_DOOR_LOCK *****/
	{"Digital door lock","قفل الباب رقمي"},
	/***** SETTING_ETC_LANG_ID_DEVICE_ID1 *****/
	{"Room 1","بطاقة تعريف  1"},
	/***** SETTING_ETC_LANG_ID_DEVICE_ID2 *****/
	{"Room 2","بطاقة تعريف  2"},
	/***** SETTING_ETC_LANG_ID_DEVICE_ID3 *****/
	{"Room 3", "بطاقة تعريف  3"},
	/***** SETTING_ETC_LANG_ID_DEVICE_ID4 *****/
	{"Room 4", "بطاقة تعريف  4"},

	/***** SETTING_ETC_LANG_ID_SYSTIME_RESET_SUB *****/
	{"Are you sure you want to restore the factory \nsettings? If you want to remove the phone \nbinding at the same time, please ensure \nthat IoT is connected.","هل تريد معالجة التهيئة؟"},

	/***** SETTING_ETC_LANG_ID_DOOR2_NUM_1 *****/
	{
	    "1",
	    "1"
	},
	/***** SETTING_ETC_LANG_ID_DOOR2_NUM_2 *****/
	{
	    "2",
	    "2"
	},
	{"Gate unlock time","وقت فتح البوابة"},

		/***** DOOR OPEN TIME *****/
	{"Door unlock time","وقت فتح الباب"},
	{"Delay Time","وقت التأخير"},
	{"UUID","UUID"},
	{"Phone Call Push Delay","تأخر تأخير في إرسال الهواتف الخلوية"},

	/***** SETTING_ETC_LANG_ID_ADDED_DEV *****/
	{"Added Devices","الأجهزة المضافة"},
	/***** SETTING_ETC_LANG_ID_DISCOVERED_DEV *****/
	{"Discovered Devices","الأجهزة المكتشفة"},
	/***** SETTING_ETC_LANG_ID_SEARCH_DEV *****/
	{"Search Devices","أجهزة البحث"},
	/***** SETTING_ETC_LANG_ID_REGISTER_DEV *****/
	{"Register","السجل" },
	/***** SETTING_ETC_LANG_ID_BRAND *****/
	{"Brand","العلامة التجارية" },
	/***** SETTING_ETC_LANG_ID_IP_ADDRESS *****/
	{"IP Address","عنوان IP" },
	/***** SETTING_ETC_LANG_ID_ACCOUNT *****/
	{"Account","الحساب" },
	/***** SETTING_ETC_LANG_ID_PASSWORD *****/
	{"Password","كلمة السر" },
	/***** SETTING_ETC_LANG_ID_MAIN_STREAM *****/
	{"Main code stream","تدفق الكود الرئيسي" },
	/***** SETTING_ETC_LANG_ID_SUB_STREAM *****/
	{"Sub code stream","تدفق الكود الفرعي" },
	/***** SETTING_ETC_LANG_ID_SUB_STREAM *****/
	{"Delete this device ?","أمحي هذا الجهاز؟" },

	{"Panasonic","Panasonic" },
	/***** SETTING_ETC_LANG_ID_BRAND_DAHUA *****/
	{"Dahua","Dahua" },
	/***** SETTING_ETC_LANG_ID_BRAND_HIKVISION *****/
	{"Hikvision","Hikvision" },
	/***** SETTING_ETC_LANG_ID_BRAND_XM *****/
	{"Xiongmai","Xiongmai" },
	/***** SETTING_ETC_LANG_ID_BRAND_RTSP *****/
	{"RTSP","RTSP" },


	/***** SETTING_ETC_LANG_ID_INPUT_IP_ADDR *****/
	{"Input IP Address","أدخل عنوان" },
	/***** SETTING_ETC_LANG_ID_INPUT_ACCOUNT *****/
	{"Input Account","حساب الإدخال" },
	/***** SETTING_ETC_LANG_ID_INPUT_PASSWORD *****/
	{"Input Password","إدخال كلمة السر" },
	/***** SETTING_ETC_LANG_ID_INPUT_RTSP *****/
	{"Input RTSP","أدخل RTSP" },
	{"TP test","فحص TP" },

};
/***
** 日期: 2022-05-04 13:59
** 作者: leo.liu
** 函数作用：获取setting etc  页面的字符串资源
** 返回参数说明：
***/
const char *layout_setting_etc_string_get(LAYOUT_SETTING_ETC_LANG_ID id)
{
	return layout_setting_etc_language[id][language_cur_id];
}

/***
** 日期: 2022-05-07 10:20
** 作者: leo.liu
** 函数作用：获取指定的语言
** 返回参数说明：
***/
const char *layout_setting_etc_string_get_form_language(LAYOUT_SETTING_ETC_LANG_ID string_id, LANGUAGE_ID language_id)
{
	return layout_setting_etc_language[string_id][language_id];
}

static const char *layout_setting_motion_language[SETTING_MOTION_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/*****     SETTING_MOTION_LANG_ID_USE_MOTION_DETECT, *****/
	{"Enable motion detection","استخدام كاشف الحركة"},
	/*****         SETTING_MOTION_LANG_ID_SELECT_CAMERA, *****/
	{"Camera selection","اختيار الكاميرات"},
	/*****        SETTING_MOTION_LANG_ID_SAVING_FORMAT, *****/
	{"Recording format","حفظ التعديل"},
	/*****        SETTING_MOTION_LANG_ID_MOTION_HIGHT, *****/
	{"High", "عالي"},
	/*****        SETTING_MOTION_LANG_ID_MOTION_MIDDLE, *****/
	{"Middle", "متوسط"},
	/*****         SETTING_MOTION_LANG_ID_MOTION_LOW, *****/
	{"Low", "منخفض"},
	/*****        SETTING_MOTION_LANG_ID_SENSITIVITY, *****/
	{"Sensitivity setting", "تحديد الحساسية"},
	/*****        SETTING_MOTION_LANG_ID_MOTION_TIMER, *****/
	{"Motion timer", "مؤقت الحركة"},
	/*****        SETTING_MOTION_LANG_ID_LCD_IS_ON, *****/
	{"Screen ON when motion detected","شاشة LCD قيد التشغيل لاكتشاف الحركة"},
	/***** SETTING_MOTION_LANG_ID_USE_TIMER *****/
	{"Using motion timers","استخدام مؤقتات الحركة"}};
/***
** 日期: 2022-05-05 11:40
** 作者: leo.liu
** 函数作用：移动侦测设置资源获取
** 返回参数说明：
***/
const char *layout_setting_motion_string_get(LAYOUT_SETTING_MOTION_LANG_ID id)
{
	return layout_setting_motion_language[id][language_cur_id];
}

static const char *layout_setting_frame_language[SETTING_FRAME_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_FRAME_LANG_ID_OPEN_FRAME_TIME *****/
	{"Screen Saver time",  "افتح الإطار الزمني"},
	/***** SETTING_FRAME_LANG_ID_NIGHT_MODE *****/
	{"Night mode","الوضع الليلي"},
	/***** SETTING_FRAME_LANG_ID_LIST_OF_DISPLAY *****/
	{"List of display","قائمة العرض"},
	/***** SETTING_FRAME_LANG_ID_IMAGE_OF_BACKGROUND *****/
	{"Image of background","صورة الخلفية"},
	/***** SETTING_FRAME_LANG_ID_ALWAYS *****/
	{"Always", "دائماً"},
	/***** SETTING_FRAME_LANG_ID_NO_BACKGROUND_IMAGE *****/
	{"No Back ground","ليست هناك صورة خلفية"},
	/***** SETTING_FRAME_LANG_ID_BASIC_IMAGE *****/
	{"Basic image","الصورة الرئيسية"},
	/***** SETTING_FRAME_LANG_ID_NOT_AVAILABLE *****/
	{"Not available in motion detection mode", "غير متوفر في وضع اكتشاف الحركة"},
	/***** SETTING_FRAME_LANG_ID_TIME_SET *****/
	{"Time setting","ضبط الوقت"},
	/***** SETTING_FRAME_LANG_ID_START *****/
	{"Start", "بدأ"},
	/***** SETTING_FRAME_LANG_ID_END *****/
	{"End","انهاء"},
	/***** SETTING_FRAME_LANG_ID_TIME *****/
	{"Time", "الوقت"},
	/***** SETTING_FRAME_LANG_ID_CALENDAR *****/
	{"Calendar","تقويم"},
	/***** SETTING_FRAME_LANG_ID_PLAYBACK *****/
	{"Photos in media", "الصور الفوتوغرافية في الوسائط"},
	/***** SETTING_FRAME_LANG_ID_DOOR1_CCTV1 *****/
	{"Door1 & CCTV 1","باب 1  &كاميرا مراقبه  1"},
	/***** SETTING_FRAME_LANG_ID_DOOR2_CCTV2 *****/
	{"Door2 & CCTV 2", "باب  2  &كاميرا مراقبه  2"},
	{"SD Card Image","صورة بطاقة SD"},
};
/***
** 日期: 2022-05-06 15:26
** 作者: leo.liu
** 函数作用：待机设置资源获取
** 返回参数说明：
***/
const char *layout_setting_frame_string_get(LAYOUT_SETTING_FRAME_LANG_ID id)
{
	return layout_setting_frame_language[id][language_cur_id];
}

static const char *layout_setting_password_language[SETTING_PASSWORD_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** SETTING_PASSWORD_LANG_ID_PWD_NOT_MATCH *****/
	{"Incorrect password","كلمة المرور غير متطابقة"},
	/***** SETTING_PASSWORD_LANG_ID_APPLE *****/
	{"Apply","تأكيد"},
	/***** SETTING_PASSWORD_LANG_ID_ENTER_PWD *****/
	{"Enter Password","الرجاء إدخال كلمة المرور"},
	/***** SETTING_PASSWORD_LANG_ID_PWD_RESET *****/
	{"Password reset", "إعادة تعيين كلمة المرور"},
	/***** SETTING_PASSWORD_LANG_ID_NEW_PWD *****/
	{"New password","كلمة المرور الجديدة"},
	/***** SETTING_PASSWORD_LANG_ID_CONFIRM_PWD *****/
	{"Confirm Password","تأكيد كلمة المرور"},
};
/***
** 日期: 2022-05-07 15:55
** 作者: leo.liu
** 函数作用：密码相关字符串资源
** 返回参数说明：
***/
const char *layout_setting_password_string_get(LAYOUT_SETTING_PASSWORD_LANG_ID id)
{
	return layout_setting_password_language[id][language_cur_id];
}

static const char *layout_playback_language[PLAYBACK_LANG_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/***** PLAYBACK_LANG_ID_DELETE_PHOTO *****/
	{"Delete the photo?","حذف الصورة؟"},
	{"Delete the video?","حذف الفيديو؟"},
};
/***
**   日期:2022-05-24 11:17:49
**   作者: leo.liu
**   函数作用：playback
**   参数说明:
***/
const char *layout_playback_string_get(LAYOUT_PLAYBACK_LANGUAGE_ID id)
{
	return layout_playback_language[id][language_cur_id];
}

static const char *layout_security_language[LAYOUT_SECURITY_LANGUAGE_ID_TOTAL][LANGUAGE_ID_TOTAL] =
    {
	/*****   LAYOUT_SECURITY_LANGUAGE_ID_AUTO_REC*****/
	{"Auto Rec", "التسجيل التلقائي"},
	/***** LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1  *****/
	{"Security Sensor 1","جهاز استشعار الأمان  1"},
	/***** LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_2  *****/
	{"Security Sensor 2", "مستشعر الأمان  2"},
	/***** LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_WORKING  *****/
	{"Security function is in operation","وظيفة الأمان قيد التشغيل"},
	/***** LAYOUT_SECURITY_LANGUAGE_ID_END  *****/
	{"Disable Alarm","أنهاء"},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_EXE *****/
	{"Enable Alarm","تمكين المنبه"},
	/*****   LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_ERROR *****/
	{"Security 1 Please check the sensor status", " يرجى التحقق من حالة مستشعر الأمان 1"},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_2_ERROR *****/
	{"Security 2 Please check the sensor status","يرجى التحقق من حالة مستشعر الأمان 2"},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_AND_2_ERROR *****/
	{"Security 1,2 Please check the sensor status", "يرجى التحقق من حالة مستشعر الأمان  1  ،  2 "},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY *****/
	{"Security emergency","الطوارئ الأمنية"
	 },
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_1 *****/
	{
	    "Security sensor 1 emergency", "مستشعر الأمان 1  الطوارئ الأمنية"},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_2 *****/
	{
	    "Security sensor 2 emergency", "مستشعر الأمان 2 الطوارئ الأمنية"},
	/*****  LAYOUT_SECURITY_LANGUAGE_ID_EMERITY_RECORD *****/
	{"Emergency records","سجلات الطوارئ"},
};
/***
**   日期:2022-05-26 14:39:55
**   作者: leo.liu
**   函数作用：安全警报字符资源
**   参数说明:
***/
const char *layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID id)
{
	return layout_security_language[id][language_cur_id];
}

static const char *layout_intercom_language[LAYOUT_INTERCOM_LANGUAGE_ID_TOTAL][LANGUAGE_ID_TOTAL] =
{
	/*****  LAYOUT_INTERCOM_LANGUAGE_ID_CALLING *****/
	{"Connecting","توصيل..."},
	/*****  LAYOUT_INTERCOM_LANGUAGE_ID_TALKING *****/
	{"Connected","متصل"},
	{"Please select a room number","يرجى اختيار رقم الغرفة"},
	{"No extension monitor was connected","لم يتم ضبط الخط الداخلي"},
};
/***
**   日期:2022-05-30 09:17:03
**   作者: leo.liu
**   函数作用：intercom页面
**   参数说明:
***/
const char *layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID id)
{
	return layout_intercom_language[id][language_cur_id];
}

static const char *layout_wifi_language[LAYOUT_WIFI_LANGUAGE_ID_TOTAL][LANGUAGE_ID_TOTAL] = {

    /*****  LAYOUT_WIFI_LANGUAGE_ID_WIFI *****/
    {"Wi-Fi","واي فاي"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_NETWORK_CONNECTED *****/
    {"Connected network","اتصال الشبكه"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_SEARCHED_NETWORK *****/
    {"Searched network",
    
     "بحث الشبكه"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_WIFI_SECURITY *****/
    {"Security",
    
     "حماية"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_IP_ADDRES *****/
    {"IP account",
    
     "عنوان IP"},
    /*****LAYOUT_WIFI_LANGUAGE_ID_ADD_WIFI   *****/
    {"Add Wi-Fi network",
    
     "أضف شبكة Wi-Fi"},
    /*****LAYOUT_WIFI_LANGUAGE_ID_ENTER_ACCOUNT   *****/
    {"Please enter the account number",
     
     "الرجاء إدخال رقم الحساب"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_WIFI_CONNECTING *****/
    {"connecting", "توصيل"},
    /*****  LAYOUT_WIFI_LANGUAGE_ID_NTWORK_ERROR *****/
    {"Network Error.Please check the network","خطأ في الشبكة. يرجى التحقق من شبكة الإنترنت."},

	  {"name empty.","name empty."},

	  {"Name short.","اسم قصير."},

	{"Password short.","كلمة المرور قصيرة."},

	{"pwd long.","pwd long."},

	  {"Pwd error.","خطأ في كلمة المرور"},

	{"forget the network","خطأ في كلمة المرور"},


};
/***
**   日期:2022-06-06 08:35:34
**   作者: leo.liu
**   函数作用：wifi所需资源
**   参数说明:
***/
const char *layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID id)
{
	return layout_wifi_language[id][language_cur_id];
}