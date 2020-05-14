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

float UA_LigamentLength::get_del_R_px(std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> ll_pts, float OpenCV_rel_err)
{
	float del_R_px;
	
	// first initialise points in tuple as local variables
	float rpx_x = std::get<0>(ll_pts).x;
	float rpx_y = std::get<0>(ll_pts).y;
	float bx = std::get<1>(ll_pts).x;
	float by = std::get<1>(ll_pts).y;

	// next, calculate absolute errors in tuple values, assumed they all have same rel error since OpenCV obtained values
	float del_rpx_x = OpenCV_rel_err * rpx_x;
	float del_rpx_y = OpenCV_rel_err * rpx_y;
	float del_bx = OpenCV_rel_err * bx;
	float del_by = OpenCV_rel_err * by;

	// now we calculate error in radius squared, R2. This is done to make the maths easier.
	float R2 = pow((bx - rpx_x), 2.0); +pow((by - rpx_y), 2.0);
	// partial differentials
	float dR2_dbx = (2*bx - 2*rpx_x);	// dR2/dbx
	float dR2_dby = (2*by - 2*rpx_y);
	float dR2_drpx_x = (2 * rpx_x - 2 * bx);
	float dR2_drpx_y = (2 * rpx_y - 2 * by);
	// calculate error in R2
	float del_R2 = sqrt(pow((dR2_dbx*del_bx), 2.0) + pow((dR2_dby * del_by), 2.0) + pow((dR2_drpx_x * del_rpx_x), 2.0) + pow((dR2_drpx_y * del_rpx_y), 2.0));

	// finally, we can determine error in radius R in px
	// differentials
	float dR_dR2 = 1 / (2 * sqrt(R2));
	del_R_px = dR_dR2 * del_R2;


	return del_R_px;
}

std::tuple<float, float> UA_LigamentLength::get_del_P1_x_y(cv::Point2f rpx, float OpenCV_rel_err, float del_R_px)
{
	float P1x = rpx.x;
	float del_P1x = OpenCV_rel_err*P1x;
	
	float del_P1y = sqrt(pow((OpenCV_rel_err*rpx.y), 2.0) + pow( del_R_px, 2.0));

	return std::tuple<float, float>(del_P1x, del_P1y);
}

std::tuple<float, float> UA_LigamentLength::get_del_P2_x_y(cv::Point2f rpx, float OpenCV_rel_err, float radius, float del_R_px, float h)
{
	float del_P2x = OpenCV_rel_err * rpx.x;

	float del_rpx_y = OpenCV_rel_err * rpx.y;
	float del_R = del_R_px;
	float del_h = OpenCV_rel_err * h;
	float del_P2y = sqrt(pow(del_rpx_y, 2.0) + pow(del_h, 2.0) + pow(-1*del_R, 2.0));

	return std::tuple<float, float>(del_P2x, del_P2y);
}

float UA_LigamentLength::get_del_L_px(std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> ll_pts, std::tuple<float, float> del_P1, std::tuple<float, float> del_P2)
{
	float del_L_px = 0;

	// initialise points as local variables
	float P1x = std::get<2>(ll_pts).x;
	float P1y = std::get<2>(ll_pts).y;
	float P2x = std::get<3>(ll_pts).x;
	float P2y = std::get<3>(ll_pts).y;

	// initialise uncertainties as variables
	float del_P1x = std::get<0>(del_P1);
	float del_P1y = std::get<1>(del_P1);
	float del_P2x = std::get<0>(del_P2);
	float del_P2y = std::get<1>(del_P2);

	// variables for calculation
	// using ligament length squared, L2 for easier maths
	float L2 = pow((P1x - P2x), 2.0) + pow((P1y - P2y), 2.0);
	// differentiables
	float dL2_dP1x = 2 * P1x - 2 * P2x;
	float dL2_dP1y = 2 * P1y - 2 * P2y;
	float dL2_dP2x = 2 * P2x - 2 * P1x;
	float dL2_dP2y = 2 * P2y - 2 * P1y;
	// uncertainty in L2
	float del_L2 = sqrt(pow((dL2_dP1x*del_P1x), 2.0) + pow((dL2_dP1y * del_P1y), 2.0) + pow((dL2_dP2x * del_P2x), 2.0) + pow((dL2_dP2y * del_P2y), 2.0));

	// now we get the uncertainty in L (px)
	float dL_dL2 = 1 / (2 * sqrt(L2));
	del_L_px = dL_dL2 * del_L2;

	return del_L_px;
}

float UA_LigamentLength::get_del_liglen(std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f> ll_pts, float OpenCV_rel_err, float dy, float del_dy)
{
	float del_ll = 0;
	
	// prelim stuff, calculate radis of main drop head
	float head_radius = cv::norm(std::get<0>(ll_pts) - std::get<1>(ll_pts));
	cv::Point2f rpx = std::get<0>(ll_pts);
	cv::Point2f P2 = std::get<3>(ll_pts);
	float h = P2.y + head_radius - rpx.y;

	// 1. Get error in Radius calculation (px)
	float del_R_px = get_del_R_px(ll_pts, OpenCV_rel_err);

	// 2. Get error in x and y components of P1
	std::tuple<float, float> del_P1 = get_del_P1_x_y(rpx, OpenCV_rel_err, del_R_px);

	// 3. Get error in x and y components of P2
	std::tuple<float, float> del_P2 = get_del_P2_x_y(rpx, OpenCV_rel_err, head_radius, del_R_px, h);

	// 4. Get error in L (px)
	float del_L_px = get_del_L_px(ll_pts, del_P1, del_P2);

	// 5. Get error in L (mm)
	float Lpx = cv::norm(std::get<2>(ll_pts) - std::get<3>(ll_pts));
	float dLmm_ddy = Lpx;
	float dLmm_dLpx = dy;
	del_ll = sqrt(pow(dLmm_ddy * del_dy, 2.0) + pow(dLmm_dLpx * del_L_px, 2.0));

	return del_ll;
}

float UA_Volume::get_del_di(float X, float dx, float del_X, float del_dx)
{
	// differentiables
	float ddi_dX = dx;
	float ddi_ddx = X;

	// calculate error
	float del_di = sqrt(pow((ddi_dX*del_X),2.0) + pow((ddi_ddx*del_dx), 2.0));
	
	return del_di;
}

float UA_Volume::get_del_Vi(float di, float H, float del_di, float del_H)
{
	// differentiables
	float dVi_dH = PI * di;
	float dVi_ddi = PI * H;

	// calculate uncertainty in Vi
	float del_Vi = sqrt(pow((dVi_dH*del_H), 2.0) + pow((dVi_ddi*del_di), 2.0));

	return del_Vi;
}
