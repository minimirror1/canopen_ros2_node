#include <rclcpp/rclcpp.hpp>
#include "Easy_CANopen.h"

class CANopenNode : public rclcpp::Node
{
public:
  CANopenNode() : Node("canopen_node")
  {
    // Easy-CANopen 초기화 및 설정
    canopen_ = new CANopen();
    
    // 하트비트 간격 설정 (예: 1000ms)
    Easy_CANopen_This_Node_Configure_Heartbeat_Interval(canopen_, 1000);
    
    // 부팅 메시지 브로드캐스트
    Easy_CANopen_This_Node_Broadcast_Bootup_Message(canopen_);
    
    // 마스터 및 슬레이브 노드 초기화
    master_node_ = {0};
    slave_node_ = {0};
    
    // 노드 설정 활성화
    Easy_CANopen_Other_Node_Activate_Node_Configuration(true);
    
    // 식별자 설정
    uint32_t vendor_ID = 10003;
    uint32_t product_code = 65467;
    uint32_t revision_number = 565;
    uint32_t serial_number = 3234;
    Easy_CANopen_Other_Node_Set_Vendor_ID_To_Node(&master_node_, vendor_ID);
    Easy_CANopen_Other_Node_Set_Product_Code_To_Node(&master_node_, product_code);
    Easy_CANopen_Other_Node_Set_Revision_Number_To_Node(&master_node_, revision_number);
    Easy_CANopen_Other_Node_Set_Serial_Number_To_Node(&master_node_, serial_number);
    
    // 주기적으로 실행될 타이머 설정
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(100),
      std::bind(&CANopenNode::timer_callback, this));
  }

  ~CANopenNode()
  {
    delete canopen_;
  }

private:
  void timer_callback()
  {
    // 슬레이브 노드에서 메시지 수신
    if (Easy_CANopen_Thread_Listen_Messages(&slave_node_)) {
      RCLCPP_INFO(this->get_logger(), "슬레이브 노드로부터 메시지 수신됨");
      RCLCPP_INFO(this->get_logger(), "Vendor ID = %i", slave_node_.od_communication.vendor_ID);
      RCLCPP_INFO(this->get_logger(), "Product code = %i", slave_node_.od_communication.product_code);
      RCLCPP_INFO(this->get_logger(), "Revision number = %i", slave_node_.od_communication.revision_number);
      RCLCPP_INFO(this->get_logger(), "Serial number = %i", slave_node_.od_communication.serial_number);
    }

    // 마스터 노드에서 메시지 수신
    if (Easy_CANopen_Thread_Listen_Messages(&master_node_)) {
      RCLCPP_INFO(this->get_logger(), "마스터 노드로부터 메시지 수신됨");
      RCLCPP_INFO(this->get_logger(), "Response is set: %i", master_node_.master.lss.selective_value_is_set);
    }

    // 하트비트 상태 브로드캐스트
    Easy_CANopen_This_Node_Broadcast_Heartbeat_Status(canopen_);

    // 메시지 전송
    Easy_CANopen_Thread_Transmit_Messages(canopen_);
  }

  CANopen* canopen_;
  CANopen master_node_;
  CANopen slave_node_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CANopenNode>());
  rclcpp::shutdown();
  return 0;
}
