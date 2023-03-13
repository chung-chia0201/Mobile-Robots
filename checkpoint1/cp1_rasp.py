#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Int32

def cp1_pub():
    pub = rospy.Publisher('chatter1', Int32, queue_size=10)
    rospy.init_node('cp1_pub', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    pro_sub = Int32()
    while not rospy.is_shutdown():
        pro_input = raw_input()
        rospy.loginfo("user's input is: %s",pro_input)
        pro_sub.data = int(pro_input)
        pub.publish(pro_sub)
        rate.sleep()

def callback(data):
    rospy.loginfo("message from Arduino is %s", data)

def cp1_sub():
    rospy.Subscriber("chatter2", Int32, callback)

if __name__ == '__main__':
    print("start")
    cp1_sub()
    try:
        cp1_pub()
    except rospy.ROSInterruptException:
        pass
    
    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()