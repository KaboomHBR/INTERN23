#include <ros/ros.h>
#include <find_object_2d/ObjectsStamped.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <actionlib_msgs/GoalID.h>

// to cancel move_base, in terminal:
// rostopic pub -1 /move_base/cancel actionlib_msgs/GoalID -- {}
            /*rostopic info move_base/cancel
             * Type: actionlib_msgs/GoalID
             */
            /*rosmsg show actionlib_msgs/GoalID
             * time stamp
             * string id
             */
//ROS_INFO("rosnode kill move_base");
/* now gotta publish the same info to this topic, but achieved by a publisher
 * the activation of this publisher should based on the condition of the 
 * topic its node subscribe, which is "object" from the find_object_2d ros pkg
 * to achieve writing a node that contains subcriber and publisher:
 * use class object to make the code clean
 */

// to save an image, in terminal:
// rosservice call /image_saver/save
            /*rosservice info /image_saver/save
             * Node: /image_saver
             * URI: rosrpc://samuel-HP-Pavilion-Laptop-14-ce0xxx:56343
             * Type: std_srvs/Empty
             * Args: 
             */
            


class SubscribeAndPublish
{
public:
  SubscribeAndPublish()
  {
    pub = nh.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1);
    sub = nh.subscribe("/objects", 1, &SubscribeAndPublish::objectsDetectedCallback, this);
  }

  void objectsDetectedCallback(const std_msgs::Float32MultiArrayConstPtr & msg)
  {
    //handle the "object" topic
    printf("---\n");
    const std::vector<float> & data = msg->data;
    if(data.size())
    {
        actionlib_msgs::GoalID empty_goal;
        pub.publish(empty_goal);
        //ROS_INFO("rosnode move_base should be cancelled");
        printf("stop\n");
    }
  }

private:
  ros::NodeHandle nh; 
  ros::Publisher pub;
  ros::Subscriber sub;
  ros::ServiceClient client; // for taking a picture, rosservice call /image_saver/save, but it does not work very well since spin() would makes it generating infinite amount of pictures 
};//End of class SubscribeAndPublish


int main(int argc, char** argv)
{
    ros::init(argc, argv, "objects_detected");
    
    SubscribeAndPublish objects_detected; // doesn't need to be the exact name tho

    ros::spin();

    return 0;
}
