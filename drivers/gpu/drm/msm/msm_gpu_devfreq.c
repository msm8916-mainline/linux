// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#include "msm_gpu.h"
#include "msm_gpu_trace.h"

#include <linux/devfreq.h>
#include <linux/devfreq_cooling.h>
#include <linux/math64.h>
#include <linux/units.h>

/*
 * Power Management:
 */
static unsigned long get_freq(struct msm_gpu *gpu);

static int msm_devfreq_target(struct device *dev, unsigned long *freq,
		u32 flags)
{
	struct msm_gpu *gpu = dev_to_gpu(dev);
	struct msm_gpu_devfreq *df = &gpu->devfreq;
	struct dev_pm_opp *opp;

	// opp-suspend kacke ....
	if (df->suspended)
		dev_err(dev, "%s while suspended ??\n", __func__);

	unsigned long curr_freq = get_freq(gpu);
	if (*freq == curr_freq)
		return 0;

	/*
	 * Note that devfreq_recommended_opp() can modify the freq
	 * to something that actually is in the opp table:
	 */
	unsigned long fff = *freq;
	opp = devfreq_recommended_opp(dev, freq, flags);
	if (IS_ERR(opp))
		return PTR_ERR(opp);


	if (*freq == curr_freq)
		return 0;

	dev_dbg(dev, "%s %lu => %lu (%lu)\n", __func__, get_freq(gpu), *freq, fff);

	trace_msm_gpu_freq_change(dev_pm_opp_get_freq(opp));

	if (gpu->funcs->gpu_set_freq) {
		mutex_lock(&df->lock);
		gpu->funcs->gpu_set_freq(gpu, opp, df->suspended);
		mutex_unlock(&df->lock);
	} else {
		dev_pm_opp_set_rate(dev, *freq);
	}

	dev_pm_opp_put(opp);

	return 0;
}

static unsigned long get_freq(struct msm_gpu *gpu)
{
	if (gpu->funcs->gpu_get_freq)
		return gpu->funcs->gpu_get_freq(gpu);

	return clk_get_rate(gpu->core_clk);
}

static int msm_devfreq_get_dev_status(struct device *dev,
		struct devfreq_dev_status *status)
{
	struct msm_gpu *gpu = dev_to_gpu(dev);
	struct msm_gpu_devfreq *df = &gpu->devfreq;
	u64 busy_cycles, busy_time;
	unsigned long sample_rate;
	ktime_t time;

	mutex_lock(&df->lock);

	status->current_frequency = get_freq(gpu);
	time = ktime_get();
	status->total_time = ktime_us_delta(time, df->time);
	df->time = time;

	if (df->suspended) {
		mutex_unlock(&df->lock);
		status->busy_time = 0;
		return 0;
	}

	busy_cycles = gpu->funcs->gpu_busy(gpu, &sample_rate);
	busy_time = busy_cycles - df->busy_cycles;
	df->busy_cycles = busy_cycles;

	mutex_unlock(&df->lock);

	busy_time *= USEC_PER_SEC;
	busy_time = div64_ul(busy_time, sample_rate);
	if (WARN_ON(busy_time > ~0LU))
		busy_time = ~0LU;

	status->busy_time = busy_time;

	dev_dbg(&gpu->pdev->dev,
		"busy %lu / total %lu = %lu | freq %lu MHz bscy: %llu | srate: %lu\n",
		status->busy_time, status->total_time,
		status->busy_time / (status->total_time / 100),
		status->current_frequency / 1000 / 1000,
		busy_cycles,
		sample_rate);


	return 0;
}

static int msm_devfreq_get_cur_freq(struct device *dev, unsigned long *freq)
{
	*freq = get_freq(dev_to_gpu(dev));

	return 0;
}

static struct devfreq_dev_profile msm_devfreq_profile = {
	.timer = DEVFREQ_TIMER_DELAYED,
	.polling_ms = 50,
	.target = msm_devfreq_target,
	.get_dev_status = msm_devfreq_get_dev_status,
	.get_cur_freq = msm_devfreq_get_cur_freq,
};

static bool has_devfreq(struct msm_gpu *gpu)
{
	struct msm_gpu_devfreq *df = &gpu->devfreq;
	return !!df->devfreq;
}

void msm_devfreq_init(struct msm_gpu *gpu)
{
	struct msm_gpu_devfreq *df = &gpu->devfreq;
	struct msm_drm_private *priv = gpu->dev->dev_private;

	/* We need target support to do devfreq */
	if (!gpu->funcs->gpu_busy)
		return;

	/*
	 * Setup default values for simple_ondemand governor tuning.  We
	 * want to throttle up at 50% load for the double-buffer case,
	 * where due to stalling waiting for vblank we could get stuck
	 * at (for ex) 30fps at 50% utilization.
	 */
	priv->gpu_devfreq_config.upthreshold = 50;
	priv->gpu_devfreq_config.downdifferential = 10;

	mutex_init(&df->lock);

	msm_devfreq_profile.initial_freq = gpu->fast_rate;

	/*
	 * Don't set the freq_table or max_state and let devfreq build the table
	 * from OPP
	 * After a deferred probe, these may have be left to non-zero values,
	 * so set them back to zero before creating the devfreq device
	 */
	msm_devfreq_profile.freq_table = NULL;
	msm_devfreq_profile.max_state = 0;

	df->devfreq = devm_devfreq_add_device(&gpu->pdev->dev,
			&msm_devfreq_profile, DEVFREQ_GOV_SIMPLE_ONDEMAND,
			&priv->gpu_devfreq_config);

	if (IS_ERR(df->devfreq)) {
		DRM_DEV_ERROR(&gpu->pdev->dev, "Couldn't initialize GPU devfreq\n");
		df->devfreq = NULL;
		return;
	}

	devfreq_suspend_device(df->devfreq);

	gpu->cooling = of_devfreq_cooling_register(gpu->pdev->dev.of_node, df->devfreq);
	if (IS_ERR(gpu->cooling)) {
		DRM_DEV_ERROR(&gpu->pdev->dev,
				"Couldn't register GPU cooling device\n");
		gpu->cooling = NULL;
	}

}

void msm_devfreq_cleanup(struct msm_gpu *gpu)
{
	struct msm_gpu_devfreq *df = &gpu->devfreq;

	if (!has_devfreq(gpu))
		return;

	devm_devfreq_remove_device(&gpu->pdev->dev, df->devfreq);
	devfreq_cooling_unregister(gpu->cooling);
}

void msm_devfreq_resume(struct msm_gpu *gpu)
{
	struct msm_gpu_devfreq *df = &gpu->devfreq;
	unsigned long sample_rate;

	if (!has_devfreq(gpu))
		return;

	mutex_lock(&df->lock);
	df->busy_cycles = gpu->funcs->gpu_busy(gpu, &sample_rate);
	df->time = ktime_get();
	df->suspended = false;
	mutex_unlock(&df->lock);

	devfreq_resume_device(df->devfreq);
}

void msm_devfreq_suspend(struct msm_gpu *gpu)
{
	struct msm_gpu_devfreq *df = &gpu->devfreq;

	if (!has_devfreq(gpu))
		return;

	mutex_lock(&df->lock);
	df->suspended = true;
	mutex_unlock(&df->lock);

	devfreq_suspend_device(df->devfreq);
}

