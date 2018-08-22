/*
 * rtc-s2mps18.c
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  2013-12-11  Performance improvements and code clean up by
 *		Minsung Kim <ms925.kim@samsung.com>
 *
 */
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/rtc.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/regmap.h>
#include <linux/platform_device.h>
#include <linux/mfd/samsung/rtc-s2mp.h>
#include <linux/mfd/samsung/s2mps18.h>
#include <linux/mfd/samsung/s2mps18-private.h>

/*#define CONFIG_WEEKDAY_ALARM_ENABLE*/

#if defined(CONFIG_RTC_ALARM_BOOT)
#include <linux/reboot.h>
#include <linux/wakelock.h>
#endif
#ifdef CONFIG_SEC_DEBUG_EXTRA_INFO
#include <linux/sec_debug.h>
#endif

struct s2m_rtc_info {
	struct device		*dev;
	struct i2c_client   	*i2c;
	struct i2c_client   	*pmic_i2c;
	struct s2mps18_dev	*iodev;
	struct rtc_device	*rtc_dev;
	struct mutex		lock;
	struct work_struct	irq_work;
	int			irq;
#if defined(CONFIG_RTC_ALARM_BOOT)
	int			alarm_boot_irq;
	bool			lpm_mode;
	bool			alarm_irq_flag;
	struct			wake_lock alarm_wake_lock;
#endif
	int			smpl_irq;
	bool			use_irq;
	bool			wtsr_en;
	bool			smpl_en;
	bool			alarm_enabled;
	u8			update_reg;
	bool			use_alarm_workaround;
	bool			alarm_check;
	u8			wudr_mask;
	u8			audr_mask;
	int			smpl_warn_info;
};

static struct wakeup_source *rtc_ws;

#ifdef CONFIG_SEC_PM
static bool is_rtc_cleared;
#endif

enum S2M_RTC_OP {
	S2M_RTC_READ,
	S2M_RTC_WRITE_TIME,
	S2M_RTC_WRITE_ALARM,
};

static void s2m_data_to_tm(u8 *data, struct rtc_time *tm)
{
//	tm->tm_msec = (data[RTC_MSEC] & 0x0f) + (data[RTC_MSEC] & 0xf0) * 10;
	tm->tm_sec = data[RTC_SEC] & 0x7f;
	tm->tm_min = data[RTC_MIN] & 0x7f;
	tm->tm_hour = data[RTC_HOUR] & 0x1f;
	tm->tm_wday = __fls(data[RTC_WEEKDAY] & 0x7f);
	tm->tm_mday = data[RTC_DATE] & 0x1f;
	tm->tm_mon = (data[RTC_MONTH] & 0x0f) - 1;
	tm->tm_year = (data[RTC_YEAR] & 0x7f) + 100;
	tm->tm_yday = 0;
	tm->tm_isdst = 0;
}

static int s2m_tm_to_data(struct rtc_time *tm, u8 *data)
{
//	data[RTC_MSEC] = ((tm->tm_msec / 10) << 4) | (tm->tm_msec % 10);
	data[RTC_SEC] = tm->tm_sec;
	data[RTC_MIN] = tm->tm_min;

	if (tm->tm_hour >= 12)
		data[RTC_HOUR] = tm->tm_hour | HOUR_PM_MASK;
	else
		data[RTC_HOUR] = tm->tm_hour;

	data[RTC_WEEKDAY] = BIT(tm->tm_wday);
	data[RTC_DATE] = tm->tm_mday;
	data[RTC_MONTH] = tm->tm_mon + 1;
	data[RTC_YEAR] = tm->tm_year > 100 ? (tm->tm_year - 100) : 0 ;

	if (tm->tm_year < 100) {
		pr_warn("%s: SEC RTC cannot handle the year %d\n", __func__,
				1900 + tm->tm_year);
		return -EINVAL;
	}
	return 0;
}

static int s2m_rtc_update(struct s2m_rtc_info *info,
				 enum S2M_RTC_OP op)
{
	u8 data;
	int ret;

	if (!info || !info->iodev) {
		pr_err("%s: Invalid argument\n", __func__);
		return -EINVAL;
	}

	switch (op) {
	case S2M_RTC_READ:
		data = RTC_RUDR_MASK;
		break;
	case S2M_RTC_WRITE_TIME:
		data = RTC_WUDR_MASK_REV;
		break;
	case S2M_RTC_WRITE_ALARM:
		data = RTC_AUDR_MASK_REV;
		break;
	default:
		dev_err(info->dev, "%s: invalid op(%d)\n", __func__, op);
		return -EINVAL;
	}

	data |= info->update_reg;

	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_UPDATE, data);
	if (ret < 0)
		dev_err(info->dev, "%s: fail to write update reg(%d,%u)\n",
				__func__, ret, data);
	else
		usleep_range(1000, 2000);

	return ret;
}

static int s2m_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	u8 data[NR_RTC_CNT_REGS];
	int ret;

	mutex_lock(&info->lock);
	ret = s2m_rtc_update(info, S2M_RTC_READ);
	if (ret < 0)
		goto out;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_SEC, NR_RTC_CNT_REGS,
			data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read time reg(%d)\n", __func__,
			ret);
		goto out;
	}

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
			__func__, data[RTC_YEAR] + 2000, data[RTC_MONTH],
			data[RTC_DATE], data[RTC_HOUR] & 0x1f, data[RTC_MIN],
			data[RTC_SEC], data[RTC_WEEKDAY],
			data[RTC_HOUR] & HOUR_PM_MASK ? "PM" : "AM");

	s2m_data_to_tm(data, tm);
	ret = rtc_valid_tm(tm);
out:
	mutex_unlock(&info->lock);
	return ret;
}

static int s2m_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	u8 data[NR_RTC_CNT_REGS];
	int ret;

	ret = s2m_tm_to_data(tm, data);
	if (ret < 0)
		return ret;

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
			__func__, data[RTC_YEAR] + 2000, data[RTC_MONTH],
			data[RTC_DATE], data[RTC_HOUR] & 0x1f, data[RTC_MIN],
			data[RTC_SEC], data[RTC_WEEKDAY],
			data[RTC_HOUR] & HOUR_PM_MASK ? "PM" : "AM");

	mutex_lock(&info->lock);
	ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_SEC, NR_RTC_CNT_REGS,
			data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write time reg(%d)\n", __func__,
			ret);
		goto out;
	}

	ret = s2m_rtc_update(info, S2M_RTC_WRITE_TIME);
out:
	mutex_unlock(&info->lock);
	return ret;
}

/* This is a workaround for the problem that RTC TIME is overwirted by write
 * buffer when setting RTC ALARM. It is quite rare but it does happen. The root
 * cuase is that clear signal of RUDR & WUDR is 1 clock delay while it should be
 * 2 clock delay.
 */
static int s2m_rtc_check_rtc_time(struct s2m_rtc_info *info)
{
	u8 data[NR_RTC_CNT_REGS];
	struct rtc_time tm;
	struct timeval sys_time;
	unsigned long rtc_time;
	int ret;

	/* Read RTC TIME */
	ret = s2m_rtc_update(info, S2M_RTC_READ);
	if (ret < 0)
		goto out;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_SEC, NR_RTC_CNT_REGS,
			data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read time reg(%d)\n", __func__,
			ret);
		goto out;
	}

	/* Get system time */
	do_gettimeofday(&sys_time);

	/* Convert RTC TIME to seconds since 01-01-1970 00:00:00. */
	s2m_data_to_tm(data, &tm);
	rtc_tm_to_time(&tm, &rtc_time);

	if (abs(rtc_time - sys_time.tv_sec) > 2) {
		/* Set RTC TIME */
		rtc_time_to_tm(sys_time.tv_sec, &tm);
		ret = s2m_tm_to_data(&tm, data);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to tm_to_data(%d)\n",
					__func__, ret);
			goto out;
		}

		ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_SEC,
				NR_RTC_CNT_REGS, data);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to write time reg(%d)\n",
					__func__, ret);
			goto out;
		}

		ret = s2m_rtc_update(info, S2M_RTC_WRITE_TIME);

		dev_warn(info->dev, "%s: adjust RTC TIME: sys_time: %lu, "
				"rtc_time: %lu\n", __func__, sys_time.tv_sec,
				rtc_time);

		dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
				__func__, data[RTC_YEAR] + 2000, data[RTC_MONTH],
				data[RTC_DATE], data[RTC_HOUR] & 0x1f, data[RTC_MIN],
				data[RTC_SEC], data[RTC_WEEKDAY],
				data[RTC_HOUR] & HOUR_PM_MASK ? "PM" : "AM");
	}
out:
	return ret;
}

static int s2m_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	u8 data[NR_RTC_CNT_REGS];
	u8 reg, val;
	int ret;

	mutex_lock(&info->lock);
	ret = s2m_rtc_update(info, S2M_RTC_READ);
	if (ret < 0)
		goto out;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_A0SEC, NR_RTC_CNT_REGS,
			data);
	if (ret < 0) {
		dev_err(info->dev, "%s:%d fail to read alarm reg(%d)\n",
			__func__, __LINE__, ret);
		goto out;
	}

	s2m_data_to_tm(data, &alrm->time);

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d(%d)\n", __func__,
			alrm->time.tm_year + 1900, alrm->time.tm_mon + 1,
			alrm->time.tm_mday, alrm->time.tm_hour,
			alrm->time.tm_min, alrm->time.tm_sec,
			alrm->time.tm_wday);

	alrm->enabled = info->alarm_enabled;
	alrm->pending = 0;

	switch (info->iodev->device_type) {
	case S2MPS18X:
		reg = S2MPS18_PMIC_REG_STATUS2;
		break;
	default:
		/* If this happens the core funtion has a problem */
		BUG();
	}

	ret = s2mps18_read_reg(info->pmic_i2c, reg, &val); /* i2c for PM */
	if (ret < 0) {
		dev_err(info->dev, "%s:%d fail to read STATUS2 reg(%d)\n",
			__func__, __LINE__, ret);
		goto out;
	}

	if (val & RTCA0E)
		alrm->pending = 1;
out:
	mutex_unlock(&info->lock);
	return ret;
}

static int s2m_rtc_set_alarm_enable(struct s2m_rtc_info *info, bool enabled)
{
	if (!info->use_irq)
		return -EPERM;

	if (enabled && !info->alarm_enabled) {
		info->alarm_enabled = true;
		enable_irq(info->irq);
	} else if (!enabled && info->alarm_enabled) {
		info->alarm_enabled = false;
		disable_irq(info->irq);
	}
	return 0;
}

static int s2m_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	u8 data[NR_RTC_CNT_REGS];
	int ret, i;

	mutex_lock(&info->lock);
	ret = s2m_tm_to_data(&alrm->time, data);
	if (ret < 0)
		goto out;

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d(0x%02x)%s\n",
			__func__, data[RTC_YEAR] + 2000, data[RTC_MONTH],
			data[RTC_DATE], data[RTC_HOUR] & 0x1f, data[RTC_MIN],
			data[RTC_SEC], data[RTC_WEEKDAY],
			data[RTC_HOUR] & HOUR_PM_MASK ? "PM" : "AM");

	if (info->alarm_check) {
		for (i = 0; i < NR_RTC_CNT_REGS; i++)
			data[i] &= ~ALARM_ENABLE_MASK;

		ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_A0SEC, NR_RTC_CNT_REGS,
				data);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to disable alarm reg(%d)\n",
				__func__, ret);
			goto out;
		}

		ret = s2m_rtc_update(info, S2M_RTC_WRITE_ALARM);
		if (ret < 0)
			goto out;
	}

	for (i = 0; i < NR_RTC_CNT_REGS; i++)
		data[i] |= ALARM_ENABLE_MASK;

	ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_A0SEC, NR_RTC_CNT_REGS,
			data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write alarm reg(%d)\n",
			__func__, ret);
		goto out;
	}

	ret = s2m_rtc_update(info, S2M_RTC_WRITE_ALARM);
	if (ret < 0)
		goto out;

	if (info->use_alarm_workaround) {
		ret = s2m_rtc_check_rtc_time(info);
		if (ret < 0)
			goto out;
	}

	ret = s2m_rtc_set_alarm_enable(info, alrm->enabled);
out:
	mutex_unlock(&info->lock);
	return ret;
}

#if defined(CONFIG_RTC_ALARM_BOOT)
static inline int s2m_rtc_set_update_reg(struct s2m_rtc_info *info,
		enum S2M_RTC_OP op)
{
	int ret;
	u8 data;

	ret = s2mps18_read_reg(info->i2c, S2MP_RTC_REG_UPDATE, &data);
	if (ret < 0)
		return ret;

	switch (op) {
	case S2M_RTC_READ:
		data |= RTC_RUDR_MASK;
		break;
	case S2M_RTC_WRITE_TIME:
		data |= info->wudr_mask;
		break;
	case S2M_RTC_WRITE_ALARM:
		data |= info->audr_mask;
		break;
	default:
		dev_err(info->dev, "%s: invalid op(%d)\n", __func__, op);
		return -EINVAL;
	}

	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_UPDATE, data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write update reg(%d)\n",
				__func__, ret);
	} else {
		usleep_range(1000, 2000);
	}

	return ret;
}

static int s2m_rtc_stop_alarm_boot(struct s2m_rtc_info *info)
{
	u8 data[NR_RTC_CNT_REGS];
	int ret, i;
	struct rtc_time tm;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data);
	if (ret < 0)
		return ret;

	s2m_data_to_tm(data, &tm);

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d\n", __func__,
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);

	for (i = 0; i < NR_RTC_CNT_REGS; i++)
		data[i] &= ~ALARM_ENABLE_MASK;

	ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data);
	if (ret < 0)
		return ret;

	ret = s2m_rtc_set_update_reg(info, S2M_RTC_WRITE_ALARM);

	return ret;
}

static int s2m_rtc_start_alarm_boot(struct s2m_rtc_info *info)
{
	int ret;
	u8 data[NR_RTC_CNT_REGS];
	struct rtc_time tm;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data);
	if (ret < 0)
		return ret;

	s2m_data_to_tm(data, &tm);

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d\n", __func__,
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);

	data[RTC_SEC] |= ALARM_ENABLE_MASK;
	data[RTC_MIN] |= ALARM_ENABLE_MASK;
	data[RTC_HOUR] |= ALARM_ENABLE_MASK;
	data[RTC_WEEKDAY] &= ~(ALARM_ENABLE_MASK);
	if (data[RTC_DATE] & 0x1f)
		data[RTC_DATE] |= ALARM_ENABLE_MASK;
	if (data[RTC_MONTH] & 0xf)
		data[RTC_MONTH] |= ALARM_ENABLE_MASK;
	if (data[RTC_YEAR] & 0x7f)
		data[RTC_YEAR] |= ALARM_ENABLE_MASK;

	ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data);
	if (ret < 0)
		return ret;

	ret = s2m_rtc_set_update_reg(info, S2M_RTC_WRITE_ALARM);

	return ret;
}

static int s2m_rtc_set_alarm_boot(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	u8 data[NR_RTC_CNT_REGS];
	int ret;

	mutex_lock(&info->lock);

	s2m_tm_to_data(&alrm->time, data);

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d\n", __func__,
			data[RTC_YEAR] + 2000, data[RTC_MONTH], data[RTC_DATE],
			data[RTC_HOUR] & 0x1f, data[RTC_MIN], data[RTC_SEC]);

	ret = s2m_rtc_stop_alarm_boot(info);
	if (ret < 0)
		return ret;

	ret = s2mps18_read_reg(info->i2c, S2MP_RTC_REG_UPDATE, &info->update_reg);
	if (ret < 0) {
		dev_err(info->dev, "%s: read fail\n", __func__);
		return ret;
	}

	if (alrm->enabled)
		info->update_reg |= RTC_WAKE_MASK;
	else
		info->update_reg &= ~RTC_WAKE_MASK;

	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_UPDATE,
			(char)info->update_reg);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write update reg(%d)\n",
				__func__, ret);
	} else {
		usleep_range(1000, 2000);
	}

	ret = s2mps18_bulk_write(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data);
	if (ret < 0)
		return ret;

	ret = s2m_rtc_set_update_reg(info, S2M_RTC_WRITE_ALARM);
	if (ret < 0)
		return ret;

	if (alrm->enabled)
		ret = s2m_rtc_start_alarm_boot(info);

	mutex_unlock(&info->lock);

	return ret;
}

static int s2m_rtc_get_alarm_boot(struct device *dev,
		struct rtc_wkalrm *alrm)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);

	if (info->alarm_irq_flag)
		alrm->enabled = 0x1;
	else
		alrm->enabled = 0x0;

	dev_info(info->dev, "%s: lpm(%d), enabled(%d)\n", __func__,
			info->lpm_mode, alrm->enabled);

	return info->lpm_mode;
}
#endif

static int s2m_rtc_alarm_irq_enable(struct device *dev,
					unsigned int enabled)
{
	struct s2m_rtc_info *info = dev_get_drvdata(dev);
	int ret;

	mutex_lock(&info->lock);
	ret = s2m_rtc_set_alarm_enable(info, enabled);
	mutex_unlock(&info->lock);
	return ret;
}

static irqreturn_t s2m_rtc_alarm_irq(int irq, void *data)
{
	struct s2m_rtc_info *info = data;

	if (!info->rtc_dev)
		return IRQ_HANDLED;

	dev_info(info->dev, "%s:irq(%d)\n", __func__, irq);

	rtc_update_irq(info->rtc_dev, 1, RTC_IRQF | RTC_AF);
	__pm_wakeup_event(rtc_ws, 500);
	return IRQ_HANDLED;
}

static const struct rtc_class_ops s2m_rtc_ops = {
	.read_time = s2m_rtc_read_time,
	.set_time = s2m_rtc_set_time,
	.read_alarm = s2m_rtc_read_alarm,
	.set_alarm = s2m_rtc_set_alarm,
#if defined(CONFIG_RTC_ALARM_BOOT)
	.set_alarm_boot = s2m_rtc_set_alarm_boot,
	.get_alarm_boot = s2m_rtc_get_alarm_boot,
#endif
	.alarm_irq_enable = s2m_rtc_alarm_irq_enable,
};

static void s2m_rtc_optimize_osc(struct s2m_rtc_info *info,
						struct s2mps18_platform_data *pdata)
{
	int ret = 0;

	/* edit option for OSC_BIAS_UP */
	if (pdata->osc_bias_up >= 0) {
		ret = s2mps18_update_reg(info->i2c, S2MP_RTC_REG_CAPSEL,
			pdata->osc_bias_up << OSC_BIAS_UP_SHIFT,
			OSC_BIAS_UP_MASK);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to write OSC_BIAS_UP(%d)\n",
				__func__, pdata->osc_bias_up);
			return;
		}
	}

	/* edit option for CAP_SEL */
	if (pdata->cap_sel >= 0) {
		ret = s2mps18_update_reg(info->i2c, S2MP_RTC_REG_CAPSEL,
			pdata->cap_sel << CAP_SEL_SHIFT, CAP_SEL_MASK);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to write CAP_SEL(%d)\n",
			__func__, pdata->cap_sel);
			return;
		}
	}

	/* edit option for OSC_CTRL */
	if (pdata->osc_xin >= 0) {
		ret = s2mps18_update_reg(info->i2c, S2MP_RTC_REG_OSCCTRL,
			pdata->osc_xin << OSC_XIN_SHIFT, OSC_XIN_MASK);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to write OSC_CTRL(%d)\n",
			__func__,  pdata->osc_xin);
			return;
		}
	}
	if (pdata->osc_xout >= 0) {
		ret = s2mps18_update_reg(info->i2c, S2MP_RTC_REG_OSCCTRL,
			pdata->osc_xout << OSC_XOUT_SHIFT, OSC_XOUT_MASK);
		if (ret < 0) {
			dev_err(info->dev, "%s: fail to write OSC_CTRL(%d)\n",
			__func__, pdata->osc_xout);
			return;
		}
	}
}

static bool s2m_is_jigonb_low(struct s2m_rtc_info *info)
{
	int ret, reg;
	u8 val, mask;

	switch (info->iodev->device_type) {
	case S2MPS18X:
		reg = S2MPS18_PMIC_REG_STATUS1;
		mask = BIT(1);
		break;
	default:
		BUG();
	}

	ret = s2mps18_read_reg(info->i2c, reg, &val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read status1 reg(%d)\n",
			__func__, ret);
		return false;
	}

	return !(val & mask);
}

#if defined(CONFIG_RTC_ALARM_BOOT)
static irqreturn_t s2m_rtc_alarm1_irq(int irq, void *data)
{
	struct s2m_rtc_info *info = data;

	dev_info(info->dev, "%s: irq(%d), lpm_mode:(%d)\n",
			__func__, irq, info->lpm_mode);

	if (info->lpm_mode) {
		wake_lock(&info->alarm_wake_lock);
		info->alarm_irq_flag = true;
	}

	rtc_update_irq(info->rtc_dev, 1, RTC_IRQF | RTC_AF);

	return IRQ_HANDLED;
}
#endif

extern unsigned int get_smpl_irq_num(void);

static unsigned int smpl_irq;

static irqreturn_t s2m_smpl_warn_irq_handler(int irq, void *data)
{
	struct s2m_rtc_info *info = data;

	if (!info->rtc_dev)
		return IRQ_HANDLED;

	if (gpio_get_value(info->smpl_warn_info) & 0x1)
		return IRQ_HANDLED;

	disable_irq_nosync(smpl_irq);

	return IRQ_WAKE_THREAD;
}

static irqreturn_t s2m_smpl_warn_irq(int irq, void *data)
{
	struct s2m_rtc_info *info = data;
	int state = 0;

	if (!info->rtc_dev)
		return IRQ_HANDLED;

	do {
		state = (gpio_get_value(info->smpl_warn_info) & 0x1);
		msleep(100);
	} while (!state);

	dev_info(info->dev, "%s: SMPL_WARN HAPPENED!\n", __func__);
	enable_irq(smpl_irq);

	return IRQ_HANDLED;
}

#ifdef CONFIG_SEC_DEBUG_EXTRA_INFO
static unsigned long smpl_warn_number = 0;
#endif

static void exynos_smpl_warn_work(struct work_struct *work)
{
	struct s2m_rtc_info *info = container_of(work,
			struct s2m_rtc_info, irq_work);
	int state = 0;

	do {
		state = (gpio_get_value(info->smpl_warn_info) & 0x1);
		msleep(100);
	} while (!state);

#ifdef CONFIG_SEC_DEBUG_EXTRA_INFO
	smpl_warn_number++;
	sec_debug_set_extra_info_smpl(smpl_warn_number);
#endif
	dev_info(info->dev, "%s: SMPL_WARN HAPPENED!\n", __func__);
}

static void s2m_rtc_enable_wtsr_smpl(struct s2m_rtc_info *info,
						struct s2mps18_platform_data *pdata)
{
	u8 val;
	int ret;

	if (pdata->wtsr_smpl->check_jigon && s2m_is_jigonb_low(info))
		pdata->wtsr_smpl->smpl_en = false;

	val = (pdata->wtsr_smpl->wtsr_en << WTSR_EN_SHIFT)
		| (pdata->wtsr_smpl->smpl_en << SMPL_EN_SHIFT)
		| WTSR_TIMER_BITS(pdata->wtsr_smpl->wtsr_timer_val)
		| SMPL_TIMER_BITS(pdata->wtsr_smpl->smpl_timer_val);

	dev_info(info->dev, "%s: WTSR: %s, SMPL: %s\n", __func__,
			pdata->wtsr_smpl->wtsr_en ? "enable" : "disable",
			pdata->wtsr_smpl->smpl_en ? "enable" : "disable");

	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_WTSR_SMPL, val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write WTSR/SMPL reg(%d)\n",
				__func__, ret);
		return;
	}
	info->wtsr_en = pdata->wtsr_smpl->wtsr_en;
	info->smpl_en = pdata->wtsr_smpl->smpl_en;
}

static void s2m_rtc_disable_wtsr_smpl(struct s2m_rtc_info *info,
					struct s2mps18_platform_data *pdata)
{
	int ret;

	dev_info(info->dev, "%s: disable WTSR\n", __func__);
	ret = s2mps18_update_reg(info->i2c, S2MP_RTC_REG_WTSR_SMPL, 0,
			WTSR_EN_MASK | SMPL_EN_MASK);
	if (ret < 0)
		dev_err(info->dev, "%s: fail to update WTSR reg(%d)\n",
				__func__, ret);
}

static int s2m_rtc_init_reg(struct s2m_rtc_info *info,
				struct s2mps18_platform_data *pdata)
{
	u8 data, update_val, ctrl_val, capsel_val;
	int ret;

#if defined(CONFIG_RTC_ALARM_BOOT)
	u8 data_alm1[NR_RTC_CNT_REGS];
	struct rtc_time alrm;

	ret = s2mps18_bulk_read(info->i2c, S2MP_RTC_REG_A1SEC, NR_RTC_CNT_REGS, data_alm1);
	if (ret < 0)
		return ret;

	s2m_data_to_tm(data_alm1, &alrm);

	dev_info(info->dev, "%s: %d-%02d-%02d %02d:%02d:%02d\n", __func__,
			alrm.tm_year + 1900, alrm.tm_mon + 1, alrm.tm_mday,
			alrm.tm_hour, alrm.tm_min, alrm.tm_sec);
#endif

	ret = s2mps18_read_reg(info->i2c, S2MP_RTC_REG_UPDATE, &update_val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read update reg(%d)\n",
			__func__, ret);
		return ret;
	}

	info->update_reg =
		update_val & ~(info->wudr_mask | RTC_FREEZE_MASK | RTC_RUDR_MASK | info->audr_mask);

	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_UPDATE, info->update_reg);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write update reg(%d)\n",
			__func__, ret);
		return ret;
	}

	s2m_rtc_update(info, S2M_RTC_READ);

	ret = s2mps18_read_reg(info->i2c, S2MP_RTC_REG_CTRL, &ctrl_val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read control reg(%d)\n",
			__func__, ret);
		return ret;
	}

	ret = s2mps18_read_reg(info->i2c, S2MP_RTC_REG_CAPSEL, &capsel_val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to read cap_sel reg(%d)\n",
			__func__, ret);
		return ret;
	}

	/* If the value of RTC_CTRL register is 0, RTC registers were reset */
	if ((ctrl_val & MODEL24_MASK) && ((capsel_val & 0xf0) == 0xf0))
		return 0;

#ifdef CONFIG_SEC_PM
	is_rtc_cleared = true;
#endif

	/* Set RTC control register : Binary mode, 24hour mode */
	data = MODEL24_MASK;
	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_CTRL, data);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write CTRL reg(%d)\n",
			__func__, ret);
		return ret;
	}

	ret = s2m_rtc_update(info, S2M_RTC_WRITE_ALARM);
	if (ret < 0)
		return ret;

	capsel_val |= 0xf0;
	ret = s2mps18_write_reg(info->i2c, S2MP_RTC_REG_CAPSEL, capsel_val);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to write CAP_SEL reg(%d)\n",
				__func__, ret);
		return ret;
	}

	if (pdata->init_time) {
		dev_info(info->dev, "%s: initialize RTC time\n", __func__);
		ret = s2m_rtc_set_time(info->dev, pdata->init_time);
	}
	else {
		dev_info(info->dev, "%s: RTC initialize is not operated: "
				"This causes a weekday problem\n", __func__);
	}
	return ret;
}

#ifdef CONFIG_SEC_PM
static ssize_t show_rtc_status(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	return sprintf(buf, "%u\n", is_rtc_cleared);
}

static DEVICE_ATTR(rtc_status, 0440, show_rtc_status, NULL);
#endif /* CONFIG_SEC_PM */

static int s2m_rtc_probe(struct platform_device *pdev)
{
	struct s2mps18_dev *iodev = dev_get_drvdata(pdev->dev.parent);
	struct s2mps18_platform_data *pdata = dev_get_platdata(iodev->dev);
	struct s2m_rtc_info *info;
	int irq_base;
	int ret = 0;

	info = devm_kzalloc(&pdev->dev, sizeof(struct s2m_rtc_info),
				GFP_KERNEL);

	if (!info)
		return -ENOMEM;

	irq_base = pdata->irq_base;
	if (!irq_base) {
		dev_err(&pdev->dev, "Failed to get irq base %d\n", irq_base);
		return -ENODEV;
	}

	mutex_init(&info->lock);
	info->dev = &pdev->dev;
	info->iodev = iodev;
	info->i2c = iodev->rtc;
	info->pmic_i2c = iodev->pmic;
	info->alarm_check = true;
	info->use_alarm_workaround = false;

	switch (info->iodev->device_type) {
	case S2MPS18X:
		info->irq = irq_base + S2MPS18_PMIC_IRQ_RTCA0_INT2;
#if defined(CONFIG_RTC_ALARM_BOOT)
		info->alarm_boot_irq = irq_base + S2MPS18_PMIC_IRQ_RTCA1_INT2;
		info->wudr_mask = RTC_WUDR_MASK_REV;
		info->audr_mask = RTC_AUDR_MASK_REV;
#endif
		break;
	default:
		/* If this happens the core funtion has a problem */
		BUG();
	}

	platform_set_drvdata(pdev, info);

	ret = s2m_rtc_init_reg(info, pdata);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to initialize RTC reg:%d\n", ret);
		goto err_rtc_init_reg;
	}

	/* enable wtsrm smpl */
	if (pdata->wtsr_smpl)
		s2m_rtc_enable_wtsr_smpl(info, pdata);

	s2m_rtc_optimize_osc(info, pdata);

	if (pdata->smpl_warn_en) {
		if (!gpio_is_valid(pdata->smpl_warn)) {
			dev_err(&pdev->dev, "smpl_warn GPIO NOT VALID\n");
			goto err_smpl_warn;
		}
		info->smpl_irq = gpio_to_irq(pdata->smpl_warn);

		ret = devm_request_threaded_irq(&pdev->dev, info->smpl_irq,
			s2m_smpl_warn_irq_handler,
			s2m_smpl_warn_irq,
			IRQF_TRIGGER_LOW | IRQF_ONESHOT,
			"SMPL WARN", info);
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to request smpl warn IRQ: %d: %d\n",
				info->smpl_irq, ret);
			goto err_smpl_warn;
		}
		info->smpl_warn_info = pdata->smpl_warn;
		INIT_WORK(&info->irq_work, exynos_smpl_warn_work);
	}

	device_init_wakeup(&pdev->dev, true);
	rtc_ws = wakeup_source_register("rtc-s2mp");

	ret = devm_request_threaded_irq(&pdev->dev, info->irq, NULL,
			s2m_rtc_alarm_irq, 0, "rtc-alarm0", info);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to request alarm IRQ: %d: %d\n",
			info->irq, ret);
		goto err_rtc_irq;
	}
	disable_irq(info->irq);
	disable_irq(info->irq);
	info->use_irq = true;

	info->rtc_dev = devm_rtc_device_register(&pdev->dev, "s2mp-rtc",
			&s2m_rtc_ops, THIS_MODULE);

	if (IS_ERR(info->rtc_dev)) {
		ret = PTR_ERR(info->rtc_dev);
		dev_err(&pdev->dev, "Failed to register RTC device: %d\n", ret);
		goto err_rtc_dev_register;
	}
	smpl_irq = get_smpl_irq_num();
	enable_irq(info->irq);

#ifdef CONFIG_SEC_PM
	ret = sysfs_create_file(power_kobj, &dev_attr_rtc_status.attr);
	if (ret)
		dev_err(&pdev->dev, "%s: failed to create rtc_status(%d)\n",
				__func__, ret);
#endif /* CONFIG_SEC_PM */

#if defined(CONFIG_RTC_ALARM_BOOT)
	ret = devm_request_threaded_irq(&pdev->dev, info->alarm_boot_irq, NULL,
			s2m_rtc_alarm1_irq, 0, "rtc-alarm1", info);

	if (ret < 0)
		dev_err(&pdev->dev, "Failed to request alarm IRQ: %d: %d\n",
				info->alarm_boot_irq, ret);

	info->lpm_mode = lpcharge;

	if (info->lpm_mode)
		wake_lock_init(&info->alarm_wake_lock, WAKE_LOCK_SUSPEND,
				"alarm_wake_lock");
#endif

	return 0;

err_rtc_dev_register:
	devm_free_irq(&pdev->dev, info->irq, info);
	enable_irq(info->irq);
	enable_irq(info->irq);
err_rtc_irq:
	wakeup_source_unregister(rtc_ws);
err_smpl_warn:
err_rtc_init_reg:
	platform_set_drvdata(pdev, NULL);
	mutex_destroy(&info->lock);

	return ret;
}

static int s2m_rtc_remove(struct platform_device *pdev)
{
	struct s2m_rtc_info *info = platform_get_drvdata(pdev);

	if (!info->alarm_enabled)
		enable_irq(info->irq);

#if defined(CONFIG_RTC_ALARM_BOOT)
	free_irq(info->alarm_boot_irq, info);
#endif

	wakeup_source_unregister(rtc_ws);

	return 0;
}

static void s2m_rtc_shutdown(struct platform_device *pdev)
{
	/*disable wtsr, smpl */
	struct s2m_rtc_info *info = platform_get_drvdata(pdev);
	struct s2mps18_platform_data *pdata =
			dev_get_platdata(info->iodev->dev);

	if (info->wtsr_en || info->smpl_en)
		s2m_rtc_disable_wtsr_smpl(info, pdata);
}

static const struct platform_device_id s2m_rtc_id[] = {
	{ "s2mps18-rtc", 0 },
	{},
};

static struct platform_driver s2m_rtc_driver = {
	.driver		= {
		.name	= "s2mps18-rtc",
		.owner	= THIS_MODULE,
	},
	.probe		= s2m_rtc_probe,
	.remove		= s2m_rtc_remove,
	.shutdown	= s2m_rtc_shutdown,
	.id_table	= s2m_rtc_id,
};

module_platform_driver(s2m_rtc_driver);

/* Module information */
MODULE_DESCRIPTION("Samsung RTC driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
