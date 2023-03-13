#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import Int32

def cp4_pub():
    pub = rospy.Publisher('chatter_cmd', Int32, queue_size=10) 
    #create a publisher who is name is pub,publish topic(chatter_cmd), messange type is Int32 and queue size is 10.
    rospy.init_node('cp3_pub', anonymous=True)
    #initialize node, set node name is cp3_pub
    rate = rospy.Rate(10) # 10hz
    pro_pub = Int32()
    while not rospy.is_shutdown():
        pro_input_power = input("Set speed of mobilerobot:") 
        pro_pub.data = int(pro_input_power)
        pub.publish(pro_pub)
        rate.sleep()
        #the right wheel setting parameters are published by pub_1, and the left wheel setting parameters are published by pub_2

if __name__ == '__main__':
    print("start")
    #show start before starting
    try:
        cp4_pub()
    except rospy.ROSInterruptException:
        pass
    rospy.spin()
