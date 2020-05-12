#include "UncertaintyAnalysis.h"
// UA Images
std::tuple<float, float> UA_Images::get_del_img_w_h(std::vector<cv::Mat> grayscale_imgs)
{
	float del_img_w = 0;
	float del_img_h = 0;

	// first create a vector of Image Heights and Widths
	std::vector<int> ImageHeights;
	std::vector<int> ImageWidths;

	// initialise statistical variables
	float sigma_w = -1;		// std dev in image width
	float sigma_h = -1;		// std dev in image height
	float var_w = -1;
	float var_h = -1;
	float sum_w = 0;
	float sum_h = 0;
	float mean_w = -1;
	float mean_h = -1;
	float sum_var_w = 0;
	float sum_var_h = 0;

	int img_w;
	int img_h;

	// first loop to store values in ImageHeights and ImageWidths. Also
	// needed to determine mean.
	for (int i = 0; i < grayscale_imgs.size(); i++)
	{
		img_w = grayscale_imgs.at(i).size().width;
		img_h = grayscale_imgs.at(i).size().height;

		ImageHeights.push_back(grayscale_imgs.at(i).size().height);
		ImageWidths.push_back(grayscale_imgs.at(i).size().width);

		// calculating mean
		sum_w = sum_w + img_w;
		sum_h = sum_h + img_h;
	}

	mean_w = sum_w / grayscale_imgs.size();
	mean_h = sum_h / grayscale_imgs.size();

	// second loop to determine variance in image heights and widths, allowing for 
	// calculation of standard deviations
	for (int i = 0; i < grayscale_imgs.size(); i++)
	{
		img_w = grayscale_imgs.at(i).size().width;
		img_h = grayscale_imgs.at(i).size().height;

		// for width
		sum_var_w = sum_var_w + pow((img_w - mean_w), 2);

		// for height
		sum_var_h = sum_var_h + pow((img_h - mean_h), 2);
	}

	var_w = sum_var_w / grayscale_imgs.size();
	var_h = sum_var_h / grayscale_imgs.size();

	sigma_w = sqrt(var_w);
	sigma_h = sqrt(var_h);

	// now we get the uncertainties
	del_img_w = 2 * sigma_w;
	del_img_h = 2 * sigma_h;

	std::tuple<float, float> ret = std::make_tuple(del_img_w, del_img_h);
	return ret;
}

std::tuple<float, float> UA_Images::get_del_dx_dy(cv::Mat gray_img, double ROI_W, double ROI_H, float del_imgw, float del_imgh, float del_ROIw, float del_ROIh)
{
	float del_dx = -1;
	float del_dy = -1;

	// first get image width and height
	float img_w = gray_img.size().width;
	float img_h = gray_img.size().height;

	// now we calculate del_dx
	float df_dx_ROI_W = 1 / img_w;
	float df_dx_IMG_W = -ROI_W / pow(img_w, 2.0);
	del_dx = sqrt(pow(df_dx_ROI_W * del_ROIw, 2) + pow(df_dx_IMG_W * del_imgw, 2));

	// now we calclulate del_dy
	float df_dy_ROI_H = 1 / img_h;
	float df_dy_IMG_H = -ROI_H / pow(img_h, 2.0);
	del_dy = sqrt(pow(df_dy_ROI_H*del_ROIh,2) + pow(df_dy_IMG_H*del_imgh,2));

	std::tuple<float, float> ret = std::make_tuple(del_dx, del_dy);
	return ret;
}

float UA_Position::get_del_rmm(float rpx, float del_rpx_rel, float dy, float del_dy)
{
	float del_rmm = -1;
	float del_rpx = rpx * del_rpx_rel;
	del_rmm = sqrt(pow((rpx*del_dy),2.0) + pow((dy*del_rpx),2.0));

	return del_rmm;
}

// error in velocity calculations
float UA_Velocity::get_del_v(float v0_i, float delta_rmm_i, float delta_t_ms, float del_rmm_i, float del_t_i)
{
	float del_v = -1;
	
	// first we calculate v_r+
	float v_r = (delta_rmm_i + del_rmm_i) / delta_t_ms;		// m/s

	// next, we calculate v_t+
	float v_t = delta_rmm_i / (delta_t_ms + del_t_i);		// m/s

	// now we do intermediate calculations
	float C_r = abs(v_r - v0_i);
	float C_t = abs(v_t - v0_i);

	// finally we get the error in velocity
	del_v = sqrt(C_r * C_r + C_t * C_t);

	return del_v;
}
