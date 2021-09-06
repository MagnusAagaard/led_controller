#!/usr/bin/python3
import sys
import rospy
from std_msgs.msg import String
from led_controller.msg import Color

class LEDController:
    def __init__(self):
        self.mir_robot = rospy.get_param('~mir_robot',False)
        self._init_status_msgs()
        self._init_subscribers()
        self._init_publishers()
        self.status = 'FREE'
        self.state_id = 0

    def _init_subscribers(self):
        status_topic = rospy.get_param('~status_topic','')
        if not status_topic:
            rospy.logerr('Parameter \'status_topic\' is not provided.')
            sys.exit(-1)
        
        rospy.Subscriber(status_topic, String, self._callback, queue_size=1)
        if self.mir_robot:
            rospy.Subscriber('/mir_interfacer/state_id', String, self._callback_state_id, queue_size=1)

    def _callback_state_id(self, msg):
        self.state_id = int(msg.data)

    def _callback(self, msg):
        self.status = self.status_msg_dict.get(int(msg.data[0]))
        #print(self.status)
        # If we have MiR robot, check for emergency stop etc.
        if self.mir_robot:
            if self.state_id != 3:
                self.status = self.status_msg_dict_mir.get(self.state_id)
        color_msg = Color()
        if self.status == 'FREE':
            color_msg.r = 0
            color_msg.g = 0
            color_msg.b = 0
        elif self.status == 'BUSY' or self.status == 'EXECUTING':
            color_msg.r = 0
            color_msg.g = 0
            color_msg.b = 255
        elif self.status == 'ARRIVED':
            color_msg.r = 0
            color_msg.g = 255
            color_msg.b = 0
        elif self.status == 'PAUSE':
            color_msg.r = 255
            color_msg.g = 255
            color_msg.b = 0
        elif self.status == 'EMERGENCY_STOP':
            color_msg.r = 255
            color_msg.g = 0
            color_msg.b = 0
        elif self.status == 'STARTING':
            color_msg.r = 1
            color_msg.g = 1
            color_msg.b = 1
        else:
            #rospy.logerr('Unknown status recieved!')
            color_msg.r = 255
            color_msg.g = 0
            color_msg.b = 255

        self.status_publisher.publish(color_msg)


    def _init_status_msgs(self):
        self.status_msg_dict = {
                    1 : 'FREE',
                    2 : 'BUSY',
                    3 : 'ARRIVED'
                }
        if self.mir_robot:
            self.status_msg_dict_mir = {
                0 : 'STARTING',
                3 : 'RUNNING',
                4 : 'PAUSE',
                5 : 'EXECUTING',
                10 : 'EMERGENCY_STOP',
                11 : 'MANUAL_CONTROL'
            }

    def _init_publishers(self):
        self.status_publisher = rospy.Publisher('~status', Color, queue_size=1)


def main():
    rospy.init_node('led_controller', log_level=rospy.INFO)
    controller = LEDController()
    rospy.spin()


if __name__ == "__main__":
    main()