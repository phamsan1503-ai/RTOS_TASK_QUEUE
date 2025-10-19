# RTOS_QUEUE_TASK
+ YÊU CẦU:
Viết chương trình sử dụng FreeRTOS có tối thiểu 2 tác vụ:
- Task 1: Thực hiện Blink LED theo tần số, độ rộng xung nhận được qua Queue.
- Task 2: Định kỳ tạo ra tần số và độ rộng xung thay đổi và gửi sang Task 1 qua Queue.
2 Task hoạt động song song, kết quả LED nhấp nháy với tần số, độ rộng xung khác nhau theo thời gian

## Giới thiệu chương trình

Trong chương trình này, em đã xây dựng một ứng dụng điều khiển LED sử dụng hệ điều hành thời gian thực FreeRTOS trên vi điều khiển STM32. Chương trình gồm hai tác vụ (task) hoạt động song song và trao đổi dữ liệu với nhau thông qua hàng đợi (queue):

- Task 1 – Nhiệm vụ nháy LED (Blink Task):
Có chức năng nhận dữ liệu tần số (frequency) và độ rộng xung (duty cycle) từ hàng đợi, sau đó điều khiển chân GPIO để LED nhấp nháy đúng theo các thông số này. Tần số và chu kỳ làm việc có thể thay đổi trong quá trình chạy, giúp LED hiển thị hiệu ứng nhấp nháy linh hoạt.

- Task 2 – Nhiệm vụ phát tín hiệu điều khiển (Signal Task):
Định kỳ (mỗi 5 giây) tạo ra các giá trị tần số và độ rộng xung khác nhau, rồi gửi sang Task 1 thông qua hàng đợi. Nhờ vậy, LED sẽ tự động thay đổi tốc độ và độ sáng tương đối theo thời gian mà không cần can thiệp bên ngoài.
Việc chia chương trình thành hai tác vụ riêng biệt giúp tận dụng khả năng đa nhiệm song song của FreeRTOS, làm cho hệ thống dễ mở rộng, dễ bảo trì và mô phỏng được ứng dụng điều khiển thời gian thực. Kết quả thực tế là LED nhấp nháy với tần số và độ rộng xung thay đổi sau mỗi 5 giây, thể hiện rõ sự phối hợp giữa hai tác vụ thông qua cơ chế truyền thông tin bằng Queue của FreeRTOS.
## Công cụ sử dụng
- Ngôn ngữ: C
- Vi điều khiển: STM32 
- IDE: Keil C, STM32CUBEMX
