#include <ros.h>
#include <std_msgs/Int32.h>
   
ros::NodeHandle nh;
std_msgs::Int32 num2;
ros::Publisher chatter2("chatter2", &num2);
   
void times2(const std_msgs::Int32 &num1){
    int num = num1.data*2;
    num2.data=num;
    chatter2.publish(&num2);
}

ros::Subscriber<std_msgs::Int32>chatter1("chatter1",&times2);
   
void setup()
{
    nh.initNode();
    nh.subscribe(chatter1);
    nh.advertise(chatter2);
}
   
void loop()
{
    nh.spinOnce();
    delay(1000);
}