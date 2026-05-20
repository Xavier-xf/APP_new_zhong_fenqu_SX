#ifndef _AK_AUDIO_CONFIG_H_
#define _AK_AUDIO_CONFIG_H_
#define ENABLE				   (1)
#define DISABLE                (0)
/**************************************/
/* user vqe param */
/**************************************/
/* ai agc */
#define USER_AI_AGC_LEVEL        (18022)        //default (0.75) * (1 << 15)
#define USER_AI_AGC_MIN_GAIN      (0)
#define USER_AI_AGC_MAX_GAIN      (4)
#define USER_AI_NEAR_SENSITIVE    (60)
											
/*         #### how to set agc ###      
struct ak_audio_agc_attr user_ai_agc = {0};
user_ai_agc.agc_level = USER_AI_AGC_LEVEL;
user_ai_agc.agc_max_gain = USER_AI_AGC_MAX_GAIN;
user_ai_agc.agc_min_gain = USER_AI_AGC_MIN_GAIN;
user_ai_agc.near_sensitivity = USER_AI_NEAR_SENSITIVE;
user_ai_agc.enable = ENABLE;
ak_ai_set_agc_attr(ai_handle_id,&user_ai_agc);*/
											
/* ai nr */
#define USER_AI_NOISE_SUP_DB     (-30)			// noise suppress db
											
/*         #### how to set ai nr ###      
struct ak_audio_nr_attr user_ai_nr_attr = {0};
user_ai_nr_attr.noise_suppress_db = USER_AI_NOISE_SUP_DB;
user_ai_nr_attr.enable = ENABLE;
ak_ai_set_nr_attr(ai_handle_id,&user_ai_nr_attr);*/
											
/* ai aec */
#define USER_AEC_OUT_DIGI_GAIN   (819)
#define USER_AEC_IN_DIGI_GAIN    (921)
#define USER_AEC_TAIL            (512)
#define USER_AEC_OUT_THRESHLD    (9830)
											
/*         #### how to set aec ###      
struct ak_audio_aec_attr user_aec_attr = {0};
user_aec_attr.audio_in_digi_gain = USER_AEC_IN_DIGI_GAIN;
user_aec_attr.audio_out_digi_gain = USER_AEC_OUT_DIGI_GAIN;
user_aec_attr.audio_out_threshold = USER_AEC_OUT_THRESHLD;
user_aec_attr.enable = ENABLE;
user_aec_attr.tail = 512; //don't need to set this value now
ak_ai_set_aec_attr(ai_handle_id, &user_aec_attr);*/
											
/* ai aslc */
#define USER_AI_LIMIT            (11468)
#define USER_AI_DB               (-10)
											
/*         #### how to set ai aslc ###      
struct ak_audio_aslc_attr user_ai_aslc_attr = {0};
user_ai_aslc_attr.aslc_db = USER_AI_DB;
user_ai_aslc_attr.limit = USER_AI_LIMIT;
ak_ai_set_aslc_attr(ai_handle_id, &user_ai_aslc_attr);*/
											
/* ao nr */
#define USER_AO_NOISE_SUP_DB     (-30)				// noise suppress db
/*         #### how to set ao nr ###      
struct ak_audio_nr_attr user_ao_nr_attr = {0};
user_ao_nr_attr.noise_suppress_db = USER_AO_NOISE_SUP_DB;
user_ao_nr_attr.enable = ENABLE;
ak_ao_set_nr_attr(ao_handle_id,&user_ao_nr_attr);*/
											
/* ao aslc */
#define USER_AO_LIMIT            (11468)
#define USER_AO_DB               (-1)
/*         #### how to set ao aslc ###      
struct ak_audio_aslc_attr user_ao_aslc_attr = {0};
user_ao_aslc_attr.aslc_db = USER_AO_DB;
user_ao_aslc_attr.limit = USER_AO_LIMIT;
ak_ao_set_aslc_attr(ao_handle_id, &user_ao_aslc_attr);*/
 
/* set ai voice quality enhancement param */
//struct ak_audio_nr_attr user_ai_nr_attr ={USER_AI_NOISE_SUP_DB,0,ENABLE};
//struct ak_audio_agc_attr user_ai_agc_attr = {USER_AI_AGC_LEVEL,USER_AI_AGC_MAX_GAIN,USER_AI_AGC_MIN_GAIN,USER_AI_NEAR_SENSITIVE, 0, ENABLE};
//struct ak_audio_aec_attr user_ai_aec_attr = {0, USER_AEC_OUT_DIGI_GAIN, USER_AEC_IN_DIGI_GAIN, 0, USER_AEC_TAIL, ENABLE};
//struct ak_audio_aslc_attr user_ai_aslc_attr = {USER_AI_LIMIT, USER_AI_DB, ENABLE};
 
/* set ao voice quality enhancement param */
//struct ak_audio_nr_attr user_ao_nr_attr = {USER_AO_NOISE_SUP_DB, 0, ENABLE};
//struct ak_audio_aslc_attr user_ao_aslc_attr = {USER_AO_LIMIT, USER_AO_DB, ENABLE};
/* gain */

#define USER_AI_GAIN    (3)
#define USER_AI_VOLUME  (-10)
 
/*			### how to set ai gain ###
ak_ai_set_gain(ai_handle_id,USER_AI_GAIN);*/

#define USER_AO_GAIN  (2)
 
/*			### how to set ao gain ###
ak_ao_set_gain(ao_handle_id,USER_AO_GAIN);*/

#define DEFAULT_VOLUME_L_INDEX  (0)
#define MIN_VOLUME_L_INDEX  (1)
#define MAX_VOLUME_L_INDEX  (6)
/*			### Tone volume level ###
 */
#define TONE_VOLUME_L_DEFAULT  (-9)
#define TONE_VOLUME_L_1  (-15)
#define TONE_VOLUME_L_2  (-12)
#define TONE_VOLUME_L_3  (-9)
#define TONE_VOLUME_L_4  (-6)
#define TONE_VOLUME_L_5  (-3)
#define TONE_VOLUME_L_6  (0)
#define TONE_VOLUME_L_MAX_INDEX  (8)

/*			### Talk volume level ###
 */
#define TALK_VOLUME_L_DEFAULT  (-7)
#define TALK_VOLUME_L_1  (-16)
#define TALK_VOLUME_L_2  (-13)
#define TALK_VOLUME_L_3  (-10)
#define TALK_VOLUME_L_4  (-7)
#define TALK_VOLUME_L_5  (-4)
#define TALK_VOLUME_L_6  (-1)
#define TALK_VOLUME_L_MAX_INDEX  (8)

 /*
struct ak_audio_eq_attr user_ai_eq_attr={
0,

10,

{ 50, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000},

{0,0,0,0,0,0,0,0,0,0},

{717,717,717,717,717,717,717,717,717,717},

{TYPE_HPF,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1},

0,

0,

0,

0,

0,

0,

0,

{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

};
  */



/*         #### how to set eq ###      
struct ak_audio_eq_attr m_eq = {0};
m_eq.pre_gain = (signed short)(0*(1<<10));
//if use exported above attr ,just use like this m_eq.pre_gain = xxx;
m_eq.aslc_ena = 0;
m_eq.aslc_level_max = 0;
m_eq.bands = 1;
m_eq.bandfreqs[0] = 1200;
m_eq.bandgains[0] = (signed short)(0*(1<<10));
//if use exported above attr ,just use like this m_eq.bandgains[0] = xxx;
m_eq.bandQ[0] = (unsigned short)(0.8*(1<<10));
//if use exported above attr ,just use like this m_eq.bandQ[0] = xxx;
m_eq.band_types[0] = TYPE_HPF;
m_eq.enable = 1;
m_eq.band_enable[0] =1;
ak_ai_set_eq_attr(ai_handle, &m_eq);*/

/*
struct ak_audio_eq_attr user_ao_eq_attr={
 0,

10,

{ 50, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000},

{0,0,0,0,0,0,0,0,0,0},

{717,717,717,717,717,717,717,717,717,717},

{TYPE_HPF,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1,TYPE_PF1},

0,

0,

0,

0,

0,

0,

0,

{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

};
*/
#endif
