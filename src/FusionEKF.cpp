#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;


// Constructor.
FusionEKF::FusionEKF() {
    is_initialized_ = false;

    previous_timestamp_ = 0;

    // Initializing matrices
    
    R_laser_ = MatrixXd(2, 2);
    R_radar_ = MatrixXd(3, 3);
    H_laser_ = MatrixXd(2, 4);
    Hj_      = MatrixXd(3, 4);
    
    ekf_.F_ = MatrixXd(4, 4);
    ekf_.P_ = MatrixXd(4, 4);

    // Measurement covariance matrix - laser
    
    R_laser_ << 0.0225, 0,
                0, 0.0225;
    
    // Measurement covariance matrix - radar
    R_radar_ << 0.09, 0, 0,
                0, 0.0009, 0,
                0, 0, 0.09;

    /**
     * Finish initializing the FusionEKF.
     * Set the process and measurement noises
     */
    
    // Measurement matrix - laser
    H_laser_ << 1, 0, 0, 0,
                0, 1, 0, 0;

    // State covariance matrix P_
    ekf_.P_ <<  1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1000, 0,
                0, 0, 0, 1000;

    // The initial transition matrix F_
    ekf_.F_ <<  1, 0, 1, 0,
                0, 1, 0, 1,
                0, 0, 1, 0,
                0, 0, 0, 1;
  
}


// Destructor.
FusionEKF::~FusionEKF() {}


void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  
    /*****************************************************************************
    *  Initialization
    ****************************************************************************/
    if (!is_initialized_) {
        /**
         * Initialize the state ekf_.x_ with the first measurement.
         * Create the covariance matrix.
         * Remember: you'll need to convert radar from polar to cartesian coordinates.
        */
        
        // First measurement
        ekf_.x_ = VectorXd(4);
        ekf_.x_ << 1, 1, 1, 1;

        if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
            
            // Convert radar from polar to cartesian coordinates and initialize state.
            double rho = measurement_pack.raw_measurements_[0];
            double phi = measurement_pack.raw_measurements_[1];
            double rho_dot = measurement_pack.raw_measurements_[2];
            
            double px = rho * cos(phi);
            double py = rho * sin(phi);
      
            double vx = rho_dot * sin(phi);
            double vy = rho_dot * cos(phi);

            ekf_.x_ << px, py, vx, vy;
        }
        else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
            ekf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 0, 0;
        }
        
        
        previous_timestamp_ = measurement_pack.timestamp_;

        is_initialized_ = true;
        return;
    }
    
    /*****************************************************************************
     *  Prediction
     ****************************************************************************/

    /**
     TODO:
     * Update the state transition matrix F according to the new elapsed time.
       - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
    */

    // Compute the time elapsed between the current and previous measurements
    float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
    previous_timestamp_ = measurement_pack.timestamp_;

    ekf_.F_(0, 2) = dt;
    ekf_.F_(1, 3) = dt;
    
    float dt_2 = pow(dt, 2);
    float dt_3 = pow(dt, 3);
    float dt_4 = pow(dt, 4);

    float noise_ax = 9.;
    float noise_ay = 9.;
    
    ekf_.Q_ = MatrixXd(4, 4);
    ekf_.Q_ <<  dt_4/4*noise_ax, 0, dt_3/2*noise_ax, 0,
                0, dt_4/4*noise_ay, 0, dt_3/2*noise_ay,
                dt_3/2*noise_ax, 0, dt_2*noise_ax, 0,
                0, dt_3/2*noise_ay, 0, dt_2*noise_ay;
    
    ekf_.Predict();

    /*****************************************************************************
    *  Update
    ****************************************************************************/

    /**
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
    */
    
    VectorXd x_new = measurement_pack.raw_measurements_;
    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
        // Radar updates
        Tools tools;
        Hj_ = tools.CalculateJacobian(ekf_.x_);
        ekf_.H_ = Hj_;
        ekf_.R_ = R_radar_;
        ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    }
    else {
        // Laser updates
        ekf_.R_ = R_laser_;
        ekf_.H_ = H_laser_;
        ekf_.Update(x_new);
    }

    // print the output
    cout << "x_ = " << ekf_.x_ << endl;
    cout << "P_ = " << ekf_.P_ << endl;
}