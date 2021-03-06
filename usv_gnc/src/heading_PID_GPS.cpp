/*
################################################################################
Jan-09-2020, Mingxi, Code clean up, and remove the rpm averaging
                     changed topic name with standard prefix
                     m_imu, m_rpy, m_heading
Aug-20-2020,	Jianguang, get m_heading directed from GPSFix.msg published by the new gps driver.			 
*/
//include C language libraries
//extern "C" {
//#include "rc_usefulincludes.h"
//} 
extern "C" {
#include "roboticscape.h"
}
#include "gps_common/GPSFix.h"
#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>  // for atoi() and exit()
//#include <rc/mpu.h>
//#include <rc/time.h>
#include <sstream>
#include "ros/ros.h"
//include ros message types
//#include "sensor_msgs/Imu.h"
#include "sensor_msgs/MagneticField.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Vector3.h"
#include "std_msgs/Float64.h"
#include <tf/tf.h>
#include <cmath>
#include "sensor_msgs/Imu.h"
class HeadingPID
{
	geometry_msgs::Vector3 rpy_data; //roll pitch yaw data
  	std_msgs::Float64 heading_data;  //heading data
	std_msgs::Float64 stdb_pwm, port_pwm;
	float K_p, K_i,K_d,K_g,total_pwm, delta_pwm;
	float c_heading,i_heading, m_heading, d_heading, o_m_heading, e_heading;
	double ct, dt;
	float hz=10;
	public:
	HeadingPID()
	{
		stdb_thrust_pub = nh.advertise<std_msgs::Float64>("/gnc/stdb_cmd", 2);
		port_thrust_pub = nh.advertise<std_msgs::Float64>("/gnc/port_cmd", 2);
		imu_sub = nh.subscribe("/sensor/imu/data",2,&HeadingPID::imu_callback,this);
	  	gps_sub = nh.subscribe("/sensor/gps_data",2,&HeadingPID::gps_callback,this);
		c_heading_sub = nh.subscribe("/gnc/c_heading",2,&HeadingPID::c_heading_callback,this);
		//parameters
		//default
		K_p=1;
		K_i=0;
		K_d=0;
		K_g=0;
		i_heading = 0;
		total_pwm=1;	//max 2
		///
		ros::NodeHandle nh_priv("~");
		nh_priv.getParam("K_g",K_g);
		nh_priv.getParam("K_p",K_p);
		nh_priv.getParam("K_i",K_i);
		nh_priv.getParam("K_d",K_d);
		nh_priv.getParam("Total_thrust", total_pwm);

		ROS_INFO("Node started");
		ct = ros::Time::now().toSec();
	}

	void gps_callback(const gps_common::GPSFix& msg)
	{				
		m_heading = msg.track;
		ROS_INFO("m_heading[%f]", m_heading);
	}

	void imu_callback(const sensor_msgs::Imu::ConstPtr& msg)
	{
		d_heading = -msg->angular_velocity.z*RAD_TO_DEG;// added minus sign becaues error = c_heading -m_heading
	}

	void c_heading_callback(const std_msgs::Float64::ConstPtr& msg)
	{
		c_heading = msg->data;
		i_heading =0;
		//ROS_INFO("c_heading[%f]", c_heading);
	}

	void running()
	{
		ros::Rate loop_rate(hz);
		while(ros::ok())
		{
			e_heading = c_heading - m_heading;
			//check heading error ambiguity//
			if(e_heading >180)	
			{
				e_heading = e_heading -360;
			}
			if(e_heading <-180)
			{
				e_heading  = 360 + e_heading ;
			}

			//PID part
			dt = ros::Time::now().toSec()-ct; //compute delta t
			ct = ros::Time::now().toSec();//update time

			i_heading = i_heading+e_heading*dt; //compute integration error
			delta_pwm = K_g*(K_p*e_heading + K_i*i_heading + K_d*d_heading);
			//ROS_INFO("%f|%f|%f",delta_pwm,dt,ct);
			//stdb_pwm.data = total_pwm/2.0 + delta_pwm/2.0;
			stdb_pwm.data = total_pwm/2.0 + delta_pwm;
			port_pwm.data = total_pwm - stdb_pwm.data;

			//saturation
			if(stdb_pwm.data>1) stdb_pwm.data=1;
			if(stdb_pwm.data<0) stdb_pwm.data=0;
			if(port_pwm.data>1) port_pwm.data=1;
			if(port_pwm.data<0) port_pwm.data=0;
			//ROS_INFO("port_cmd,stdb_cmd[%f,%f]", port_pwm.data, stdb_pwm.data);
			//publish
			if(!std::isnan(port_pwm.data)&&!std::isnan(stdb_pwm.data))
			{
				//ROS_INFO("port_cmd,stdb_cmd[%f,%f]", port_pwm.data, stdb_pwm.data);
				stdb_thrust_pub.publish(stdb_pwm);
				port_thrust_pub.publish(port_pwm);
			}

			ros::spinOnce();
			loop_rate.sleep();
		}
	}

	void set_pwm()
	{

	}
	private:
		ros::NodeHandle nh;
		ros::Publisher stdb_thrust_pub;
		ros::Publisher port_thrust_pub;
		ros::Subscriber imu_sub;
		ros::Subscriber gps_sub;
		ros::Subscriber c_heading_sub;		
};


//global variables



int main(int argc, char** argv)
{
	// initialize a ros node (name used in ros context)
	ros::init(argc, argv, "headingPID");
	HeadingPID heading_PID;
	heading_PID.running();

  	return 0;
}
