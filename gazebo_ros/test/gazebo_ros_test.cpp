#include <gazebo_msgs/ModelStates.h>
#include <gtest/gtest.h>
#include <ros/ros.h>
#include <rosgraph_msgs/Clock.h>

class GazeboRosTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    has_new_stamp_ = false;
  }

  ros::NodeHandle nh_;
  ros::Subscriber model_states_sub_;
  bool has_new_stamp_;
  ros::Time stamp_;
public:
  void clockCallback(const rosgraph_msgs::ClockConstPtr& msg)
  {
    stamp_ = msg->clock;
    has_new_stamp_ = true;
  }
};

// Test if the clock is published at all, and that the timestamp
// is not too long in the past.
TEST_F(GazeboRosTest, clockSubscribeTest)
{
  model_states_sub_ = nh_.subscribe("clock", 1,
                          &GazeboRosTest::clockCallback,
                          dynamic_cast<GazeboRosTest*>(this));

  while (!has_new_stamp_)
  {
    ros::spinOnce();
    ros::Duration(0.1).sleep();
  }

  // This check depends on the update period being much longer
  // than the expected difference between now and the received image time
  // TODO(lucasw)
  // this likely isn't that robust - what if the testing system is really slow?
  double time_diff = (ros::Time::now() - stamp_).toSec();
  ROS_INFO_STREAM(time_diff);
  EXPECT_LT(time_diff, 0.5);
  model_states_sub_.shutdown();
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "gazebo_ros_test");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
