/*
 * drivers/media/platform/exynos/mfc/exynos_mfc_media.h
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __EXYNOS_MFC_MEDIA_H
#define __EXYNOS_MFC_MEDIA_H __FILE__

#include <linux/videodev2_exynos_media.h>

/* RGB formats */
#define V4L2_PIX_FMT_RGB32X    v4l2_fourcc('R', 'G', 'B', 'X') /* 32  RGB-8-8-8-8   */

/* two planes -- one Y, one Cr + Cb interleaved  */
#define V4L2_PIX_FMT_YUV444_2P v4l2_fourcc('Y', 'U', '2', 'P') /* 24  Y/CbCr */
#define V4L2_PIX_FMT_YVU444_2P v4l2_fourcc('Y', 'V', '2', 'P') /* 24  Y/CrCb */

/* three planes -- one Y, one Cr, one Cb */
#define V4L2_PIX_FMT_YUV444_3P v4l2_fourcc('Y', 'U', '3', 'P') /* 24  Y/Cb/Cr */

/* two non contiguous planes - one Y, one Cr + Cb interleaved  */
/* 21  Y/CrCb 4:2:0  */
#define V4L2_PIX_FMT_NV21M    v4l2_fourcc('N', 'M', '2', '1')
/* 12  Y/CbCr 4:2:0 16x16 macroblocks */
#define V4L2_PIX_FMT_NV12MT_16X16 v4l2_fourcc('V', 'M', '1', '2')

/* compressed formats */
#define V4L2_PIX_FMT_H264_MVC v4l2_fourcc('M', '2', '6', '4') /* H264 MVC */
#define V4L2_PIX_FMT_FIMV     v4l2_fourcc('F', 'I', 'M', 'V') /* FIMV  */
#define V4L2_PIX_FMT_FIMV1    v4l2_fourcc('F', 'I', 'M', '1') /* FIMV1 */
#define V4L2_PIX_FMT_FIMV2    v4l2_fourcc('F', 'I', 'M', '2') /* FIMV2 */
#define V4L2_PIX_FMT_FIMV3    v4l2_fourcc('F', 'I', 'M', '3') /* FIMV3 */
#define V4L2_PIX_FMT_FIMV4    v4l2_fourcc('F', 'I', 'M', '4') /* FIMV4 */
#define V4L2_PIX_FMT_VP8      v4l2_fourcc('V', 'P', '8', '0') /* VP8 */
#define V4L2_PIX_FMT_VP9      v4l2_fourcc('V', 'P', '9', '0') /* VP9 */
#define V4L2_PIX_FMT_HEVC    v4l2_fourcc('H', 'E', 'V', 'C') /* HEVC */

enum v4l2_mpeg_mfc51_video_frame_type {
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_NOT_CODED	= 0,
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_I_FRAME	= 1,
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_P_FRAME	= 2,
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_B_FRAME	= 3,
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_SKIPPED	= 4,
	V4L2_MPEG_MFC51_VIDEO_FRAME_TYPE_OTHERS		= 5,
};

enum v4l2_mpeg_video_hevc_hierarchical_coding_type {
	V4L2_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_B	= 0,
	V4L2_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_P	= 1,
};

/* new entry for enum v4l2_mpeg_video_mpeg4_level */
#define V4L2_MPEG_VIDEO_MPEG4_LEVEL_6			8

/* new entry for enum v4l2_mpeg_video_header_mode */
#define V4L2_MPEG_VIDEO_HEADER_MODE_AT_THE_READY	2

/* new entry for enum v4l2_mpeg_video_multi_slice_mode */
#define V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_MB_ROW		3
#define V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_FIXED_BYTES	4

/* new entry for enum v4l2_mpeg_video_h264_profile */
#define V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_HIGH	17

#define V4L2_MPEG_VIDEO_H264_LOOP_FILTER_MODE_DISABLED_S_B \
	V4L2_MPEG_VIDEO_H264_LOOP_FILTER_MODE_DISABLED_AT_SLICE_BOUNDARY

#define V4L2_CID_MPEG_MFC51_VIDEO_I_PERIOD_CH	V4L2_CID_MPEG_VIDEO_GOP_SIZE
#define V4L2_CID_MPEG_MFC51_VIDEO_BIT_RATE_CH	V4L2_CID_MPEG_VIDEO_BITRATE
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_RATE_CH		\
					V4L2_CID_MPEG_MFC51_VIDEO_H264_RC_FRAME_RATE


/* CID base for MFC controls (MPEG_CLASS) */
#define V4L2_CID_MPEG_MFC_BASE		(V4L2_CTRL_CLASS_MPEG | 0x2000)

#define V4L2_CID_MPEG_VIDEO_H264_SEI_FP_AVAIL		\
					(V4L2_CID_MPEG_MFC_BASE + 1)
#define V4L2_CID_MPEG_VIDEO_H264_SEI_FP_ARRGMENT_ID	\
					(V4L2_CID_MPEG_MFC_BASE + 2)
#define V4L2_CID_MPEG_VIDEO_H264_SEI_FP_INFO		\
					(V4L2_CID_MPEG_MFC_BASE + 3)
#define V4L2_CID_MPEG_VIDEO_H264_SEI_FP_GRID_POS	\
					(V4L2_CID_MPEG_MFC_BASE + 4)
#define V4L2_CID_MPEG_MFC51_VIDEO_PACKED_PB		\
					(V4L2_CID_MPEG_MFC_BASE + 5)
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_TAG		\
					(V4L2_CID_MPEG_MFC_BASE + 6)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_ENABLE		\
					(V4L2_CID_MPEG_MFC_BASE + 7)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_LUMA		\
					(V4L2_CID_MPEG_MFC_BASE + 8)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_CHROMA	\
					(V4L2_CID_MPEG_MFC_BASE + 9)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_LUMA_BOT	\
					(V4L2_CID_MPEG_MFC_BASE + 10)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_CHROMA_BOT	\
					(V4L2_CID_MPEG_MFC_BASE + 11)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_GENERATED		\
					(V4L2_CID_MPEG_MFC_BASE + 12)
#define V4L2_CID_MPEG_MFC51_VIDEO_CHECK_STATE		\
					(V4L2_CID_MPEG_MFC_BASE + 13)
#define V4L2_CID_MPEG_MFC51_VIDEO_DISPLAY_STATUS	\
					(V4L2_CID_MPEG_MFC_BASE + 14)
#define V4L2_CID_MPEG_MFC51_VIDEO_LUMA_ADDR	\
					(V4L2_CID_MPEG_MFC_BASE + 15)
#define V4L2_CID_MPEG_MFC51_VIDEO_CHROMA_ADDR	\
					(V4L2_CID_MPEG_MFC_BASE + 16)
#define V4L2_CID_MPEG_MFC51_VIDEO_STREAM_SIZE		\
					(V4L2_CID_MPEG_MFC_BASE + 17)
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_COUNT		\
					(V4L2_CID_MPEG_MFC_BASE + 18)
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_TYPE		\
					(V4L2_CID_MPEG_MFC_BASE + 19)
#define V4L2_CID_MPEG_MFC51_VIDEO_H264_INTERLACE	\
					(V4L2_CID_MPEG_MFC_BASE + 20)
#define V4L2_CID_MPEG_MFC51_VIDEO_H264_RC_FRAME_RATE	\
					(V4L2_CID_MPEG_MFC_BASE + 21)
#define V4L2_CID_MPEG_MFC51_VIDEO_MPEG4_VOP_TIME_RES	\
					(V4L2_CID_MPEG_MFC_BASE + 22)
#define V4L2_CID_MPEG_MFC51_VIDEO_MPEG4_VOP_FRM_DELTA	\
					(V4L2_CID_MPEG_MFC_BASE + 23)
#define V4L2_CID_MPEG_MFC51_VIDEO_H263_RC_FRAME_RATE	\
					(V4L2_CID_MPEG_MFC_BASE + 24)
#define V4L2_CID_MPEG_MFC6X_VIDEO_FRAME_DELTA		\
					(V4L2_CID_MPEG_MFC_BASE + 25)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_CHROMA1	\
					(V4L2_CID_MPEG_MFC_BASE + 26)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_2BIT_LUMA	\
						(V4L2_CID_MPEG_MFC_BASE + 27)
#define V4L2_CID_MPEG_MFC51_VIDEO_CRC_DATA_2BIT_CHROMA	\
						(V4L2_CID_MPEG_MFC_BASE + 28)

#define V4L2_CID_MPEG_VIDEO_H264_MVC_VIEW_ID			\
					(V4L2_CID_MPEG_MFC_BASE + 42)
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_STATUS			\
					(V4L2_CID_MPEG_MFC_BASE + 43)
#define V4L2_CID_MPEG_MFC51_VIDEO_I_FRAME_DECODING		\
					(V4L2_CID_MPEG_MFC_BASE + 44)
#define V4L2_CID_MPEG_MFC51_VIDEO_FRAME_RATE			\
					(V4L2_CID_MPEG_MFC_BASE + 45)
#define V4L2_CID_MPEG_VIDEO_H264_PREPEND_SPSPPS_TO_IDR		\
					(V4L2_CID_MPEG_MFC_BASE + 46)
#define V4L2_CID_MPEG_VIDEO_DECODER_IMMEDIATE_DISPLAY		\
					(V4L2_CID_MPEG_MFC_BASE + 47)
#define V4L2_CID_MPEG_VIDEO_DECODER_DECODING_TIMESTAMP_MODE	\
					(V4L2_CID_MPEG_MFC_BASE + 48)
#define V4L2_CID_MPEG_VIDEO_DECODER_WAIT_DECODING_START		\
					(V4L2_CID_MPEG_MFC_BASE + 49)
#define V4L2_CID_MPEG_VIDEO_QOS_RATIO				\
					(V4L2_CID_MPEG_MFC_BASE + 50)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT	\
					(V4L2_CID_MPEG_MFC_BASE + 51)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_CH \
					(V4L2_CID_MPEG_MFC_BASE + 52)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT0	\
					(V4L2_CID_MPEG_MFC_BASE + 53)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT1	\
					(V4L2_CID_MPEG_MFC_BASE + 54)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT2	\
					(V4L2_CID_MPEG_MFC_BASE + 55)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT3	\
					(V4L2_CID_MPEG_MFC_BASE + 56)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT4	\
					(V4L2_CID_MPEG_MFC_BASE + 57)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT5	\
					(V4L2_CID_MPEG_MFC_BASE + 58)
#define V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_BIT6	\
					(V4L2_CID_MPEG_MFC_BASE + 59)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_VERSION			\
					(V4L2_CID_MPEG_MFC_BASE + 60)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_RC_FRAME_RATE		\
					(V4L2_CID_MPEG_MFC_BASE + 61)
#define V4L2_CID_MPEG_VIDEO_VP8_MIN_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 62)
#define V4L2_CID_MPEG_VIDEO_VP8_MAX_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 63)
#define V4L2_CID_MPEG_VIDEO_VP8_I_FRAME_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 64)
#define V4L2_CID_MPEG_VIDEO_VP8_P_FRAME_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 65)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_NUM_OF_PARTITIONS		\
					(V4L2_CID_MPEG_MFC_BASE + 66)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_FILTER_LEVEL		\
					(V4L2_CID_MPEG_MFC_BASE + 67)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_FILTER_SHARPNESS		\
					(V4L2_CID_MPEG_MFC_BASE + 68)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_GOLDEN_FRAMESEL		\
					(V4L2_CID_MPEG_MFC_BASE + 69)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_GF_REFRESH_PERIOD		\
					(V4L2_CID_MPEG_MFC_BASE + 70)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_HIERARCHY_QP_ENABLE	\
					(V4L2_CID_MPEG_MFC_BASE + 71)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_HIERARCHY_QP_LAYER0	\
					(V4L2_CID_MPEG_MFC_BASE + 72)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_HIERARCHY_QP_LAYER1	\
					(V4L2_CID_MPEG_MFC_BASE + 73)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_HIERARCHY_QP_LAYER2	\
					(V4L2_CID_MPEG_MFC_BASE + 74)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_REF_NUMBER_FOR_PFRAMES	\
					(V4L2_CID_MPEG_MFC_BASE + 75)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_DISABLE_INTRA_MD4X4	\
					(V4L2_CID_MPEG_MFC_BASE + 76)
#define V4L2_CID_MPEG_MFC70_VIDEO_VP8_NUM_TEMPORAL_LAYER	\
					(V4L2_CID_MPEG_MFC_BASE + 77)
#define V4L2_CID_MPEG_VIDEO_VP8_HIERARCHICAL_CODING_LAYER_BIT	\
					(V4L2_CID_MPEG_MFC_BASE + 78)
#define V4L2_CID_MPEG_VIDEO_VP8_HIERARCHICAL_CODING_LAYER_CH \
					(V4L2_CID_MPEG_MFC_BASE + 79)
#define V4L2_CID_MPEG_VIDEO_VP8_HIERARCHICAL_CODING_LAYER_BIT0	\
					(V4L2_CID_MPEG_MFC_BASE + 80)
#define V4L2_CID_MPEG_VIDEO_VP8_HIERARCHICAL_CODING_LAYER_BIT1	\
					(V4L2_CID_MPEG_MFC_BASE + 81)
#define V4L2_CID_MPEG_VIDEO_VP8_HIERARCHICAL_CODING_LAYER_BIT2	\
					(V4L2_CID_MPEG_MFC_BASE + 82)

/* ~ 90 : Reserved for using later */

#define V4L2_CID_MPEG_MFC_GET_VERSION_INFO			\
					(V4L2_CID_MPEG_MFC_BASE + 91)
#define V4L2_CID_MPEG_MFC_GET_EXTRA_BUFFER_SIZE			\
					(V4L2_CID_MPEG_MFC_BASE + 92)
#define V4L2_CID_MPEG_MFC_SET_DUAL_DPB_MODE			\
					(V4L2_CID_MPEG_MFC_BASE + 93)
#define V4L2_CID_MPEG_MFC_SET_DYNAMIC_DPB_MODE			\
					(V4L2_CID_MPEG_MFC_BASE + 95)
#define V4L2_CID_MPEG_MFC_SET_USER_SHARED_HANDLE		\
					(V4L2_CID_MPEG_MFC_BASE + 96)
#define V4L2_CID_MPEG_MFC_GET_EXT_INFO				\
					(V4L2_CID_MPEG_MFC_BASE + 97)
#define V4L2_CID_MPEG_MFC_SET_BUF_PROCESS_TYPE			\
					(V4L2_CID_MPEG_MFC_BASE + 98)
#define V4L2_CID_MPEG_MFC_GET_10BIT_INFO			\
					(V4L2_CID_MPEG_MFC_BASE + 99)
#define V4L2_CID_MPEG_MFC_H264_ENABLE_LTR		\
					(V4L2_CID_MPEG_MFC_BASE + 100)
#define V4L2_CID_MPEG_MFC_H264_MARK_LTR			\
					(V4L2_CID_MPEG_MFC_BASE + 101)
#define V4L2_CID_MPEG_MFC_H264_USE_LTR			\
					(V4L2_CID_MPEG_MFC_BASE + 102)
#define V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MAX_MB_ROW	\
					(V4L2_CID_MPEG_MFC_BASE + 103)
#define V4L2_CID_MPEG_MFC_H264_BASE_PRIORITY		\
					(V4L2_CID_MPEG_MFC_BASE + 104)
#define V4L2_CID_MPEG_MFC_CONFIG_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 105)
#define V4L2_CID_MPEG_MFC_H264_VUI_RESTRICTION_ENABLE	\
					(V4L2_CID_MPEG_MFC_BASE + 106)
#define V4L2_CID_MPEG_MFC_GET_DRIVER_INFO			\
					(V4L2_CID_MPEG_MFC_BASE + 107)
#define V4L2_CID_MPEG_MFC_CONFIG_QP_ENABLE             \
					(V4L2_CID_MPEG_MFC_BASE + 108)

/* CIDs for HEVC encoding. Number gaps are for compatibility */

#define V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 110)
#define V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 111)
#define V4L2_CID_MPEG_VIDEO_HEVC_I_FRAME_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 112)
#define V4L2_CID_MPEG_VIDEO_HEVC_P_FRAME_QP                     \
					(V4L2_CID_MPEG_MFC_BASE + 113)
#define V4L2_CID_MPEG_VIDEO_HEVC_B_FRAME_QP                     \
					(V4L2_CID_MPEG_MFC_BASE + 114)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_QP_ENABLE \
					(V4L2_CID_MPEG_MFC_BASE + 115)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_TYPE       \
					(V4L2_CID_MPEG_MFC_BASE + 116)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER      \
					(V4L2_CID_MPEG_MFC_BASE + 117)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_QP   \
					(V4L2_CID_MPEG_MFC_BASE + 118)
#define V4L2_CID_MPEG_VIDEO_HEVC_PROFILE			\
					(V4L2_CID_MPEG_MFC_BASE + 120)
#define V4L2_CID_MPEG_VIDEO_HEVC_LEVEL				\
					(V4L2_CID_MPEG_MFC_BASE + 121)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_RC_FRAME_RATE            \
					(V4L2_CID_MPEG_MFC_BASE + 122)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_TIER_FLAG                \
					(V4L2_CID_MPEG_MFC_BASE + 123)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_MAX_PARTITION_DEPTH      \
					(V4L2_CID_MPEG_MFC_BASE + 124)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_REF_NUMBER_FOR_PFRAMES   \
					(V4L2_CID_MPEG_MFC_BASE + 125)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LF_DISABLE               \
					(V4L2_CID_MPEG_MFC_BASE + 126)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LF_SLICE_BOUNDARY        \
					(V4L2_CID_MPEG_MFC_BASE + 127)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LF_BETA_OFFSET_DIV2      \
					(V4L2_CID_MPEG_MFC_BASE + 128)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LF_TC_OFFSET_DIV2        \
					(V4L2_CID_MPEG_MFC_BASE + 129)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_REFRESH_TYPE             \
					(V4L2_CID_MPEG_MFC_BASE + 130)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_REFRESH_PERIOD           \
					(V4L2_CID_MPEG_MFC_BASE + 131)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LOSSLESS_CU_ENABLE       \
					(V4L2_CID_MPEG_MFC_BASE + 132)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_CONST_INTRA_PRED_ENABLE  \
					(V4L2_CID_MPEG_MFC_BASE + 133)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_WAVEFRONT_ENABLE         \
					(V4L2_CID_MPEG_MFC_BASE + 134)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_LTR_ENABLE               \
					(V4L2_CID_MPEG_MFC_BASE + 135)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_USER_REF                 \
					(V4L2_CID_MPEG_MFC_BASE + 136)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_STORE_REF                \
					(V4L2_CID_MPEG_MFC_BASE + 137)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_SIGN_DATA_HIDING         \
					(V4L2_CID_MPEG_MFC_BASE + 138)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_GENERAL_PB_ENABLE        \
					(V4L2_CID_MPEG_MFC_BASE + 139)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_TEMPORAL_ID_ENABLE       \
					(V4L2_CID_MPEG_MFC_BASE + 140)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_STRONG_SMOTHING_FLAG     \
					(V4L2_CID_MPEG_MFC_BASE + 141)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_MAX_NUM_MERGE_MV_MINUS1  \
					(V4L2_CID_MPEG_MFC_BASE + 142)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_ADAPTIVE_RC_DARK         \
					(V4L2_CID_MPEG_MFC_BASE + 143)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_ADAPTIVE_RC_SMOOTH       \
					(V4L2_CID_MPEG_MFC_BASE + 144)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_ADAPTIVE_RC_STATIC	\
					(V4L2_CID_MPEG_MFC_BASE + 145)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_ADAPTIVE_RC_ACTIVITY     \
					(V4L2_CID_MPEG_MFC_BASE + 146)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_DISABLE_INTRA_PU_SPLIT   \
					(V4L2_CID_MPEG_MFC_BASE + 147)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_DISABLE_TMV_PREDICTION	\
					(V4L2_CID_MPEG_MFC_BASE + 148)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_WITHOUT_STARTCODE_ENABLE \
					(V4L2_CID_MPEG_MFC_BASE + 149)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_QP_INDEX_CR		\
					(V4L2_CID_MPEG_MFC_BASE + 150)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_QP_INDEX_CB		\
					(V4L2_CID_MPEG_MFC_BASE + 151)
#define V4L2_CID_MPEG_MFC90_VIDEO_HEVC_SIZE_OF_LENGTH_FIELD	\
					(V4L2_CID_MPEG_MFC_BASE + 152)
#define V4L2_CID_MPEG_VIDEO_HEVC_PREPEND_SPSPPS_TO_IDR		\
					(V4L2_CID_MPEG_MFC_BASE + 153)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_CH	\
					(V4L2_CID_MPEG_MFC_BASE + 154)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT0 \
					(V4L2_CID_MPEG_MFC_BASE + 155)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT1 \
					(V4L2_CID_MPEG_MFC_BASE + 156)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT2 \
					(V4L2_CID_MPEG_MFC_BASE + 157)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT3 \
					(V4L2_CID_MPEG_MFC_BASE + 158)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT4 \
					(V4L2_CID_MPEG_MFC_BASE + 159)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT5 \
					(V4L2_CID_MPEG_MFC_BASE + 160)
#define V4L2_CID_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_LAYER_BIT6 \
					(V4L2_CID_MPEG_MFC_BASE + 161)

/* CIDs for VP9 encoding. Number gaps are for compatibility */
#define V4L2_CID_MPEG_VIDEO_VP9_VERSION				\
					(V4L2_CID_MPEG_MFC_BASE + 163)
#define V4L2_CID_MPEG_VIDEO_VP9_RC_FRAME_RATE			\
					(V4L2_CID_MPEG_MFC_BASE + 164)
#define V4L2_CID_MPEG_VIDEO_VP9_MIN_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 165)
#define V4L2_CID_MPEG_VIDEO_VP9_MAX_QP				\
					(V4L2_CID_MPEG_MFC_BASE + 166)
#define V4L2_CID_MPEG_VIDEO_VP9_I_FRAME_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 167)
#define V4L2_CID_MPEG_VIDEO_VP9_P_FRAME_QP			\
					(V4L2_CID_MPEG_MFC_BASE + 168)
#define V4L2_CID_MPEG_VIDEO_VP9_GOLDEN_FRAMESEL			\
					(V4L2_CID_MPEG_MFC_BASE + 169)
#define V4L2_CID_MPEG_VIDEO_VP9_GF_REFRESH_PERIOD		\
					(V4L2_CID_MPEG_MFC_BASE + 170)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHY_QP_ENABLE		\
					(V4L2_CID_MPEG_MFC_BASE + 171)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER_QP	\
					(V4L2_CID_MPEG_MFC_BASE + 172)
#define V4L2_CID_MPEG_VIDEO_VP9_REF_NUMBER_FOR_PFRAMES		\
					(V4L2_CID_MPEG_MFC_BASE + 173)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER	\
					(V4L2_CID_MPEG_MFC_BASE + 174)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER_CH \
					(V4L2_CID_MPEG_MFC_BASE + 175)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER_BIT0	\
					(V4L2_CID_MPEG_MFC_BASE + 176)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER_BIT1	\
					(V4L2_CID_MPEG_MFC_BASE + 177)
#define V4L2_CID_MPEG_VIDEO_VP9_HIERARCHICAL_CODING_LAYER_BIT2	\
					(V4L2_CID_MPEG_MFC_BASE + 178)
#define V4L2_CID_MPEG_VIDEO_VP9_MAX_PARTITION_DEPTH		\
					(V4L2_CID_MPEG_MFC_BASE + 179)
#define V4L2_CID_MPEG_VIDEO_VP9_DISABLE_INTRA_PU_SPLIT		\
					(V4L2_CID_MPEG_MFC_BASE + 180)
#define V4L2_CID_MPEG_VIDEO_DISABLE_IVF_HEADER			\
					(V4L2_CID_MPEG_MFC_BASE + 181)

/* CIDs for new common interface */
#define V4L2_CID_MPEG_VIDEO_ROI_CONTROL				\
					(V4L2_CID_MPEG_MFC_BASE + 190)
#define V4L2_CID_MPEG_VIDEO_ROI_ENABLE				\
					(V4L2_CID_MPEG_MFC_BASE + 191)
#define V4L2_CID_MPEG_VIDEO_RC_PVC_ENABLE			\
					(V4L2_CID_MPEG_MFC_BASE + 192)
#define V4L2_CID_MPEG_VIDEO_TEMPORAL_SHORTTERM_MAX_LAYER	\
					(V4L2_CID_MPEG_MFC_BASE + 193)
#define V4L2_CID_MPEG_VIDEO_BLACK_BAR_DETECT			\
					(V4L2_CID_MPEG_MFC_BASE + 194)
#define V4L2_CID_MPEG_MFC_H264_NUM_OF_LTR			\
					(V4L2_CID_MPEG_MFC_BASE + 195)
#define V4L2_CID_MPEG_VIDEO_WEIGHTED_ENABLE			\
					(V4L2_CID_MPEG_MFC_BASE + 196)
#define V4L2_CID_MPEG_VIDEO_YSUM				\
					(V4L2_CID_MPEG_MFC_BASE + 197)
#define V4L2_CID_MPEG_VIDEO_RATIO_OF_INTRA			\
					(V4L2_CID_MPEG_MFC_BASE + 198)

/* QP BOUND interface */
#define V4L2_CID_MPEG_VIDEO_H264_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 201)
#define V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 202)
#define V4L2_CID_MPEG_VIDEO_MPEG4_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 203)
#define V4L2_CID_MPEG_VIDEO_H263_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 204)
#define V4L2_CID_MPEG_VIDEO_VP8_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 205)
#define V4L2_CID_MPEG_VIDEO_VP9_MAX_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 206)
#define V4L2_CID_MPEG_VIDEO_H264_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 207)
#define V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 208)
#define V4L2_CID_MPEG_VIDEO_MPEG4_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 209)
#define V4L2_CID_MPEG_VIDEO_H263_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 210)
#define V4L2_CID_MPEG_VIDEO_VP8_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 211)
#define V4L2_CID_MPEG_VIDEO_VP9_MIN_QP_P			\
					(V4L2_CID_MPEG_MFC_BASE + 212)
#define V4L2_CID_MPEG_VIDEO_H264_MAX_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 213)
#define V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 214)
#define V4L2_CID_MPEG_VIDEO_MPEG4_MAX_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 215)
#define V4L2_CID_MPEG_VIDEO_H264_MIN_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 216)
#define V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 217)
#define V4L2_CID_MPEG_VIDEO_MPEG4_MIN_QP_B			\
					(V4L2_CID_MPEG_MFC_BASE + 218)

/* SEI & VUI PARSING FOR HDR DISPLAY */
#define V4L2_CID_MPEG_VIDEO_SEI_MAX_PIC_AVERAGE_LIGHT		\
					(V4L2_CID_MPEG_MFC_BASE + 219)
#define V4L2_CID_MPEG_VIDEO_SEI_MAX_CONTENT_LIGHT		\
					(V4L2_CID_MPEG_MFC_BASE + 220)
#define V4L2_CID_MPEG_VIDEO_SEI_MAX_DISPLAY_LUMINANCE		\
					(V4L2_CID_MPEG_MFC_BASE + 221)
#define V4L2_CID_MPEG_VIDEO_SEI_MIN_DISPLAY_LUMINANCE		\
					(V4L2_CID_MPEG_MFC_BASE + 222)
#define V4L2_CID_MPEG_VIDEO_MATRIX_COEFFICIENTS			\
					(V4L2_CID_MPEG_MFC_BASE + 223)
#define V4L2_CID_MPEG_VIDEO_FORMAT				\
					(V4L2_CID_MPEG_MFC_BASE + 224)
#define V4L2_CID_MPEG_VIDEO_SEI_WHITE_POINT			\
					(V4L2_CID_MPEG_MFC_BASE + 225)
#define V4L2_CID_MPEG_VIDEO_SEI_DISPLAY_PRIMARIES_0		\
					(V4L2_CID_MPEG_MFC_BASE + 226)
#define V4L2_CID_MPEG_VIDEO_SEI_DISPLAY_PRIMARIES_1		\
					(V4L2_CID_MPEG_MFC_BASE + 227)
#define V4L2_CID_MPEG_VIDEO_SEI_DISPLAY_PRIMARIES_2		\
					(V4L2_CID_MPEG_MFC_BASE + 228)
#define V4L2_CID_MPEG_VIDEO_FULL_RANGE_FLAG			\
					(V4L2_CID_MPEG_MFC_BASE + 229)
#define V4L2_CID_MPEG_VIDEO_COLOUR_PRIMARIES			\
					(V4L2_CID_MPEG_MFC_BASE + 230)
#define V4L2_CID_MPEG_VIDEO_TRANSFER_CHARACTERISTICS		\
					(V4L2_CID_MPEG_MFC_BASE + 231)

#endif /* __EXYNOS_MFC_MEDIA_H */
